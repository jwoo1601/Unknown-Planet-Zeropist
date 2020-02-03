// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GameTypes.h"

#include "UCharacter.generated.h"

USTRUCT()
struct UPRISING_API FCharacterPhysicsAttributes
{
	GENERATED_USTRUCT_BODY()

	FCharacterPhysicsAttributes();



};

USTRUCT()
struct UPRISING_API FCharacterPhysicsVolumes
{
	GENERATED_USTRUCT_BODY()
		
	FCharacterPhysicsVolumes();

	UPROPERTY()
	FSphereVolume Head;

	UPROPERTY()
	FCapsuleVolume Body;

};

UCLASS(abstract, Blueprintable)
class UPRISING_API UCharacterContent : public UObject
{
	GENERATED_BODY()

	UCharacterContent(FObjectInitializer const& OI);



public:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1"))
	uint32 HP;

	UPROPERTY(BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1"))
	uint32 Shield;

	UPROPERTY()
	TSet<struct FCostumeData> Costumes;

	UPROPERTY()
	FCharacterPhysicsAttributes PhysicsAttributes;

	UPROPERTY()
	FCharacterPhysicsVolumes PhysicsVolumes;

	UPROPERTY()
	TArray<struct FVoiceLine> VoiceLines;

};