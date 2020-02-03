// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AttackComponent.h"

#include "MeleeAttackComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CLIENT_API UMeleeAttackComponent : public UAttackComponent
{
	GENERATED_BODY()
	
public:

	UMeleeAttackComponent(FObjectInitializer const& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Misc")
	void RetrieveCapsuleRadius(APawn *InPawn, float &OutRadius) const;

	UFUNCTION(BlueprintCallable, Category = "Misc")
	void RetrieveCapsuleHalfHeight(APawn *InPawn, float &OutHalfHeight) const;

	UFUNCTION(BlueprintCallable, Category = "Misc")
	void RetrieveCapsuleSize(APawn *InPawn, float &OutRadius, float &OutHalfHeight) const;

	void DrawDebugSimulationResult(FAttackSimulationResult const& InSimulationResult, FVector const& Center, FColor CenterPointColor, FVector const& Extent, FQuat const& Rotation, FColor TraceColor, FColor TraceHitColor, float DrawTime = 3.0f, float PointSize = 16.0f) const;

protected:

	/* UAttackComponent Interface */
	virtual void SimulateAttack_Implementation(FAttackSimulationHandle InHandle, UAttackType *InType, int32 SimulationCount) override;
	virtual void InternalApplyAttackDamage_Implementation(AActor *Target, FAttackResult const& InResult, AController *InInstigator, AActor *InDamageCauser) override;
	/* ~UAttackComponent Interface */
	
};
