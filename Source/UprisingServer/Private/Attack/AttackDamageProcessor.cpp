// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "AttackDamageProcessor.h"

#include "AttackDamageInvolverInterface.h"

UAttackDamageProcessor::UAttackDamageProcessor(FObjectInitializer const& OI) : Super(OI)
{
	bRandomizeDamage = true;
	HeadshotDamageScale = 2.0f;
}

void UAttackDamageProcessor::ResolveAttack(FAttackSimulationInfo const& InSimulationInfo, FHitResult const& InHit)
{
	float AttackDamage = 0.f;

	FAttackDamageInfo const& DamageInfo = InSimulationInfo.InitialParams.DamageInfo;
	APawn *DamageInstigator = InSimulationInfo.InitialParams.DamageInstigator;

	AttackDamage = ComputeAttackDamage(DamageInfo, InHit);

	if (Cast<UAttackDamageInvolverInterface>(DamageInstigator))
	{
		AttackDamage = IAttackDamageInvolverInterface::Execute_ComputeFinalDamage(DamageInstigator, AttackDamage, DamageInfo, false, InHit, InSimulationInfo.SimulationTime);
	}

	OnDamageComputed.Broadcast(DamageInstigator, AttackDamage, DamageInfo, false);

	PostAttackResolved(DamageInstigator, AttackDamage, DamageInfo);
	OnAttackResolved.Broadcast(DamageInstigator, AttackDamage, DamageInfo);
}

void UAttackDamageProcessor::ResolveHeadshot(FAttackSimulationInfo const& InSimulationInfo, FHitResult const& InHeadHit)
{
	float HeadshotDamage = 0.f;

	FAttackDamageInfo const& DamageInfo = InSimulationInfo.InitialParams.DamageInfo;
	APawn *DamageInstigator = InSimulationInfo.InitialParams.DamageInstigator;

	// if this is radial damage, we don't count this as a headshot
	if (InSimulationInfo.InitialParams.DamageInfo.AttackDamageType == EAttackDamageType::Radial)
	{
		ResolveAttack(InSimulationInfo, InHeadHit);
	}

	else
	{
		HeadshotDamage = ComputeHeadshotDamage(DamageInfo, InHeadHit);

		if (Cast<UAttackDamageInvolverInterface>(DamageInstigator))
		{
			HeadshotDamage = IAttackDamageInvolverInterface::Execute_ComputeFinalDamage(DamageInstigator, HeadshotDamage, DamageInfo, true, InHeadHit, InSimulationInfo.SimulationTime);
		}
		
		OnDamageComputed.Broadcast(DamageInstigator, HeadshotDamage, DamageInfo, true);

		PostHeadshotResolved(DamageInstigator, HeadshotDamage, DamageInfo);
		OnHeadshotResolved.Broadcast(DamageInstigator, HeadshotDamage, DamageInfo);
	}
}

float UAttackDamageProcessor::ComputeAttackDamage(FAttackDamageInfo const& InDamageInfo, FHitResult const& InHit)
{
	float FinalDamage;

	if (InDamageInfo.AttackDamageType == EAttackDamageType::Point)
	{
		FinalDamage = bRandomizeDamage ? FMath::RandRange(InDamageInfo.MinDamage, InDamageInfo.MaxDamage) : InDamageInfo.MinDamage;
	}

	else if (InDamageInfo.AttackDamageType == EAttackDamageType::Radial)
	{
		float DamageScale = ComputeRadialDamageScale(InDamageInfo, (InDamageInfo.Origin - InHit.ImpactPoint).Size());
		float MaxActualDamage = bRandomizeDamage ? FMath::RandRange(InDamageInfo.MinDamage, InDamageInfo.MaxDamage) : InDamageInfo.MinDamage;

		FinalDamage = FMath::Lerp(InDamageInfo.MinDamage, MaxActualDamage, FMath::Max(0.f, DamageScale));
	}

	else
	{
		FinalDamage = bRandomizeDamage ? FMath::RandRange(InDamageInfo.MinDamage, InDamageInfo.MaxDamage) : InDamageInfo.MinDamage;
	}

	return FinalDamage;
}

float UAttackDamageProcessor::ComputeHeadshotDamage(FAttackDamageInfo const& InDamageInfo, FHitResult const& InHeadHit)
{
	float PrecomputedDamage = bRandomizeDamage ? FMath::RandRange(InDamageInfo.MinDamage, InDamageInfo.MaxDamage) : InDamageInfo.MinDamage;

	return PrecomputedDamage * FMath::Max(1.f, HeadshotDamageScale);
}

float UAttackDamageProcessor::ComputeRadialDamageScale(FAttackDamageInfo const& InDamageInfo, float DistanceFromEpicenter) const
{
	const float InnerRadius = FMath::Max(0.f, InDamageInfo.InnerRadius);
	const float OuterRadius = FMath::Max(InDamageInfo.OuterRadius, InnerRadius);
	const float Distance = FMath::Max(0.f, DistanceFromEpicenter);

	if (Distance >= OuterRadius)
	{
		return 0.f;
	}

	UDamageType const* DamageType = GetDefault<UDamageType>(InDamageInfo.DamageType);

	if (!DamageType || DamageType->DamageFalloff == 0.f) || (Distance <= InnerRadius))
	{
		// no falloff or inside inner radius means full effect
		return 1.f;
	}

	// calculate the interpolated scale
	float DamageScale = 1.f - ((Distance - InnerRadius) / (OuterRadius - InnerRadius));
	DamageScale = FMath::Pow(DamageScale, DamageType->DamageFalloff);

	return DamageScale;
}