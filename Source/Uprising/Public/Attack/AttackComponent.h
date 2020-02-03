// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"

#include "AttackComponent.generated.h"

UCLASS(abstract, Blueprintable, ClassGroup = (Attack), meta = (BlueprintSpawnableComponent))
class UPRISING_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UAttackComponent(FObjectInitializer const& OI);

	bool CheckCondition() const;



private:

	UPROPERTY()
	uint8 bCustomizeWeaponAnimation : 1;

	UPROPERTY()
	class UAttackAnimMontage *WeaponAnimation;

	UPROPERTY()
	uint8 bCustomizeCharacterAnimation : 1;

	UPROPERTY()
	class UCharacterAttackAnimMontage *CharacterAnimation;

public:

	FORCEINLINE 

};