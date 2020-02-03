// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "DamageEvent.h"

void FDefaultDamageEvent::GetBestHitInfo(AActor const* HitActor, AActor const* HitInstigator, FHitResult& OutHitInfo, FVector& OutImpulseDir) const
{
	ensure(HitInstigator);
	if (HitInstigator)
	{
		OutHitInfo = HitInfo;

		// assume hit came from instigator's location
		OutImpulseDir = HitInstigator ? (OutHitInfo.ImpactPoint - HitInstigator->GetActorLocation()).GetSafeNormal() : FVector::ZeroVector;

		// assume normal points back toward instigator
		OutHitInfo.ImpactNormal = -OutImpulseDir;
		OutHitInfo.Normal = OutHitInfo.ImpactNormal;
	}
}

void FMeleeDamageEvent::GetBestHitInfo(AActor const* HitActor, AActor const* HitInstigator, FHitResult& OutHitInfo, FVector& OutImpulseDir) const
{
	ensure(HitInstigator);
	if (HitInstigator)
	{
		OutHitInfo = HitInfo;

		// assume hit came from instigator's location
		OutImpulseDir = HitInstigator ?	(OutHitInfo.ImpactPoint - HitInstigator->GetActorLocation()).GetSafeNormal() : FVector::ZeroVector;

		// assume normal points back toward instigator
		OutHitInfo.ImpactNormal = -OutImpulseDir;
		OutHitInfo.Normal = OutHitInfo.ImpactNormal;
	}
}