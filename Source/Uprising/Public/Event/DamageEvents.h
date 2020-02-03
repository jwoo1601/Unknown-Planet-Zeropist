// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"

#include "DamageEvent.generated.h"

USTRUCT()
struct CLIENT_API FDefaultDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

	UPROPERTY()
	float Damage;

	UPROPERTY()
	FHitResult HitInfo;

	FDefaultDamageEvent() : HitInfo() { }
	FDefaultDamageEvent(float InDamage, const FHitResult &InHitInfo, TSubclassOf<class UDamageType> InDamageTypeClass) : FDamageEvent(InDamageTypeClass), Damage(InDamage), HitInfo(InHitInfo) { }

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 3;

	virtual int32 GetTypeID() const override { return FDefaultDamageEvent::ClassID; };
	virtual bool IsOfType(int32 InID) const override { return (FDefaultDamageEvent::ClassID == InID) || FDamageEvent::IsOfType(InID); };

	/** Simple API for common cases where we are happy to assume a single hit is expected, even though damage event may have multiple hits. */
	virtual void GetBestHitInfo(AActor const* HitActor, AActor const* HitInstigator, struct FHitResult& OutHitInfo, FVector& OutImpulseDir) const override;
};

/**
 * 
 */
USTRUCT()
struct CLIENT_API FMeleeDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

	UPROPERTY()
	float Damage;

	UPROPERTY()
	FHitResult HitInfo;

	FMeleeDamageEvent() : HitInfo() { }
	FMeleeDamageEvent(float InDamage, const FHitResult &InHitInfo, TSubclassOf<class UDamageType> InDamageTypeClass) : FDamageEvent(InDamageTypeClass), Damage(InDamage), HitInfo(InHitInfo) { }

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 4;

	virtual int32 GetTypeID() const override { return FMeleeDamageEvent::ClassID; };
	virtual bool IsOfType(int32 InID) const override { return (FMeleeDamageEvent::ClassID == InID) || FDamageEvent::IsOfType(InID); };

	/** Simple API for common cases where we are happy to assume a single hit is expected, even though damage event may have multiple hits. */
	virtual void GetBestHitInfo(AActor const* HitActor, AActor const* HitInstigator, struct FHitResult& OutHitInfo, FVector& OutImpulseDir) const override;
};