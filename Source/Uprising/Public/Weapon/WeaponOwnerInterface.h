// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "Animation/AnimInstance.h"

#include "Client.h"
#include "Misc/GameTypes.h"

#include "WeaponOwnerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UPRISING_API IWeaponOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

/*	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attack|Animation")
	UAnimInstance* GetCharacterAnimInstance() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void PlayCustomAttackAnimation(class UAttackType *InType);

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void StopCustomAttackAnimation();

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void PlayCustomBurstAttackAnimation(class UAttackType *InType); */

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	bool AttackAnimationExists(FName AttackTypeName);

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void IsPlayingAnimation();

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void PlayAttackAnimation(FName AttackTypeName);

	UFUNCTION(BlueprintNativeEvent, Category = "Attack|Animation")
	void AnimJumpToSection(EAttackAnimation::SectionType SectionType);

	UFUNCTION(BlueprintNativeEvent, Category = "Attack")
	bool CanStartAttack(UAttackType *AttackType);

	UFUNCTION(BlueprintNativeEvent, Category = "Attack")
	bool CanStartAttackSimulation(UAttackType *AttackType, int32 SimulationCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon")
	class AWeapon* GetWeapon() const;

};
