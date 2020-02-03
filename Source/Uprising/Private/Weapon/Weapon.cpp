// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "Weapon.h"
#include "Client.h"
#include "Weapon/WeaponOwnerInterface.h"
#include "Weapon/Attack/AttackComponent.h"
#include "Weapon/Attack/AttackType.h"

AWeapon::AWeapon(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionMesh = CreateAbstractDefaultSubobject<UPrimitiveComponent>(AWeapon::PrimitiveComponentName);
	if (CollisionMesh)
	{
		RootComponent = CollisionMesh;
		CollisionMesh->bGenerateOverlapEvents = false;
		CollisionMesh->SetCollisionProfileName(TEXT("Weapon"));
		CollisionMesh->CanCharacterStepUpOn = ECB_No;
		CollisionMesh->SetCanEverAffectNavigation(false);
		CollisionMesh->bApplyImpulseOnDamage = false;
	}

	WeaponMesh = CreateAbstractDefaultSubobject<UMeshComponent>(AWeapon::MeshComponentName);
	if (WeaponMesh)
	{
		WeaponMesh->SetupAttachment(CollisionMesh);
		WeaponMesh->bGenerateOverlapEvents = false;
		WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
		WeaponMesh->CanCharacterStepUpOn = ECB_No;
		WeaponMesh->SetCanEverAffectNavigation(false);
		WeaponMesh->bApplyImpulseOnDamage = false;	
	}

	AttackComponent = CreateAbstractDefaultSubobject<UAttackComponent>(AWeapon::AttackComponentName);

	bAllowHeadShot = true;	
	bCanBeDamaged = false;
}

void AWeapon::SetWeaponOwner(APawn *NewOwner)
{
	if (NewOwner)
	{
		if (Cast<IWeaponOwnerInterface>(NewOwner))
		{
			if (GetOwner() == NewOwner)
			{
				return;
			}

			SetOwner(NewOwner);
			WeaponOwner = NewOwner;

			USkeletalMeshComponent *Mesh = WeaponOwner->FindComponentByClass<USkeletalMeshComponent>();
			checkf(Mesh, TEXT("Owner of this AWeapon actor must have a child USkeletalMeshComponent"));

			this->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, UnequippedSocketName);
			AddTickPrerequisiteActor(WeaponOwner);

			FWeaponEventParams Params(WeaponOwner, this);
			OnWeaponOwnerSetup.Broadcast(Params);
		}

		else
		{
			WEAPON_ERROR(TEXT("Failed to set weapon's owner: NewOwner must implement IWeaponOwner"))
		}
	}

	else
	{
		WEAPON_LOG(TEXT("Failed to set weapon's owner: NewOwner is nullptr"))
	}
}

void AWeapon::SetEnableInteraction(bool bAllowInteraction)
{
	if (bAllowInteraction)
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
	}

	else
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

void AWeapon::ActivateWeapon()
{
	SetActorTickEnabled(true);

	if (AttackComponent)
	{
		AttackComponent->Activate();
	}
}

void AWeapon::DeactivateWeapon()
{
	SetActorTickEnabled(false);
	
	if (AttackComponent)
	{
		AttackComponent->Deactivate();
	}
}

UAttackType* AWeapon::ChooseNewAttackType() const
{
	static const int32 RandomRangeMultiplier = 10;

	if (Role == ROLE_Authority && AttackComponent->GetStatus() == EAttackStatus::Idle)
	{
		if (AttackTypeSelectionMethod == EAttackTypeSelectionMethod::Sequential)
		{
			if (PreviousAttackType)
			{
				for (int32 i = 0; i < AvailableAttackTypes.Num(); i++)
				{
					UClass *AttackTypeClass = AvailableAttackTypes[i];

					if (AttackTypeClass == PreviousAttackType->GetClass())
					{
						if (AvailableAttackTypes.Num() >= i + 1)
						{
							return AvailableAttackTypes[0]->GetDefaultObject<UAttackType>();
						}

						else
						{
							return AvailableAttackTypes[i + 1]->GetDefaultObject<UAttackType>();
						}
					}
				}
			}

			else if (AvailableAttackTypes.Num() > 0)
			{
				return AvailableAttackTypes[0]->GetDefaultObject<UAttackType>();
			}
		}

		else if (AttackTypeSelectionMethod == EAttackTypeSelectionMethod::Random)
		{
			// @todo? need to replace random number gen method?

			if (AvailableAttackTypes.Num() > 0)
			{
				int32 Count = AvailableAttackTypes.Num();
				int32 Index = FMath::RandHelper(Count * RandomRangeMultiplier) % Count;

				return AvailableAttackTypes[Index]->GetDefaultObject<UAttackType>();
			}
		}
	}

	return nullptr;
}

bool AWeapon::CanStartAttack(UAttackType *AttackType) const
{
	return !IsPendingKillPending();
}

bool AWeapon::CanStartAttackSimulation(UAttackType *AttackType, int32 SimulationCount) const
{
	return !IsPendingKillPending();
}

bool AWeapon::AttackAnimationExists(FName AttackTypeName) const
{
	return AttackAnims.Find(AttackTypeName);
}

void AWeapon::PlayAttackAnimation(FName AttackTypeName)
{
	UAnimMontage *AttackAnimToPlay = AttackAnims.Find(AttackTypeName);
	if (AttackAnimToPlay)
	{
		USkeletalMeshComponent *SWeaponMesh = Cast<USkeletalMeshComponent>(WeaponMesh);
		if (SWeaponMesh)
		{
			SWeaponMesh->PlayAnimation(AttackAnimToPlay, false);
		}
	}
}

void AWeapon::AnimJumpToSection(EAttackAnimation::SectionType SectionType)
{
	USkeletalMeshComponent *SWeaponMesh = Cast<USkeletalMeshComponent>(WeaponMesh);
	if (SWeaponMesh)
	{
		UAnimInstance *AnimInstance = SWeaponMesh->GetAnimInstance();
		if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->Montage_JumpToSection(EAttackAnimation::GetSectionName(SectionType));
		}
	}
}

bool AWeapon::IsHeadShotAllowedOnTarget(AActor *Target) const
{
	return bAllowHeadShot;
}

void AWeapon::StartAttack()
{
	/* if (CanAttack())
	{
		UClass *CurrentAtkType = GetNewAttackTypeClass();

		if (CurrentAtkType && CurrentAtkType->IsChildOf(GetBaseAttackTypeClass()))
		{
			UAttackType *TypeInst = CurrentAtkType->GetDefaultObject<UAttackType>();

			if (TypeInst && IsAttackAllowed(TypeInst))
			{
				AttackComponent->StartAttack(TypeInst);
			}
		}
	} */

	UAttackType *AttackType = ChooseNewAttackType();
	if (AttackType)
	{
		AttackComponent->StartAttack(AttackType);
	}

	else
	{
		WEAPON_LOG(TEXT("Failed to start a new attack: ChooseNewAttackType() returns NULL!"));
	}
}

void AWeapon::StopAttack()
{
	if (AttackComponent)
	{
		AttackComponent->StopAttack();
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// UAttackComponent* AWeapon::GetAttackComponent() const { return AttackComponent; }