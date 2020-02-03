// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "MeleeWeapon.h"
#include "Components/SphereComponent.h"
#include "Attack/AttackType.h"
#include "Attack/MeleeAttackComponent.h"

AMeleeWeapon::AMeleeWeapon(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USphereComponent>(AWeapon::PrimitiveComponentName).SetDefaultSubobjectClass<UStaticMeshComponent>(AWeapon::MeshComponentName).SetDefaultSubobjectClass<UMeleeAttackComponent>(AWeapon::AttackComponentName))
{

}


TSubclassOf<class UAttackType> AMeleeWeapon::GetBaseAttackTypeClass_Implementation() const
{
	return UMeleeAttackType::StaticClass();
}