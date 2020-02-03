// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

//#include "Data/HeroLevelData.h"

#include "GGameMode.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UPRISINGSERVER_API AGGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AGGameMode(const FObjectInitializer &ObjectInitializer);

public:

	TSubclassOf<class UAttackSimulationManager> AttackSimulationManagerClass;

	TSubclassOf<class UAttackDamageProcessor> AttackDamageProcessorClass;

	virtual void BeginPlay() override;

	virtual void StartPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (ShortTooltip = "Damage Reduction Rate per Shield", AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0"))
	float DamageReductionRate = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (DisplayName = "XP Generation", ShortTooltip = "Natural XP Generation per minute", AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1"))
	int32 XPGen = 50;

//#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1", EditCondition = "bCanModifyMaxLevel"))
	int32 HeroMaxLevel = 5;
/* #else
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1"))
	int32 HeroMaxLevel = 5;
#endif */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	TArray<FHeroLevelData> HeroXPTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team", meta = (DisplayName = "Number of Team", ShortTooltip = "Number of Teams in the game", AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1", EditCondition = "bCanModifyNumTeam"))
	int32 NumTeam = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Category = "Team", meta = (AllowPrivateAccess = "true"))
	TArray <class ATeam*> TeamList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team", meta = (DisplayName = "Num of Max Member", ShortTooltip = "Max Number of Members for each team", AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1", EditCondition = "bCanModifyNumMaxMember"))
	int32 NumMaxMember = 6;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float GravityScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Walking", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float MaxStepHeight = 45.f;

public:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent &e) override;
#endif
	
public:

	FORCEINLINE float GetDamageReductionRate() const;
	FORCEINLINE int32 GetXPGen() const;
	FORCEINLINE int32 GetHeroMaxLevel() const;
	FORCEINLINE const TArray<FHeroLevelData>& GetXPTable() const;
	FORCEINLINE int32 GetNumTeam() const;
	FORCEINLINE const TArray<ATeam*>& GetTeamList() const;
	FORCEINLINE int32 GetNumMaxMember() const;

private:
#if WITH_EDITORONLY_DATA
	// Variables for Internal Usage
	UPROPERTY()
	int32 OldMaxLevel;
	UPROPERTY()
	int32 OldNumTeam;

	uint8 bCanModifyMaxLevel : 1;
	uint8 bCanModifyNumTeam : 1;
	uint8 bCanModifyNumMaxMember : 1;
#endif
	
};
