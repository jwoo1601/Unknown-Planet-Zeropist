// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"

#include "GameTypes.h"

#include "AttackAnimMontage.generated.h"

UCLASS(BlueprintType)
class UPRISING_API UAttackAnimMontage : public UAnimMontage
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	EAttackMode AttackMode;

	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	FName BurstSectionName;

	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	FName EndSectionName;

};