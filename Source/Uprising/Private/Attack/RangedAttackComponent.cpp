// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "RangedAttackComponent.h"
#include "Client.h"
#include "RangedWeapon.h"
#include "Misc/DamageMisc.h"
#include "Hero/HeroCharacter.h"
#include "UI/ZPlayerHUD.h"

URangedAttackComponent::URangedAttackComponent(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{

}

void URangedAttackComponent::DrawDebugSimulationResult(FAttackSimulationResult const& InSimulationResult, FVector const& Start, FVector const& End, FColor TraceColor, FColor TraceHitColor, float DrawTime, float PointSize) const
{
	UWorld *World = GetWorld();

	if (World)
	{
		DrawDebugPoint(World, Start, PointSize, FColor::Blue, false, DrawTime);
		DrawDebugPoint(World, End, PointSize, FColor::Blue, false, DrawTime);

		if (InSimulationResult.bAnyHit && InSimulationResult.AttackResults.Last().HitInfo.bBlockingHit)
		{
			FVector const BlockingHitPoint = InSimulationResult.AttackResults.Last().HitInfo.ImpactPoint;

			DrawDebugLine(World, Start, BlockingHitPoint, TraceColor, false, DrawTime);
			DrawDebugLine(World, BlockingHitPoint, End, TraceHitColor, false, DrawTime);
		}

		else
		{
			DrawDebugLine(World, Start, End, TraceColor, false, DrawTime);
		}

		for (int32 i = 0; i < InSimulationResult.AttackResults.Num(); i++)
		{
			FHitResult const& Hit = InSimulationResult.AttackResults[i].HitInfo;

			DrawDebugPoint(World, Hit.ImpactPoint, PointSize, (Hit.bBlockingHit ? TraceHitColor : TraceColor), false, DrawTime);
		}
	}
}

void URangedAttackComponent::ComputeLaunchDestination(AController *InController, FVector const& InStart, float MaxAttackDistance, FVector &OutDest)
{
	AWeapon *Weapon = GetWeapon();
	APlayerController *PC = Cast<APlayerController>(InController);

	if (PC)
	{
		AZPlayerHUD *PlayerHUD = Cast<AZPlayerHUD>(PC->GetHUD());
		if (PlayerHUD)
		{
			OutDest = InStart + PlayerHUD->GetCrosshairWorldDirection() * MaxAttackDistance + (PlayerHUD->GetCrosshairWorldLocation() - Weapon->GetActorLocation());

			ATTACK_LOG(TEXT("Crosshair Loc: %s, Dir: %s, Camera Loc: %s, Dir: %s"), *PlayerHUD->GetCrosshairWorldLocation().ToString(), *PlayerHUD->GetCrosshairWorldDirection().ToString(), *PC->PlayerCameraManager->GetCameraLocation().ToString(), *PC->PlayerCameraManager->GetCameraRotation().Vector().ToString());

			DrawDebugLine(GetWorld(), InStart, InStart + PlayerHUD->GetCrosshairWorldDirection() * 300.f, FColor::Purple, false, 3.f);
			DrawDebugPoint(GetWorld(), PlayerHUD->GetCrosshairWorldLocation(), 16.f, FColor::Black, false, 3.f);

			return;
		}
	}

	if (Weapon)
	{
		OutDest = InStart + Weapon->GetActorForwardVector() * MaxAttackDistance;
	}
}

void URangedAttackComponent::SimulateAttack_Implementation(FAttackSimulationHandle InHandle, UAttackType *InType, int32 SimulationCount)
{
	UWorld *World = GetWorld();

	if (World)
	{
		APawn *Instigator = GetAttackInstigator();

		if (Instigator)
		{
			URangedAttackType *TypeInst = Cast<URangedAttackType>(InType);

			if (TypeInst)
			{
				ARangedWeapon *Weapon = Cast<ARangedWeapon>(GetWeapon());

				if (Weapon)
				{
					FVector LaunchOffset = Weapon->GetMuzzleOffset() + TypeInst->SimulationStartOffset;

					FVector Start = Weapon->GetActorLocation()
								   + Weapon->GetActorForwardVector() * LaunchOffset.X
								  + Weapon->GetActorRightVector() * LaunchOffset.Y
								  + Weapon->GetActorUpVector() * LaunchOffset.Z; 

					FVector End;
					ComputeLaunchDestination(Weapon->GetWeaponOwner() ? Weapon->GetWeaponOwner()->Controller : nullptr, Start, TypeInst->MaxAttackDistance, End);

					FCollisionObjectQueryParams QueryObjectParams;
					GDamageableActorSettings.MakeObjectQueryParams(QueryObjectParams);

					FCollisionQueryParams QueryParams;
					QueryParams.AddIgnoredActor(Weapon);
					QueryParams.AddIgnoredActor(Instigator);

					TArray<FHitResult> HitResults;
					bool bHit = World->LineTraceMultiByObjectType(HitResults, Start, End, QueryObjectParams, QueryParams);

					FAttackSimulationResult SimulationResult(bHit, InType);

					if (bHit)
					{
						for (int32 i = 0; i < HitResults.Num(); i++)
						{
							if (i >= TypeInst->NumAttackedTarget)
							{
								break;
							}

							if (HitResults.IsValidIndex(i))
							{
								SimulationResult.AttackResults.Add(FAttackResult(TypeInst->DamageType, HitResults[i]));
							}
						}
					}

#if ENABLE_DRAW_DEBUG
					if (TypeInst->bDrawDebugSimulationResult)
					{
						DrawDebugSimulationResult(SimulationResult, Start, End, FColor::Red, FColor::Green);
						//DrawDebugLine(GetWorld(), Weapon->GetActorLocation(), Weapon->GetActorLocation() + Weapon->GetActorForwardVector() * 100.f, FColor::Blue, false, 3.0f);
					}
#endif

					Super::NotifySimulationCompleted(InHandle, SimulationResult);
				}

				else
				{
					ATTACK_ERROR(TEXT("Owner(Weapon) of UMeleeAttackComponent is NULL: Skipped Attack Simulating Process"))
				}
			}

			else
			{
				ATTACK_ERROR(TEXT("CurrentType of UMeleeAttackComponent is NULL: Skipped Attack Simulating Process"))
			}
		}

		else
		{
			ATTACK_ERROR(TEXT("Failed to get an instance of Pawn Attack Instigator: Skipped Attack Simulating Process"))
		}
	}

	else
	{
		ATTACK_ERROR(TEXT("Failed to get an instancne of UWorld: Skipped Attack Simulating Process"))
	}
}

void URangedAttackComponent::InternalApplyAttackDamage_Implementation(AActor *Target, FAttackResult const& InResult, AController *InInstigator, AActor *InDamageCauser)
{
	if (Target)
	{
		Target->TakeDamage(InResult.CalculatedDamage, FPointDamageEvent(InResult.CalculatedDamage, InResult.HitInfo, -InResult.HitInfo.ImpactNormal, InResult.DamageTypeClass), InInstigator, InDamageCauser);
	}
}