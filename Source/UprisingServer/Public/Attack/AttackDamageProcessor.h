// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ServerTypes.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FAttackDamageComputedSignature, APawn*, DamageInstigator, float, FinalDamage, FAttackDamageInfo const&, InDamageInfo, bool, bHeadshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttackResolvedSignature, APawn*, DamageInstigator, float, FinalDamage, FAttackDamageInfo const&, InDamageInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHeadshotResolvedSignature, APawn*, DamageInstigator, float, FinalDamage, FAttackDamageInfo const&, InDamageInfo);

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class UPRISINGSERVER_API UAttackDamageProcessor : public UObject
{
	GENERATED_BODY()

public:

	UAttackDamageProcessor(FObjectInitializer const& OI);

	/**
	* Starts an attack simulation with the given attack simulation mode
	*
	* @param Mode - attack simulation mode to use
	* @param Params - parameters to use for a new attack simulation
	*
	* @return FGameEventHandle - a simulation handle used to identify this attack simulation (invalid if there's any problem starting an attack simulation)
	*/
	UFUNCTION(BlueprintCallable)
	void ResolveAttack(FAttackSimulationInfo const& InSimulationInfo, FHitResult const& InHit);

	/**
	* Starts an attack simulation with the given attack simulation mode
	*
	* @param Mode - attack simulation mode to use
	* @param Params - parameters to use for a new attack simulation
	*
	* @return FGameEventHandle - a simulation handle used to identify this attack simulation (invalid if there's any problem starting an attack simulation)
	*/
	UFUNCTION(BlueprintCallable)
	void ResolveHeadshot(FAttackSimulationInfo const& InSimulationInfo, FHitResult const& InHeadHit);

public:

	UPROPERTY(BlueprintAssignable)
	FAttackDamageComputedSignature OnDamageComputed;

	UPROPERTY(BlueprintAssignable)
	FAttackResolvedSignature OnAttackResolved;

	UPROPERTY(BlueprintAssignable)
	FHeadshotResolvedSignature OnHeadshotResolved;

protected:

	virtual float ComputeAttackDamage(FAttackDamageInfo const& InDamageInfo, FHitResult const& InHit);

	virtual float ComputeHeadshotDamage(FAttackDamageInfo const& InDamageInfo, FHitResult const& InHeadHit);

	virtual float ComputeRadialDamageScale(FAttackDamageInfo const& InDamageInfo, float DistanceFromEpicenter) const;

	virtual void PostAttackResolved(APawn *DamageInstigator, float FinalDamage, FAttackDamageInfo const& InDamageInfo) { }

	virtual void PostHeadshotResolved(APawn *DamageInstigator, float FinalDamage, FAttackDamageInfo const& InDamageInfo) { }

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bRandomizeDamage : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeadshotDamageScale;

};