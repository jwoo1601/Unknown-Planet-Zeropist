// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "WorldCollision.h"

UENUM(BlueprintType)
namespace ESimulationFailureReason
{
	enum Type
	{
		InvalidWorldInstance,

		InvalidSimulationParams,

		FailedToSpawn,

		TimeOut,

		Unknown
	};
}

UENUM(BlueprintType)
enum class EAttackDamageType : uint8
{
	Point,
	Radial
};

USTRUCT()
struct UPRISINGSERVER_API FAttackDamageInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY()
	EAttackDamageType AttackDamageType;

	UPROPERTY()
	float MinDamage;

	UPROPERTY()
	float MaxDamage;

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float InnerRadius;

	UPROPERTY()
	float OuterRadius;

};

DECLARE_DYNAMIC_DELEGATE_OneParam(FAttackSimulationCompletionHandler, FAttackSimulationResult const&, SimulationResult);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAttackSimulationFailureHandler, ESimulationFailureReason::Type, FailureReason);

USTRUCT(BlueprintType)
struct UPRISINGSERVER_API FAttackSimulationParams
{
	GENERATED_USTRUCT_BODY()

	FAttackSimulationParams() : bUsePreciseSimulation(false), bUseMaxDistance(true), bReceivePhysicsMaterialResponse(false) { }

	UPROPERTY()
	uint8 bUsePreciseSimulation : 1;

	UPROPERTY()
	UWorld *SimulatedWorld;

	UPROPERTY()
	APawn *DamageInstigator;

	UPROPERTY(BlueprintAssignable)
	FAttackSimulationCompletionHandler CompletionHandler;

	UPROPERTY(BlueprintAssignable)
	FAttackSimulationFailureHandler FailureHandler;

	UPROPERTY()
	FAttackDamageInfo DamageInfo;

	UPROPERTY()
	FVector Start;

	UPROPERTY()
	FVector Direction;

	UPROPERTY()
	float MaxDistance;

/*	UPROPERTY()
	FVector End; */

	UPROPERTY()
	uint8 bUseMaxDistance : 1;

	/**
	* For EAttackSimulationMode::PrimitiveTrace, this represents the initial rotation of the shape
	* For EAttackSimulationMode::ProjectileCast, this represents the initial rotation of the projectile actor
	* Other than above cases, this doesn't do anything
	*/
	UPROPERTY()
	FRotator Rotation;

	FCollisionShape Shape;

	UPROPERTY()
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY()
	uint8 bReceivePhysicsMaterialResponse : 1;

	UPROPERTY()
	TArray<AActor*> IgnoredActors;

	UPROPERTY()
	TArray<UPrimitiveComponent*> IgnoredComponents;

};

USTRUCT()
struct UPRISINGSERVER_API FAttackSimulationInfo
{
	GENERATED_USTRUCT_BODY()

	friend class UAttackSimulationManager;

	FAttackSimulationInfo() { }

	UPROPERTY()
	FAttackSimulationParams InitialParams;

	UPROPERTY()
	float SimulationTime;

	UPROPERTY()
	AProjectile *ProjectileInstance;

private:

	UPROPERTY()
	FScriptDelegate ProjectileHitDelegate;

	UPROPERTY()
	TArray<FHitResult> SimulationHits;

};

USTRUCT()
struct UPRISINGSERVER_API FAttackSimulationResult
{
	GENERATED_USTRUCT_BODY()

	FAttackSimulationResult() { }

	UPROPERTY()
	uint8 bAnyHit : 1;

	UPROPERTY()
	TArray<FHitResult> HeadHits;

	UPROPERTY()
	TArray<FHitResult> Hits;

};