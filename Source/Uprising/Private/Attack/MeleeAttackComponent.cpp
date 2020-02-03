// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "MeleeAttackComponent.h"
#include "CollisionQueryParams.h"
#include "Components/CapsuleComponent.h"
#include "WorldCollision.h"
#include "Client.h"
#include "Event/DamageEvent.h"
#include "Misc/DamageMisc.h"
#include "Weapon/Attack/AttackType.h"

UMeleeAttackComponent::UMeleeAttackComponent(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{

}

FORCEINLINE void UMeleeAttackComponent::RetrieveCapsuleRadius(APawn *InPawn, float &OutRadius) const
{
	UCapsuleComponent *Component = InPawn->FindComponentByClass<UCapsuleComponent>();
	if (Component)
	{
		OutRadius = Component->GetUnscaledCapsuleRadius();
	}
}

FORCEINLINE void UMeleeAttackComponent::RetrieveCapsuleHalfHeight(APawn *InPawn, float &OutHalfHeight) const
{
	UCapsuleComponent *Component = InPawn->FindComponentByClass<UCapsuleComponent>();
	if (Component)
	{
		OutHalfHeight = Component->GetUnscaledCapsuleHalfHeight();
	}
}

FORCEINLINE void UMeleeAttackComponent::RetrieveCapsuleSize(APawn *InPawn, float &OutRadius, float &OutHalfHeight) const
{
	UCapsuleComponent *Component = InPawn->FindComponentByClass<UCapsuleComponent>();
	if (Component)
	{
		Component->GetUnscaledCapsuleSize(OutRadius, OutHalfHeight);
	}
}

void UMeleeAttackComponent::DrawDebugSimulationResult(FAttackSimulationResult const& InSimulationResult, FVector const& Center, FColor CenterPointColor, FVector const& Extent, FQuat const& Rotation, FColor TraceColor, FColor TraceHitColor, float DrawTime, float PointSize) const
{
	UWorld *World = GetWorld();
	
	if (World)
	{
		DrawDebugPoint(World, Center, PointSize, CenterPointColor, false, DrawTime);
		DrawDebugBox(World, Center, Extent, Rotation, (InSimulationResult.bAnyHit ? TraceHitColor : TraceColor), false, DrawTime);

		for (int32 i = 0; i < InSimulationResult.AttackResults.Num(); i++)
		{
			FHitResult const& Hit = InSimulationResult.AttackResults[i].HitInfo;

			DrawDebugPoint(World, Hit.ImpactPoint, PointSize, (InSimulationResult.bAnyHit ? TraceHitColor : TraceColor), false, DrawTime);
		}
	}
}

void UMeleeAttackComponent::SimulateAttack_Implementation(FAttackSimulationHandle InHandle, UAttackType *InType, int32 SimulationCount)
{
	UWorld *World = GetWorld();

	if (World)
	{		
		APawn *Instigator = GetAttackInstigator();

		if (Instigator)
		{
			UMeleeAttackType *TypeInst = Cast<UMeleeAttackType>(InType);

			if (TypeInst)
			{
				FVector ForwardOffset = Instigator->GetActorForwardVector() * TypeInst->SimulationStartOffset.X;
				FVector RightOffset = Instigator->GetActorRightVector() * TypeInst->SimulationStartOffset.Y;
				FVector UpOffset = Instigator->GetActorUpVector() * TypeInst->SimulationStartOffset.Z;

				FVector StartPoint = Instigator->GetActorLocation();

				if (!ForwardOffset.IsNearlyZero())
				{
					StartPoint += ForwardOffset;
				}

				if (!RightOffset.IsNearlyZero())
				{
					StartPoint += RightOffset;
				}

				if (!UpOffset.IsNearlyZero())
				{
					StartPoint += UpOffset;
				}

				FVector EndPoint = StartPoint + FVector(SMALL_NUMBER, 0.0f, 0.0f);

				FCollisionObjectQueryParams QueryObjectParams;
				GDamageableActorSettings.MakeObjectQueryParams(QueryObjectParams);

				FCollisionShape TraceShape;
				TraceShape.SetBox(FVector(TypeInst->MaxAttackDistance * 0.5f, TypeInst->HorizontalAttackRange * 0.5f, TypeInst->VerticalAttackRange * 0.5f));

				FQuat TraceShapeRotation = FRotator::ZeroRotator.Quaternion(); //FQuat::Identity; //Instigator->GetActorRotation().Quaternion();

				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(Super::GetWeapon());
				QueryParams.AddIgnoredActor(Instigator);

				TArray<FHitResult> HitResults;
				bool bHit = GetWorld()->SweepMultiByObjectType(HitResults, StartPoint, EndPoint, TraceShapeRotation, QueryObjectParams, TraceShape, QueryParams);

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
				if (InType->bDrawDebugSimulationResult)
				{
					FVector Center = (StartPoint + EndPoint) * 0.5f;

					DrawDebugSimulationResult(SimulationResult, Center, FColor::Yellow, TraceShape.GetExtent(), TraceShapeRotation, FColor::Red, FColor::Green);
				}
#endif

				Super::NotifySimulationCompleted(InHandle, SimulationResult);
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

void UMeleeAttackComponent::InternalApplyAttackDamage_Implementation(AActor *Target, FAttackResult const& InResult, AController *InInstigator, AActor *InDamageCauser)
{
	if (Target)
	{
		Target->TakeDamage(InResult.CalculatedDamage, FMeleeDamageEvent(InResult.CalculatedDamage, InResult.HitInfo, InResult.DamageTypeClass), InInstigator, InDamageCauser);
	}
}

