 // Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture.h"

#include "SkillContent.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Passive,
	Active
};

namespace ESkillFlags
{
	typedef uint32 Type;

	UENUM(BlueprintType)
	enum Values
	{
		Cancelable				= 0x01,
		Interruptable			= 0x02,
		MovementAllowed			= 0x04,
		MovementActionAllowed	= 0x08
	};
}

#define UNATIVE_PROPERTY(...)

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UPRISING_API USkillContent : public UObject
{
	GENERATED_BODY()

public:

	USkillContent(FObjectInitializer const& Objectinitializer);

	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool HasFlag(ESkillFlags::Values FlagToRetrieve);

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	ESkillType UsageType;
	
	UNATIVE_PROPERTY(Comment="This is a bit flag that stores various data for special features of the skill")
	ESkillFlags::Type SkillFlag;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prerequisite", meta = (AllowPrivateAccess = "true"))
	int32 RequiredPlayerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prerequisite", meta = (AllowPrivateAccess = "true"))
	TSet<TSubclassOf<class USkillContent>> SkillDependencies;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource", meta = (DisplayName = "Idle", AllowPrivateAccess = "true"))
	UTexture *IconIdle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource", meta = (DisplayName = "Activated", AllowPrivateAccess = "true"))
	UTexture *IconActivated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource", meta = (DisplayName = "Banned", AllowPrivateAccess = "true"))
	UTexture *IconBanned;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown", meta = (AllowPrivateAccess = "true"))
	TArray<float> CooldownTable;



	// Temporary Code

	/*	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property", meta = (AllowPrivateAccess = "true"))
	uint8 bSupportsCancelation : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property", meta = (AllowPrivateAccess = "true"))
	uint8 bInterceptsAttackInput : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property", meta = (AllowPrivateAccess = "true"))
	uint8 bCanMoveDuringAttack : 1;

	// Crouching or Jumping
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property", meta = (AllowPrivateAccess = "true"))
	uint8 bAllowMovementAction : 1; */

};

UENUM(BlueprintType)
enum class ESkillSimulationType : uint8
{
	Prompt,
	Tick
};

UCLASS(BlueprintType, Blueprintable)
class UPRISING_API USimulatedSkillContent : public USkillContent
{
	GENERATED_BODY()

public:

	USimulatedSkillContent(FObjectInitializer const& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	FName TriggerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
	ESkillSimulationType SimulationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float SimulationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float SimulationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float SimulationTimeOut;

};