// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AttackDamageInvolverInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAttackDamageInvolverInterface : public UInterface
{
	GENERATED_BODY()
};

class UPRISINGSERVER_API IAttackDamageInvolverInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Damage")
	float ComputeFinalDamage(AActor *DamageInstigator, float InitialDamage, FAttackDamageInfo const& InDamageInfo, bool bHeadshot, FHitResult const& InHitInfo, float SimulationTime);

};