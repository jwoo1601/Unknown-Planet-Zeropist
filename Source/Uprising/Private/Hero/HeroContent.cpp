// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "HeroContent.h"
#include "Client.h"
#include "Data/GlobalData.h"
#include "Hero/HeroCharacter.h"
#include "Skill/SkillContent.h"

#define LOCTEXT_NAMESPACE "Hero"

AHeroContent::AHeroContent(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer), DisplayName(LOCTEXT("UNDEFINED", "UNDEFINED_DISPLAY_NAME"))
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionVolume"));
	RootComponent = CollisionVolume;

	HeadVolume = CreateDefaultSubobject<USphereComponent>(TEXT("HeadVolume"));
	HeadVolume->SetupAttachment(CollisionVolume);
	HeadVolume->SetRelativeLocation(FVector(0.0f, CollisionVolume->GetUnscaledCapsuleHalfHeight() - HeadVolume->GetUnscaledSphereRadius(), 0.0f));

	HeroMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeroMesh"));
	HeroMesh->SetupAttachment(CollisionVolume);

	bOverrideTransform = true;
	HP = 300;
	Shield = 100;
}

// Called when the game starts or when spawned
void AHeroContent::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHeroContent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#if WITH_EDITOR
// Construction Script
void AHeroContent::OnConstruction(const FTransform &Transform) {
	Super::OnConstruction(Transform);

}

// Event called when one of member properties changed
void AHeroContent::PostEditChangeProperty(FPropertyChangedEvent &e) {
	Super::PostEditChangeProperty(e);

/*	FName PropertyName = e.Property != nullptr ? e.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHeroAsset, MaxLevel)) {
		bCanModifyMaxLevel = false;

		if (OldMaxLevel > MaxLevel) {
			int32 Diff = OldMaxLevel - MaxLevel;
			for (int32 i = 0; i < Diff; i++)
				ExpTable.Pop(false);

			ExpTable.Shrink();
		}

		else if (OldMaxLevel < MaxLevel) {
			UGlobalData *Instance = UGlobalData::Get();
			if (Instance == nullptr)
				CLIENT_ERROR(TEXT("Failed to Get UGlobalData Instance"))//FORMAT_CODE_INFO(TEXT("Failed to Get UGlobalData Instance")))

			else {
				int32 Diff = MaxLevel - OldMaxLevel;
				ExpTable.Reserve(ExpTable.Num() + Diff);

				for (int32 i = 0; i < Diff; i++) {
					int32 CurrentLevel = ExpTable.Num() + 1;

					if (ExpCalcMethod == EExpCalcMethod::Custom)
						ExpTable.Add(FLevelExp(CurrentLevel, CalculateExp(CurrentLevel)));

					else
						ExpTable.Add(FLevelExp(CurrentLevel, Instance->HeroAttribute.EditorDefaultExp));
				}
			}
		}

		OldMaxLevel = MaxLevel;
		bCanModifyMaxLevel = true;
	}

	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AHeroAsset, ExpTable)) {
		// if (ExpTable.Num() > MaxLevel)
		//	ExpTable.Pop();
	} */

	/* FName PropertyName = e.Property != NULL ? e.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHeroAsset, Skill0Class)) {
		Skill0 = NewObject<USkill>(this, Skill0Class);
	} */
}
#endif


TSubclassOf<USkillComponent> AHeroContent::GetSkillClass(int32 InSkillIndex) const
{
	switch (InSkillIndex)
	{
	case HERO1:
		return HeroSkill1Class;
	case HERO2:
		return HeroSkill2Class;
	case WEAPON:
		return WeaponSkillClass;
	case CLASS:
		return ClassSkillClass;
	case ULTIMATE:
		return UltimateSkillClass;
	default:
		checkNoEntry();
		return nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

