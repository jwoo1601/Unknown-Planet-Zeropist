// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "RangedWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Attack/AttackType.h"
#include "Attack/RangedAttackComponent.h"

ARangedWeapon::ARangedWeapon(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCapsuleComponent>(AWeapon::PrimitiveComponentName).SetDefaultSubobjectClass<UStaticMeshComponent>(AWeapon::MeshComponentName).SetDefaultSubobjectClass<URangedAttackComponent>(AWeapon::AttackComponentName))
{
#if WITH_EDITORONLY_DATA
	MuzzleOffsetEd = CreateDefaultSubobject<USphereComponent>(TEXT("Editor_MuzzleOffset"));
	MuzzleOffsetEd->SetupAttachment(RootComponent);
	MuzzleOffsetEd->SetSphereRadius(8.0f);
	MuzzleOffsetEd->bGenerateOverlapEvents = false;
	MuzzleOffsetEd->SetCollisionProfileName(TEXT("NoCollision"));
	MuzzleOffsetEd->CanCharacterStepUpOn = ECB_No;
	MuzzleOffsetEd->SetCanEverAffectNavigation(false);
	MuzzleOffsetEd->bApplyImpulseOnDamage = false;

	LaunchDirectionEd = CreateDefaultSubobject<UArrowComponent>(TEXT("Editor_LaunchDirection"));
	LaunchDirectionEd->SetupAttachment(MuzzleOffsetEd);
	LaunchDirectionEd->SetArrowColor(FLinearColor::Red);
	LaunchDirectionEd->bGenerateOverlapEvents = false;
	LaunchDirectionEd->SetCollisionProfileName(TEXT("NoCollision"));
	LaunchDirectionEd->CanCharacterStepUpOn = ECB_No;
	LaunchDirectionEd->SetCanEverAffectNavigation(false);
	LaunchDirectionEd->bApplyImpulseOnDamage = false;
#endif
}

#if WITH_EDITOR
void ARangedWeapon::PostEditChangeProperty(FPropertyChangedEvent &e)
{
	FName ChangedPropertyName = e.GetPropertyName();
#if WITH_EDITORONLY_DATA
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(ARangedWeapon, MuzzleOffsetEd))
	{
		MuzzleOffset = MuzzleOffsetEd->RelativeLocation;
	}
#endif
}

void ARangedWeapon::PreSave(const class ITargetPlatform* TargetPlatform)
{
#if WITH_EDITORONLY_DATA
	MuzzleOffset = MuzzleOffsetEd->RelativeLocation;
#endif
}

#endif

TSubclassOf<class UAttackType> ARangedWeapon::GetBaseAttackTypeClass_Implementation() const
{
	return URangedAttackType::StaticClass();
}