// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "AttackSimulationManager.h"

#include "Engine/World.h"

#include "Uprising.h"
#include "HeadshotReceiverInterface.h"
#include "AttackDamageProcessor.h"
#include "Misc/Projectile.h"

UAttackSimulationManager::UAttackSimulationManager(FObjectInitializer const& OI)
{
	SimulationEventManager = CreateDefaultSubobject<UGameEventManager>("SimulationEventManager", true);
}

FGameEventHandle UAttackSimulationManager::StartSimulation(EAttackSimulationMode Mode, FAttackSimulationParams const& InParams, float SimulationTimeOut /*= -1.0f */)
{
	FGameEventHandle NewHandle;
	FAttackSimulationInfo Info; Info.InitialParams = InParams;
	StoredSimulationInfo.Add(NewHandle, Info);

	FScriptDelegate SimulationCompletedDelegate; SimulationCompletedDelegate.BindUFunction(this, "ReceiveSimulationCompleted");

	SimulationEventManager->RegisterGameEvent(NewHandle, SimulationCompletedDelegate, SimulationTimeOut);

	if (Mode == EAttackSimulationMode::RayTrace)
	{
		StartRayTraceSimulation(NewHandle, InParams);
	}

	else if (Mode == EAttackSimulationMode::PrimitiveTrace)
	{
		StartPrimitiveTraceSimulation(NewHandle, InParams);
	}

	else if (Mode == EAttackSimulationMode::Projectile)
	{
		StartProjectileSimulation(NewHandle, InParams);
	}

	else if (Mode == EAttackSimulationMode::PhysicsBased)
	{
		StartPhysicsBasedSimulation(NewHandle, InParams);
	}

	return NewHandle;
}

bool UAttackSimulationManager::StopSimulation(FGameEventHandle const& InSimulationHandle)
{
	return false;
}

bool UAttackSimulationManager::QuerySimulationInfo(FGameEventHandle const& InSimulationHandle, FAttackSimulationInfo &OutInfo) const
{
	if (InSimulationHandle.IsValid())
	{
		const FAttackSimulationInfo *InfoPtr = StoredSimulationInfo.Find(InSimulationHandle);
		if (InfoPtr)
		{
			OutInfo = *InfoPtr;

			return true;
		}
	}

	return false;
}

void UAttackSimulationManager::SetDamageProcessor(UAttackDamageProcessor *InProcessor)
{
	DamageProcessor = InProcessor;
}

UAttackDamageProcessor* UAttackSimulationManager::GetDamageProcessor() const
{
	return DamageProcessor;
}

bool UAttackSimulationManager::IsSimulationPaused(FGameEventHandle const& InSimulationHandle) const
{
	return SimulationEventManager->IsPaused(InSimulationHandle);
}

void UAttackSimulationManager::Pause(FGameEventHandle const& InSimulationHandle)
{
	if (!SimulationEventManager->IsPaused(InSimulationHandle))
	{
		SimulationEventManager->Pause(InSimulationHandle);

		ReceiveSimulationPaused(InSimulationHandle, true);
	}
}

void UAttackSimulationManager::PauseAll()
{
	if (!bPaused)
	{
		SimulationEventManager->PauseAll();
		bPaused = true;

		ReceiveAllSimulationPaused(bPaused);
	}
}

void UAttackSimulationManager::Unpause(FGameEventHandle const& InSimulationHandle)
{
	if (SimulationEventManager->IsPaused(InSimulationHandle))
	{
		SimulationEventManager->Unpause(InSimulationHandle);

		ReceiveSimulationPaused(InSimulationHandle, false);
	}
}

void UAttackSimulationManager::UnpauseAll()
{
	if (!bPaused)
	{
		SimulationEventManager->UnpauseAll();
		bPaused = false;

		ReceiveAllSimulationPaused(bPaused);
	}
}

void UAttackSimulationManager::NotifySimulationCompleted(FGameEventHandle &InSimulationHandle, TArray<FHitResult> const& InTotalHits)
{
	if (InSimulationHandle.IsValid())
	{
		if (SimulationEventManager->EventExists(InSimulationHandle) && !SimulationEventManager->IsEventExpired(InSimulationHandle))
		{
			SimulationEventManager->MarkCompleted(InSimulationHandle);
			LOG(AttackLog, "An attack simulation has been marked as completed");

			FAttackSimulationInfo *InfoPtr = StoredSimulationInfo.Find(InSimulationHandle);
			if (InfoPtr)
			{
				InfoPtr->SimulationHits = InTotalHits;
			}

			else
			{
				ERROR(AttackLog, "Current attack simulation has been marked as completed but failed to report HitResults due to the absence of AttackSimulationInfo");
			}
		}
	}

	else
	{
		WARNING(AttackLog, "An attack simulation has been completed but could not be handled due to an invalid handle");
	}
}

void UAttackSimulationManager::PostSimulationCompleted(FGameEventHandle &InSimulationHandle, float SimulationTime)
{
	if (InSimulationHandle.IsValid())
	{
		FAttackSimulationInfo Info;
		if (StoredSimulationInfo.RemoveAndCopyValue(InSimulationHandle, Info))
		{
			FAttackSimulationResult SimulationResult;
			Info.SimulationTime = SimulationTime;

			for (FHitResult const& Hit : Info.SimulationHits)
			{
				if (Hit.bBlockingHit)
				{
					if (!SimulationResult.bAnyHit)
					{
						SimulationResult.bAnyHit = true;
					}

					AActor *HitActor = Hit.GetActor();
					if (HitActor && Cast<UHeadshotReceiverInterface>(HitActor))
					{
						if (IHeadshotReceiverInterface::Execute_IsHeadshot(HitActor, Hit))
						{
							SimulationResult.HeadHits.Add(Hit);
						}
					}
				}
			}

			ReceiveSimulationCompleted(Info, SimulationResult);
			Info.InitialParams.CompletionHandler.ExecuteIfBound(SimulationResult);

			InSimulationHandle.Invalidate();

			PostSimulationProcess(Info, SimulationResult);
		}

		else
		{
			ERROR(AttackLog, "A simulation handle is valid but failed to look up info for the handle");
		}
	}

	else
	{
		WARNING(AttackLog, "An attack simulation has been completed but could not be handled due to an invalid handle");
	}
}

void UAttackSimulationManager::NotifySimulationFailed(FGameEventHandle &InSimulationHandle, ESimulationFailureReason::Type FailureReason)
{
	if (InSimulationHandle.IsValid() && SimulationEventManager->EventExists(InSimulationHandle))
	{
		if (!SimulationEventManager->IsEventExpired(InSimulationHandle))
		{
			SimulationEventManager->MarkExpired(InSimulationHandle);
		}

		FAttackSimulationInfo Info;
		StoredSimulationInfo.RemoveAndCopyValue(InSimulationHandle, Info);

		InSimulationHandle.Invalidate();

		ReceiveSimulationFailed(FailureReason, Info);
		Info.InitialParams.FailureHandler.ExecuteIfBound(FailureReason);

		ERROR(AttackLog, "An attack simulation has failed due to %s", FailureReason);
	}
}

void UAttackSimulationManager::ConvertSimulationQueryParams(FAttackSimulationParams const& InSimulationParams, FCollisionObjectQueryParams &OutObjQueryParams, FCollisionQueryParams &OutQueryParams)
{
	for (ECollisionChannel Type : DamageableTypes)
	{
		OutObjQueryParams.AddObjectTypesToQuery(Type);
	}

	OutQueryParams.bReturnPhysicalMaterial = InSimulationParams.bReceivePhysicsMaterialResponse;
	OutQueryParams.bTraceComplex = InSimulationParams.bUsePreciseSimulation;
	OutQueryParams.AddIgnoredActors(InSimulationParams.IgnoredActors);
	OutQueryParams.AddIgnoredComponents(InSimulationParams.IgnoredComponents);
}

void UAttackSimulationManager::PostSimulationProcess(FAttackSimulationInfo &InSimulationInfo, FAttackSimulationResult &InSimulationResult)
{
	if (InSimulationResult.bAnyHit && DamageProcessor)
	{
		for (FHitResult const& Hit : InSimulationResult.Hits)
		{
			DamageProcessor->ResolveAttack(InSimulationInfo, Hit);
		}

		for (FHitResult const& HeadHit : InSimulationResult.HeadHits)
		{
			DamageProcessor->ResolveHeadshot(InSimulationInfo, HeadHit);
		}
	}
}

void UAttackSimulationManager::StartRayTraceSimulation(FGameEventHandle &InSimulationHandle, FAttackSimulationParams const& InParams)
{
	if (InParams.SimulatedWorld)
	{
		if (InParams.Direction.IsZero())
		{
			NotifySimulationFailed(InSimulationHandle, ESimulationFailureReason::InvalidSimulationParams);
		}

		TArray<FHitResult> Hits;

		FCollisionObjectQueryParams ObjQueryParams; FCollisionQueryParams QueryParams;
		ConvertSimulationQueryParams(InParams, ObjQueryParams, QueryParams);

		InParams.SimulatedWorld->LineTraceMultiByObjectType(Hits, InParams.Start, InParams.Start + InParams.Direction * InParams.MaxDistance, ObjQueryParams, QueryParams);

		NotifySimulationCompleted(InSimulationHandle, Hits);
	}

	else
	{
		NotifySimulationFailed(InSimulationHandle, ESimulationFailureReason::InvalidWorldInstance);
	}
}

void UAttackSimulationManager::StartPrimitiveTraceSimulation(FGameEventHandle &InSimulationHandle, FAttackSimulationParams const& InParams)
{
	if (InParams.SimulatedWorld)
	{
		if (InParams.Direction.IsZero())
		{
			NotifySimulationFailed(InSimulationHandle, ESimulationFailureReason::InvalidSimulationParams);
		}

		TArray<FHitResult> Hits;

		FCollisionObjectQueryParams ObjQueryParams; FCollisionQueryParams QueryParams;
		ConvertSimulationQueryParams(InParams, ObjQueryParams, QueryParams);

		InParams.SimulatedWorld->SweepMultiByObjectType(Hits, InParams.Start, InParams.Start + InParams.Direction * InParams.MaxDistance, InParams.Rotation.Quaternion(), ObjQueryParams, InParams.Shape, QueryParams);

		NotifySimulationCompleted(InSimulationHandle, Hits);
	}

	else
	{
		NotifySimulationFailed(InSimulationHandle, ESimulationFailureReason::InvalidWorldInstance);
	}
}

void UAttackSimulationManager::StartProjectileSimulation(FGameEventHandle &InSimulationHandle, FAttackSimulationParams const& InParams)
{
	if (InParams.SimulatedWorld)
	{
		FAttackSimulationInfo Info;
		QuerySimulationInfo(InSimulationHandle, Info);
		
		FActorSpawnParameters ProjectileSpawnParams;
		ProjectileSpawnParams.Name = *FString::Format(TEXT("ASProjectile_%i"), { FStringFormatArg(InSimulationHandle.Value()) });
		ProjectileSpawnParams.Owner = GetTypedOuter<AActor>();
		ProjectileSpawnParams.Instigator = InParams.DamageInstigator;
		ProjectileSpawnParams.bNoFail = true;
		ProjectileSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Info.ProjectileInstance = InParams.SimulatedWorld->SpawnActor<AProjectile>(InParams.ProjectileClass, InParams.Start, InParams.Rotation, ProjectileSpawnParams);
		
		if (Info.ProjectileInstance)
		{
			if (!InParams.Direction.IsZero())
			{
				Info.ProjectileInstance->SetDirection(InParams.Direction);
			}

			Info.ProjectileHitDelegate.BindUFunction(this, "ReceiveProjectileHit");
			Info.ProjectileInstance->OnActorHit.AddUnique(Info.ProjectileHitDelegate);

			if (Info.ProjectileInstance->CanLaunch())
			{
				Info.ProjectileInstance->Launch();
			}
		}

		else
		{
			NotifySimulationFailed(InSimulationHandle, ESimulationFailureReason::FailedToSpawn);
		}
	}

	else
	{
		NotifySimulationFailed(InSimulationHandle, ESimulationFailureReason::InvalidWorldInstance);
	}
}

void UAttackSimulationManager::StartPhysicsBasedSimulation(FGameEventHandle &InSimulationHandle, FAttackSimulationParams const& InParams)
{
	// does nothing for now
}

void UAttackSimulationManager::ReceiveProjectileHit(AActor *Projectile, AActor *HitActor, FVector NormalImpulse, FHitResult const& Hit)
{
	FGameEventHandle SimulationHandle;
	FAttackSimulationInfo const* InfoPtr;

	for (auto It = StoredSimulationInfo.CreateConstIterator(); It; ++It)
	{
		if (It.Value().ProjectileInstance == Projectile)
		{
			InfoPtr = &It.Value();
		}
	}

	if (SimulationHandle.IsValid() && InfoPtr)
	{
		Projectile->OnActorHit.Remove(InfoPtr->ProjectileHitDelegate);
		NotifySimulationCompleted(SimulationHandle, { Hit });
	}
}

void UAttackSimulationManager::ReceiveSimulationCallback(FGameEventHandle &InSimulationHandle, bool bCompleted, float ElapsedTime)
{
	if (InSimulationHandle.IsValid())
	{
		FAttackSimulationInfo *InfoPtr = StoredSimulationInfo.Find(InSimulationHandle);
		if (InfoPtr)
		{
			InfoPtr->SimulationTime = ElapsedTime;

			if (bCompleted)
			{
				PostSimulationCompleted(InSimulationHandle, ElapsedTime);
			}
			
			else
			{
				InfoPtr->InitialParams.FailureHandler.ExecuteIfBound(ESimulationFailureReason::TimeOut);
			}
		}
	}
}

void UAttackSimulationManager::Tick(float DeltaTime)
{
	for (auto It = StoredSimulationInfo.CreateIterator(); It; ++It)
	{
		FAttackSimulationInfo Info = It.Value();

		if (Info.InitialParams.bUseMaxDistance && Info.ProjectileInstance)
		{
			float Distance = (Info.ProjectileInstance->GetActorLocation() - Info.InitialParams.Start).Size();
			if (Distance >= Info.InitialParams.MaxDistance)
			{
				Info.ProjectileInstance->Destroy(true);
			}
		}
	}
}

bool UAttackSimulationManager::IsTickable() const
{
	return !bPaused;
}

TStatId UAttackSimulationManager::GetStatId() const
{
	return GetStatId();
}

UWorld* UAttackSimulationManager::GetTickableGameObjectWorld() const
{
	return GetWorld();
}