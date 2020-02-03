// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"

#include "Weapon/Weapon.h"

#include "RangedWeapon.generated.h"

/**
 * 
 */
UCLASS(abstract, BlueprintType, Blueprintable)
class CLIENT_API ARangedWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	ARangedWeapon(FObjectInitializer const& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent &e) override;
	virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;
#endif

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector MuzzleOffset;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USphereComponent *MuzzleOffsetEd;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UArrowComponent *LaunchDirectionEd;
#endif

protected:

	virtual TSubclassOf<class UAttackType> GetBaseAttackTypeClass_Implementation() const override;

public:

	FORCEINLINE FVector GetMuzzleOffset() const { return MuzzleOffset; }

};
