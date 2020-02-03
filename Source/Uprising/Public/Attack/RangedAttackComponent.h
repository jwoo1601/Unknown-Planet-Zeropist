// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AttackComponent.h"

#include "RangedAttackComponent.generated.h"

UCLASS(BlueprintType, Blueprintable)
class CLIENT_API URangedAttackComponent : public UAttackComponent
{
	GENERATED_BODY()

public:

	URangedAttackComponent(FObjectInitializer const& ObjectInitializer);

	void DrawDebugSimulationResult(FAttackSimulationResult const& InSimulationResult, FVector const& Start, FVector const& End, FColor TraceColor, FColor TraceHitColor, float DrawTime = 3.0f, float PointSize = 16.0f) const;

	void ComputeLaunchDestination(AController *InController, FVector const& InStart, float MaxAttackDistance, FVector &OutDest);

protected:

	/* UAttackComponent Interface */
	virtual void SimulateAttack_Implementation(FAttackSimulationHandle InHandle, UAttackType *InType, int32 SimulationCount) override;
	virtual void InternalApplyAttackDamage_Implementation(AActor *Target, FAttackResult const& InResult, AController *InInstigator, AActor *InDamageCauser) override;
	/* ~UAttackComponent Interface */

};