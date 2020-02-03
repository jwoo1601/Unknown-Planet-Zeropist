// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ZPlayerController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CLIENT_API AZPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AZPlayerController(FObjectInitializer const& ObjectInitializer);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|AimOffset", meta = (AllowPrivateAccess = "true"))
	float AOCharacterRotFixThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|AimOffset", meta = (AllowPrivateAccess = "true"))
	float AOCharacterRotSpeed;

public:

	FORCEINLINE float GetAOCharacterRotFixThreshold() const { return AOCharacterRotFixThreshold; }
	FORCEINLINE float GetAOCharacterRotSpeed() const { return AOCharacterRotSpeed; }
	
};
