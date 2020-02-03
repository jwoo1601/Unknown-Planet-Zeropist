// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"
#include "Curves/CurveFloat.h"

#include "AttackType.generated.h"

USTRUCT(BlueprintType)
struct CLIENT_API FAttackAnimMontageData
{
	GENERATED_BODY()

	FAttackAnimMontageData() { }

	FAttackAnimMontageData(UAnimMontage *InAttackAnimMontage, FName InStartSectionName, FName InEndSectionName, FName InBurstSectionName) : AttackAnimMontage(InAttackAnimMontage), StartSectionName(InStartSectionName), EndSectionName(InEndSectionName), BurstSectionName(InBurstSectionName) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage *AttackAnimMontage;

	/* Name of the section to start with (Animation will not be jump to any section if set to NAME_None) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StartSectionName;

	/* Name of the section to be jumped to when the attack enters stop process (Valid only if AttackMode is set to EAttackMode::Continuos) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EndSectionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BurstSectionName;

};

/**
*
*/
UCLASS(Blueprintable, BlueprintType)
class CLIENT_API UAttackType : public UObject
{
	GENERATED_BODY()

public:

	UAttackType(FObjectInitializer const& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackType")
	FName AttackID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackType")
	EAttackMode AttackMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	uint8 bCanMoveDuringAttack : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (UIMin = "0", ClampMin = "0"))
	int32 NumAttackedTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector SimulationStartOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float MaxAttackDistance;

	/* Whether to receive OnStartAttackAnimation or not instead of automated attack animation process */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	uint8 bUseCustomAttackAnimation : 1;

	/* Name of the slot which will be played once attack executed. AttackID will be used instead if this is set to NAME_None */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FAttackAnimMontageData AnimMontageData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float HeadShotDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (UIMin = "0", ClampMin = "0"))
	int32 MinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (UIMin = "0", ClampMin = "0"))
	int32 MaxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	UCurveFloat *ChargedDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float MaxChargingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float AttackCooldown;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	uint8 bDrawDebugSimulationResult : 1;
#endif

};

UCLASS(BlueprintType, Blueprintable)
class CLIENT_API UMeleeAttackType : public UAttackType
{
	GENERATED_BODY()

public:

	UMeleeAttackType(FObjectInitializer const& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeRange", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float HorizontalAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeRange", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float VerticalAttackRange;

};

UCLASS(BlueprintType, Blueprintable)
class CLIENT_API URangedAttackType : public UAttackType
{
	GENERATED_BODY()

public:

	URangedAttackType(FObjectInitializer const& ObjectInitializer);

	/* Weapon Anim Stuffs */

};

UCLASS(BlueprintType, Blueprintable)
class CLIENT_API UShootingAttackType : public URangedAttackType
{
	GENERATED_BODY()

public:

	UShootingAttackType(FObjectInitializer const& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (UIMin = "1", ClampMin = "1"))
	int32 AmmoCost;

};