// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SkeletalMesh.h"

#include "GameTypes.h"

#include "Costume.generated.h"

USTRUCT(BlueprintType)
struct UPRISING_API FCostumeData
{
	GENERATED_USTRUCT_BODY()

	FCostumeData() { }

	UPROPERTY()
	FName CostumeID;

	UPROPERTY()
	FText DisplayName;

	UPROPERTY()
	FText Description;

	UPROPERTY()
	USkeletalMesh *Helmet;

	UPROPERTY()
	USkeletalMesh *Shoulders;

	UPROPERTY()
	USkeletalMesh *Armor;

	UPROPERTY()
	USkeletalMesh *Gloves;

	UPROPERTY()
	USkeletalMesh *Boots;

};