#include "AttackType.h"

UAttackType::UAttackType(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	AttackMode = EAttackMode::Single;
	bCanMoveDuringAttack = true;
	NumAttackedTarget = 1;
	SimulationStartOffset = FVector::ZeroVector;
	MaxAttackDistance = 500.f;
	bUseCustomAttackAnimation = false;
	DamageType = UDamageType::StaticClass();
	HeadShotDamageMultiplier = 2.0f;
	MinDamage = 0;
	MaxDamage = 0;
	MaxChargingTime = 2.0f;
	AttackCooldown = 1.0f;

#if WITH_EDITORONLY_DATA
	bDrawDebugSimulationResult = false;
#endif
}

UMeleeAttackType::UMeleeAttackType(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	HorizontalAttackRange = 300.f;
	VerticalAttackRange = 200.f;
}

URangedAttackType::URangedAttackType(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{

}

UShootingAttackType::UShootingAttackType(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{

}