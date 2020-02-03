// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "SkillContent.h"
#include "Hero/HeroCharacter.h"
#include "Skill/SkillComponent.h"

USkillContent::USkillContent(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	bSupportsCancelation = false;
	bInterceptsAttackInput = false;

	CooldownTable.Add(0.0f);
}

float USkillContent::GetCooldownforLevel(int32 InLevel)
{
	if (InLevel >= 1 && InLevel <= CooldownTable.Num())
	{
		return CooldownTable[InLevel - 1];
	}

	return 0.0f;
}

USimulatedSkillContent::USimulatedSkillContent(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	SimulationRate = 0.1f;
	SimulationDuration = 1.0f;
}