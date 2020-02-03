// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ActorEvents.generated.h"

USTRUCT(BlueprintType)
struct CLIENT_API FActorLevelUpEvent
{
	GENERATED_BODY()

public:

	FActorLevelUpEvent() { }

	FActorLevelUpEvent(AActor *InTarget, int32 InOldLevel, int32 InNewLevel) : Target(InTarget), OldLevel(InOldLevel), NewLevel(InNewLevel) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor *Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OldLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NewLevel;

};

/**
 * 
 */
USTRUCT(BlueprintType)
struct CLIENT_API FActorXPChangedEvent
{
	GENERATED_BODY()

public:

	FActorXPChangedEvent() { }

	FActorXPChangedEvent(AActor *InTarget, int32 InOldXP, int32 InNewXP) : Target(InTarget), OldXP(InOldXP), NewXP(InNewXP) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor *Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OldXP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NewXP;

};