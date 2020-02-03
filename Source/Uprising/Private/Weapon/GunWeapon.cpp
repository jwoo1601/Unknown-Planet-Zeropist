// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "GunWeapon.h"
#include "Weapon/WeaponOwnerInterface.h"
#include "Weapon/Attack/AttackType.h"

AGunWeapon::AGunWeapon(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsReloading = false;
	CurrentAmmo = 0;
	MaxAmmo = 20;

	ReceiveAttackStartedDelegate.BindUFunction(this, TEXT("ReceiveAttackStarted"));
}

void AGunWeapon::StartFire()
{
	StartAttack();
}

void AGunWeapon::StopFire()
{
	StopAttack();
}

void AGunWeapon::BeginPlay()
{
	Super::BeginPlay();

	AttackComponent->OnAttackStarted.Add(ReceiveAttackStartedDelegate);
}

void AGunWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AttackComponent->OnAttackStarted.Remove(ReceiveAttackStartedDelegate);
}

bool AGunWeapon::IsAttackAllowed_Implementation(UAttackType *InAttackType) const
{
	UShootingAttackType *TypeInst = Cast<UShootingAttackType>(InAttackType);
	if (CurrentAmmo >= TypeInst->AmmoCost)
	{
		return true;
	}

	return false;
}

TSubclassOf<class UAttackType> AGunWeapon::GetBaseAttackTypeClass_Implementation() const
{
	return UShootingAttackType::StaticClass();
}

bool AGunWeapon::CanReload_Implementation() const
{
	return !bIsReloading && CurrentAmmo < MaxAmmo;
}

void AGunWeapon::Reload_Implementation()
{
	if (CanReload())
	{
		APawn *Owner = GetWeaponOwner();
		if (Owner)
		{
			bIsReloading = true;

			Event_OnReloadStarted();
			FGunWeaponEventParams Params(GetWeaponOwner(), this);
			OnReloadStarted.Broadcast(Params);

			UAnimInstance *AnimInst = IWeaponOwnerInterface::Execute_GetCharacterAnimInstance(Owner);
			if (AnimInst)
			{
				AnimInst->Montage_Play(OwnerReloadAnimation);
			}

			else
			{
				NotifyReloadAnimationEnded();
			}
		}
	}
}

void AGunWeapon::NotifyReloadAnimationEnded()
{
	if (bIsReloading)
	{
		CurrentAmmo = MaxAmmo;

		Event_OnReloadCompleted();
		FGunWeaponEventParams Params(GetWeaponOwner(), this);
		OnReloadCompleted.Broadcast(Params);

		bIsReloading = false;
	}
}

void AGunWeapon::AddReloadStartedEvent(FScriptDelegate const& InDelegate)
{
	OnReloadStarted.Add(InDelegate);
}

void AGunWeapon::AddReloadCompletedEvent(FScriptDelegate const& InDelegate)
{
	OnReloadCompleted.Add(InDelegate);
}

void AGunWeapon::RemoveReloadStartedEvent(FScriptDelegate const& InDelegate)
{
	OnReloadStarted.Remove(InDelegate);
}

void AGunWeapon::RemoveReloadCompletedEvent(FScriptDelegate const& InDelegate)
{
	OnReloadCompleted.Remove(InDelegate);
}

void AGunWeapon::ReceiveAttackStarted(FAttackEventParams const& InParams)
{
	UShootingAttackType *TypeInst = Cast<UShootingAttackType>(InParams.AttackType);
	if (TypeInst)
	{
		CurrentAmmo -= TypeInst->AmmoCost;
	}
}