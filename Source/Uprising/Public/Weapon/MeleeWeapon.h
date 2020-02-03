// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS(abstract, BlueprintType, Blueprintable)
class CLIENT_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	AMeleeWeapon(FObjectInitializer const& ObjectInitializer);

protected:

	virtual TSubclassOf<class UAttackType> GetBaseAttackTypeClass_Implementation() const override;
	
};
