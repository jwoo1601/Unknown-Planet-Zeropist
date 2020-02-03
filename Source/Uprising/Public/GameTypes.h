// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAttackMode : uint8
{
	Single,
	Auto,
	Charged,

	CustomAttackMode1,
	CustomAttackMode2,
	CustomAttackMode3,
	CustomAttackMode4,
	CustomAttackMode5,
	CustomAttackMode6,
	CustomAttackMode7,
	CustomAttackMode8,
	CustomAttackMode9,
	CustomAttackMode10,
	CustomAttackMode11,
	CustomAttackMode12,
	CustomAttackMode13,
	CustomAttackMode14,
	CustomAttackMode15,
	CustomAttackMode16

};

USTRUCT()
struct UPRISING_API FPhysicsVolume
{
	GENERATED_USTRUCT_BODY()

	FPhysicsVolume(FTransform InOrientation) : Orientation(InOrientation) { }

	UPROPERTY()
	FTransform Orientation;

protected:
	typedef FPhysicsVolume Super;

};

USTRUCT()
struct UPRISING_API FBoxVolume : public FPhysicsVolume
{
	GENERATED_USTRUCT_BODY()

	FBoxVolume(FTransform Orientation, FVector const &InHalfExtent) : Super(Orientation), HalfExtent(InHalfExtent) { }

	UPROPERTY()
	FVector HalfExtent;
};

USTRUCT()
struct UPRISING_API FSphereVolume : public FPhysicsVolume
{
	GENERATED_USTRUCT_BODY()

	FSphereVolume(FTransform Orientation, float InRadius) : Super(Orientation), Radius(InRadius) { }

	UPROPERTY()
	float Radius;

};

USTRUCT()
struct UPRISING_API FCapsuleVolume : public FPhysicsVolume
{
	GENERATED_USTRUCT_BODY()

	FCapsuleVolume(FTransform Orientation, float InRadius, float InHalfHeight) : Super(Orientation), Radius(InRadius), HalfHeight(InHalfHeight) { }

	UPROPERTY()
	float Radius;

	UPROPERTY()
	float HalfHeight;
};

namespace EAttackAnimation
{
	enum Target
	{
		Target_Weapon,
		Target_WeaponOwner,
	};

	UENUM()
	enum SectionType
	{
		Section_Start,
		Section_End,
		Section_Burst
	};

	FName GetSectionName(SectionType Type)
	{
		switch (Type)
		{
		case Section_Start:
			return TEXT("Start");
		case Section_End:
			return TEXT("End");
		case Section_Burst:
			return TEXT("Burst");
		default:
			return NAME_None;
		}
	}

};