// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "HeroMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EHeroMovementMode : uint8 {

	MOVE_Sprint UMETA(DisplayName = "Sprint")

};

/**
 * 
 */
UCLASS()
class CLIENT_API UHeroMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 CurrentStamina;
	

	
};
