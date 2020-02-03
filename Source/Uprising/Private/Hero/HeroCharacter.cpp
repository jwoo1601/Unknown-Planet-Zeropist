// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "HeroCharacter.h"
#include "Client.h"
#include "HeroContent.h"
#include "HeroMovementComponent.h"
#include "Data/GlobalData.h"
#include "GameMode/ZGameMode.h"
#include "Player/ZPlayerController.h"
#include "Skill/SkillContent.h"
#include "Skill/SkillComponent.h"
#include "Weapon/Reloadable.h"
#include "Weapon/Weapon.h"
#include "Weapon/Attack/AttackType.h"

#define CHECK_VALID_SKILL(SkillIndex) { if (!RegisteredSkills.IsValidIndex((SkillIndex))) { return; } }

TArray<FName, TFixedAllocator<5>> AHeroCharacter::RegisteredSkillNameArray = { TEXT("HeroSkill1"), TEXT("HeroSkill2"), TEXT("WeaponSkill"), TEXT("ClassSkill"), TEXT("UltimateSkill") };

// Sets default values
AHeroCharacter::AHeroCharacter(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UHeroMovementComponent>(TEXT("HeroMovement")))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HeadVolume = CreateDefaultSubobject<USphereComponent>(TEXT("HeadVolume"));
	HeadVolume->SetupAttachment(RootComponent);
	HeadVolume->SetNotifyRigidBodyCollision(true);
	HeadVolume->bGenerateOverlapEvents = false;
	HeadVolume->SetCollisionProfileName(FName(TEXT("Head")));
	HeadVolume->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	// set our turn rates for input
	//BaseTurnRate = 45.f;
	//BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate

	// this is equivalent to Simulation Generates Hit Event = true
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->bGenerateOverlapEvents = true;
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("Hero")));
	GetCapsuleComponent()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset.Set(30.f, 25.f, 70.f);
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bEnableCameraRotationLag = false;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ViewCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	CurrentLevel = 1;
	SkillInUse = nullptr;

	SkillUseStartedDelegate.BindUFunction(this, TEXT("ReceiveSkillUseStarted"));
	SkillUseCanceledDelegate.BindUFunction(this, TEXT("ReceiveSkillUseCanceled"));
	SkillUseCompletedDelegate.BindUFunction(this, TEXT("ReceiveSkillUseCompleted"));
	WeaponAttackStartedDelegate.BindUFunction(this, TEXT("ReceiveWeaponAttackStarted"));
	WeaponAttackCooldownResolvedDelegate.BindUFunction(this, TEXT("ReceiveWeaponAttackCooldownResolved"));
	WeaponReloadStartedDelegate.BindUFunction(this, TEXT("ReceiveWeaponReloadStarted"));
	WeaponReloadCompletedDelegate.BindUFunction(this, TEXT("ReceiveWeaponReloadCompleted"));
}

bool AHeroCharacter::IsHeadVolume_Implementation(UPrimitiveComponent *InComponent) const
{
	bool bResult = false;

	if (InComponent)
	{
		bResult = (InComponent == HeadVolume);
	}

	return bResult;
}

void AHeroCharacter::EquipMainWeapon()
{
	EquipWeapon_Impl(MainWeapon);
}

void AHeroCharacter::EquipSubWeapon()
{
	EquipWeapon_Impl(SubWeapon);
}

void AHeroCharacter::UnequipWeapon()
{
	if (EquippedWeapon && CanUnequipWeapon(EquippedWeapon))
	{
		CharacterStatus = ECharacterStatus::Equipping;
		PreviousWeapon = EquippedWeapon;

		EquippedWeapon->DeactivateWeapon();

		UAnimMontage *UnequipAnimation = EquippedWeapon->GetOwnerUnequipAnimation();
		if (UnequipAnimation)
		{
			FGameEventTrigger::Register(FGameEventTrigger(TEXT("UnequipWeapon"), GetUniqueID()), FGameEventNotifyDelegate::CreateUObject(this, &AHeroCharacter::ReceiveUnequipWeapon));
			FGameEventTrigger::Register(FGameEventTrigger(TEXT("UnequipWeaponEnd"), GetUniqueID()), FGameEventNotifyDelegate::CreateUObject(this, &AHeroCharacter::ReceiveUnequipAnimationEnd));
			PlayAnimMontage(UnequipAnimation);
		}

		else
		{
			USkeletalMeshComponent *Mesh = GetMesh();
			if (Mesh)
			{
				EquippedWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, EquippedWeapon->GetUnequippedSocketName());
			}

			EquippedWeapon = nullptr;
			bCombatMode = false;
			CharacterStatus = ECharacterStatus::Idle;
		}
	}
}

void AHeroCharacter::SwapEquippedWeapon()
{
	if (EquippedWeapon && MainWeapon && SubWeapon)
	{
		if (EquippedWeapon == MainWeapon)
		{
			WeaponToEquip = SubWeapon;
		}

		else if (EquippedWeapon == SubWeapon)
		{
			WeaponToEquip = MainWeapon;
		}

		if (CanUnequipWeapon(EquippedWeapon) && CanEquipWeapon(WeaponToEquip))
		{
			bSwappingWeapon = true;

			SwapWeapon_Unequip();
		}		
	}
}

// void AHeroCharacter::StartRestoreHP(int32 TotalHPAmount, float InDuration, float InFirstDelay /*=-1.0f*/)
/* {
	FTimerManager &Mgr = GetWorldTimerManager();
	if (!Mgr.TimerExists(HPRestoreTimerHandle))
	{
		if (InDuration == 0)
		{
			RestoreHP(TotalHPAmount, InFirstDelay);
		}

		else if (InDuration > 0)
		{
			HPRestoreCount = InDuration;
			RestoredHPAmount = FMath::Floor(TotalHPAmount / InDuration);
			Mgr.SetTimer(HPRestoreTimerHandle, FTimerDelegate::CreateUObject(this, &AHeroCharacter::ProcessHPRestore), 1.0f, true, InFirstDelay);
		}
	} 
}

void AHeroCharacter::TickStats()
{

}

void AHeroCharacter::InternalTickStats_Implementation()
{

}

void AHeroCharacter::ProcessHPRestore()
{

}

void AHeroCharacter::ProcessHPRestoreAfterDelay()
{

} */

// void AHeroCharacter::RestoreHP(int32 HPAmount, float InDelay /*=(-1.0f)*/)
/* {

} */

void AHeroCharacter::SetMaxHP(int32 NewHP)
{
	if (NewHP > 0)
	{
		MaxHP = NewHP;
		
		if (CurrentHP > MaxHP)
		{
			SetCurrentHP(MaxHP);
		}
	}
}

void AHeroCharacter::SetCurrentHP(int32 NewHP)
{
	if (NewHP >= 0)
	{
		CurrentHP = NewHP;
		if (CurrentHP == 0)
		{
			// @todo: SetDead()
		}
	}
}

void AHeroCharacter::AddCurrentHP(int32 DeltaHP)
{
	if (DeltaHP != 0)
	{
		SetCurrentHP(CurrentHP + DeltaHP);
	}
}

void AHeroCharacter::SetMaxShield(int32 NewShield)
{
	if (NewShield > 0)
	{
		MaxShield = NewShield;

		if (CurrentShield > MaxShield)
		{
			SetCurrentShield(MaxShield);
		}
	}
}

void AHeroCharacter::SetCurrentShield(int32 NewShield)
{
	if (NewShield >= 0)
	{
		CurrentShield = NewShield;
	}
}

void AHeroCharacter::AddCurrentShield(int32 DeltaShield)
{
	if (DeltaShield != 0)
	{
		SetCurrentShield(CurrentShield + DeltaShield);
	}
}

#if WITH_EDITOR
void AHeroCharacter::PostEditChangeProperty(FPropertyChangedEvent &e)
{
	FName PropertyName = e.Property != nullptr ? e.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHeroCharacter, ContentClass))
	{

	}
}
#endif

void AHeroCharacter::PostInitProperties()
{
	Super::PostInitProperties();

}

void AHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ContentClass == nullptr)
	{
		ContentClass = UGlobalData::Get()->DefaultHeroContentClass;
	}

	if (ContentClass != nullptr)
	{
		RegisteredSkills.Reserve(NUM_TOTAL_SKILLS);
		ApplyContentData(ContentClass->GetDefaultObject<AHeroContent>());
	}
}

// Called every frame
void AHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bFreeLookMode)
	{
		if (EquippedWeapon && EquippedWeapon->GetOwnerAimOffset())
		{
			static const float DefaultCharacterRotFixThreshold = 70.f;
			static const float DefaultCharacterRotSpeed = 5.f;

			FRotator CtrlYawRot(0.0f, GetControlRotation().Yaw, 0.0f);
			FRotator ActorYawRot(0.0f, GetActorRotation().Yaw, 0.0f);

			FRotator DeltaRot = (CtrlYawRot - ActorYawRot).GetNormalized();

			float CharacterRotFixThreshold;

			AZPlayerController *ZController = Cast<AZPlayerController>(GetController());
			if (ZController)
			{
				CharacterRotFixThreshold = ZController->GetAOCharacterRotFixThreshold();
			}

			else
			{
				CharacterRotFixThreshold = DefaultCharacterRotFixThreshold;
			}

			if (bAimOffsetRotation || FMath::Abs(DeltaRot.Yaw) >= CharacterRotFixThreshold)
			{
				if (!bAimOffsetRotation)
				{
					bAimOffsetRotation = true;
				}

				float CharacterRotSpeed;

				if (ZController)
				{
					CharacterRotSpeed = ZController->GetAOCharacterRotSpeed();
				}

				else
				{
					CharacterRotSpeed = DefaultCharacterRotSpeed;
				}

				AddActorWorldRotation(FMath::RInterpTo(FRotator::ZeroRotator, DeltaRot, DeltaTime, CharacterRotSpeed));

				if (DeltaRot.IsNearlyZero(1.f))
				{
					bAimOffsetRotation = false;
				}
			}
		}

		else
		{
			if (!bUseControllerRotationYaw)
			{
				bUseControllerRotationYaw = true;
			}
		}
	}
}

void AHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis(TEXT("MoveForward"), this, &AHeroCharacter::HandleMoveInput_Forward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &AHeroCharacter::HandleMoveInput_Right);

	InputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleSprintInput_Pressed);
	InputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AHeroCharacter::HandleSprintInput_Released);

	InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleJumpInput_Pressed);
	InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &AHeroCharacter::HandleJumpInput_Released);

	InputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleCrouchInput_Pressed);
	InputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &AHeroCharacter::HandleCrouchInput_Released);

	InputComponent->BindAxis(TEXT("CameraYaw"), this, &AHeroCharacter::HandleCameraInput_Yaw);
	InputComponent->BindAxis(TEXT("CameraPitch"), this, &AHeroCharacter::HandleCameraInput_Pitch);

	InputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleAttackInput_Pressed);
	InputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &AHeroCharacter::HandleAttackInput_Released);

	InputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleReloadInput);

	InputComponent->BindAction(TEXT("HeroSkill1"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleHeroSkill1UseInput);
	InputComponent->BindAction(TEXT("HeroSkill2"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleHeroSkill2UseInput);

	InputComponent->BindAction(TEXT("WeaponSkill"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleWeaponSkillUseInput);
	InputComponent->BindAction(TEXT("ClassSkill"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleClassSkillUseInput);

	InputComponent->BindAction(TEXT("UltimateSkill"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleUltimateSkillUseInput);

	InputComponent->BindAction(TEXT("EquipWeapon"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleEquipWeaponInput);
	InputComponent->BindAction(TEXT("SwapWeapon"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleSwapWeaponInput);

	InputComponent->BindAction(TEXT("FreeLook"), EInputEvent::IE_Pressed, this, &AHeroCharacter::HandleFreeLookInput_Pressed);
	InputComponent->BindAction(TEXT("FreeLook"), EInputEvent::IE_Released, this, &AHeroCharacter::HandleFreeLookInput_Released);

	for (USkillComponent *Skill : RegisteredSkills)
	{
		Skill->SetupInputComponent(PlayerInputComponent);
	}
}

float AHeroCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	bool bAppliedDamage = false;
	float ActualDamage = DamageAmount;

	UDamageType const* const TypeInst = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent* const PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
		ActualDamage = InternalTakePointDamage(ActualDamage, *PointDamageEvent, EventInstigator, DamageCauser);

		if (ActualDamage > 0.f)
		{
			ActualDamage = ApplyDamage(ActualDamage, DamageEvent);
			bAppliedDamage = true;

			ReceivePointDamage(ActualDamage, TypeInst, PointDamageEvent->HitInfo.ImpactPoint, PointDamageEvent->HitInfo.ImpactNormal, PointDamageEvent->HitInfo.Component.Get(), PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection, EventInstigator, DamageCauser, PointDamageEvent->HitInfo);
			OnTakePointDamage.Broadcast(this, ActualDamage, EventInstigator, PointDamageEvent->HitInfo.ImpactPoint, PointDamageEvent->HitInfo.Component.Get(), PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection, TypeInst, DamageCauser);

			UPrimitiveComponent* const PrimComp = PointDamageEvent->HitInfo.Component.Get();
			if (PrimComp)
			{
				PrimComp->ReceiveComponentDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
			}
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent* const RadialDamageEvent = (FRadialDamageEvent*)&DamageEvent;
		ActualDamage = InternalTakeRadialDamage(ActualDamage, *RadialDamageEvent, EventInstigator, DamageCauser);

		if (ActualDamage > 0.f)
		{
			ActualDamage = ApplyDamage(ActualDamage, DamageEvent);
			bAppliedDamage = true;

			FHitResult const& Hit = (RadialDamageEvent->ComponentHits.Num() > 0) ? RadialDamageEvent->ComponentHits[0] : FHitResult();
			ReceiveRadialDamage(ActualDamage, TypeInst, RadialDamageEvent->Origin, Hit, EventInstigator, DamageCauser);

			for (int HitIdx = 0; HitIdx < RadialDamageEvent->ComponentHits.Num(); ++HitIdx)
			{
				FHitResult const& CompHit = RadialDamageEvent->ComponentHits[HitIdx];
				UPrimitiveComponent* const PrimComp = CompHit.Component.Get();
				if (PrimComp && PrimComp->GetOwner() == this)
				{
					PrimComp->ReceiveComponentDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
				}
			}
		}
	}

	if (ActualDamage > 0.f)
	{
		if (!bAppliedDamage)
		{
			ActualDamage = ApplyDamage(ActualDamage, DamageEvent);
		}

		ReceiveAnyDamage(ActualDamage, TypeInst, EventInstigator, DamageCauser);
		OnTakeAnyDamage.Broadcast(this, ActualDamage, TypeInst, EventInstigator, DamageCauser);
		if (EventInstigator != NULL)
		{
			EventInstigator->InstigatedAnyDamage(ActualDamage, TypeInst, this, DamageCauser);
		}
	}

	return ActualDamage;
}

int32 AHeroCharacter::ApplyDamage(int32 InitialDamage, FDamageEvent const& DamageEvent)
{
	int32 FinalDamage = InitialDamage;

	if (CurrentHP > 0 && CurrentShield > 0)
	{
		float ShieldReductionRate = InitialDamage / CurrentHP;
		int32 ShieldToReduce = FMath::FloorToInt(CurrentShield * ShieldReductionRate); // Should use FMath::Round() on this float value?

		UWorld *WorldInst = GetWorld();
		if (WorldInst)
		{
			AZGameMode *GameModeInst = Cast<AZGameMode>(WorldInst->GetAuthGameMode());
			if (GameModeInst)
			{
				float DamageReductionRate = GameModeInst->GetDamageReductionRate();
				if (!FMath::IsNearlyZero(DamageReductionRate))
				{
					int32 DamageToReduce = ShieldToReduce * DamageReductionRate;

					if (DamageToReduce > InitialDamage)
					{
						DamageToReduce = InitialDamage;
						ShieldToReduce = DamageToReduce / DamageReductionRate;
					}

					FinalDamage -= DamageToReduce;
				}

				AddCurrentHP(-FinalDamage);
				AddCurrentShield(-ShieldToReduce);
			}

			else
			{
				HERO_ERROR(TEXT("Failed to get an instance of AZGameMode via GetAuthGameMode: Skipped Damaging Process"))
			}
		}

		else
		{
			HERO_ERROR(TEXT("Failed to get an instance of UWorld via GetWorld(): Skipped Damaging Process"))
		}

	}

	return FinalDamage;
}

void AHeroCharacter::HandleMoveInput_Forward(float InValue)
{
	if (InValue)
	{
		AddMovementInput(GetActorForwardVector(), InValue);
	}
}

void AHeroCharacter::HandleMoveInput_Right(float InValue)
{
	if (InValue)
	{
		AddMovementInput(GetActorRightVector(), InValue);
	}
}

void AHeroCharacter::HandleSprintInput_Pressed()
{

}

void AHeroCharacter::HandleSprintInput_Released()
{

}

void AHeroCharacter::HandleJumpInput_Pressed()
{
	Jump();
	bJumpInput = CanJump();
	if (bJumpInput)
	{
		CharacterStatus = ECharacterStatus::Jumping;
	}
}

void AHeroCharacter::HandleJumpInput_Released()
{
	StopJumping();
	bJumpInput = false;
	CharacterStatus = ECharacterStatus::Idle;
}

void AHeroCharacter::HandleCrouchInput_Pressed()
{
	bCrouchInput = true;
	CharacterStatus = ECharacterStatus::Crouching;
}

void AHeroCharacter::HandleCrouchInput_Released()
{
	bCrouchInput = false;
	CharacterStatus = ECharacterStatus::Idle;
}

void AHeroCharacter::HandleCameraInput_Yaw(float InValue)
{
	if (Controller && InValue)
	{
		float NewYaw = 30.f * InValue * GetWorld()->GetDeltaSeconds();
		AddControllerYawInput(NewYaw);
	}
}

void AHeroCharacter::HandleCameraInput_Pitch(float InValue)
{
	if (Controller && InValue)
	{
		float NewPitch = FMath::Clamp(-30.f * InValue * GetWorld()->GetDeltaSeconds(), -60.f, 60.f);
		AddControllerPitchInput(NewPitch);
	}
}

void AHeroCharacter::HandleAttackInput_Pressed()
{
	if (SkillInUse && SkillInUse->GetSkillContent()->bInterceptsAttackInput)
	{
		SkillInUse->HandleInterceptedAttackInput_Impl(EInputEvent::IE_Pressed);
	}

	else
	{
		if (EquippedWeapon && CanAttackWithWeapon(EquippedWeapon))
		{
			EquippedWeapon->StartAttack();
		}
	}
}

void AHeroCharacter::HandleAttackInput_Released()
{
	if (SkillInUse && SkillInUse->GetSkillContent()->bInterceptsAttackInput)
	{
		SkillInUse->HandleInterceptedAttackInput_Impl(EInputEvent::IE_Released);
	}

	else
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->StopAttack();
		}
	}
}

void AHeroCharacter::HandleReloadInput()
{
	if (EquippedWeapon)
	{
		IReloadable *ReloadableWeapon = Cast<IReloadable>(EquippedWeapon);

		if (ReloadableWeapon && CanReloadWeapon(ReloadableWeapon))
		{
			IReloadable::Execute_Reload(EquippedWeapon);
		}
	}
}

void AHeroCharacter::HandleHeroSkill1UseInput()
{
	CHECK_VALID_SKILL(HERO1);

	if (RegisteredSkills[HERO1])
	{
		UseSkill_Impl(RegisteredSkills[HERO1]);
	}
}

void AHeroCharacter::HandleHeroSkill2UseInput()
{
	CHECK_VALID_SKILL(HERO2);

	if (RegisteredSkills[HERO2])
	{
		UseSkill_Impl(RegisteredSkills[HERO2]);
	}
}

void AHeroCharacter::HandleWeaponSkillUseInput()
{
	CHECK_VALID_SKILL(WEAPON);

	if (RegisteredSkills[WEAPON])
	{
		UseSkill_Impl(RegisteredSkills[WEAPON]);
	}
}

void AHeroCharacter::HandleClassSkillUseInput()
{
	CHECK_VALID_SKILL(CLASS);

	if (RegisteredSkills[CLASS])
	{
		UseSkill_Impl(RegisteredSkills[CLASS]);
	}
}

void AHeroCharacter::HandleUltimateSkillUseInput()
{
	CHECK_VALID_SKILL(ULTIMATE);

	if (RegisteredSkills[ULTIMATE])
	{
		UseSkill_Impl(RegisteredSkills[ULTIMATE]);
	}
}

void AHeroCharacter::HandleEquipWeaponInput()
{
	if (EquippedWeapon)
	{
		UnequipWeapon();
	}

	else
	{
		if (PreviousWeapon)
		{
			EquipWeapon_Impl(PreviousWeapon);
		}

		else
		{
			EquipMainWeapon();
		}
	}
}

void AHeroCharacter::HandleSwapWeaponInput()
{
	SwapEquippedWeapon();
}

void AHeroCharacter::HandleFreeLookInput_Pressed()
{
	bUseControllerRotationYaw = false;
	bFreeLookMode = true;
}

void AHeroCharacter::HandleFreeLookInput_Released()
{
	bFreeLookMode = false;
}

USkillComponent* AHeroCharacter::GetSkillComponent(ESkillIndex InIndex) const 
{ 
	return RegisteredSkills.IsValidIndex(InIndex) ? RegisteredSkills[InIndex] : nullptr;
}

void AHeroCharacter::InitializeCharacterStats()
{
	CharacterStatus = ECharacterStatus::Idle;
	CurrentLevel = 1;
}

void AHeroCharacter::ApplyContentData(AHeroContent *InContent)
{
	checkf(InContent, TEXT("InAsset must not be NULL"));

	if (HeroContent == nullptr)
	{
		HeroContent = InContent;
		MaxHP = InContent->GetHP();
		CurrentHP = MaxHP;

		MaxShield = InContent->GetShield();
		CurrentShield = MaxShield;

		ApplyMovementAttribute(InContent->GetMovementAttribute());

		UCapsuleComponent *CollisionVolume = GetCapsuleComponent();
		UCapsuleComponent *CCollisionVolume = InContent->GetCollisionVolume();
		CollisionVolume->SetCapsuleSize(CCollisionVolume->GetUnscaledCapsuleRadius(), CCollisionVolume->GetUnscaledCapsuleHalfHeight(), true);
		CollisionVolume->SetMassOverrideInKg(NAME_None, CCollisionVolume->GetMass());
		CollisionVolume->SetLinearDamping(CCollisionVolume->GetLinearDamping());
		CollisionVolume->SetAngularDamping(CCollisionVolume->GetAngularDamping());
		CollisionVolume->SetEnableGravity(CCollisionVolume->IsGravityEnabled());
		/* Needs to add setting Physics Constraints */

		USphereComponent *CHeadVolume = InContent->GetHeadVolume();
		HeadVolume->SetSphereRadius(CHeadVolume->GetUnscaledSphereRadius());

		USkeletalMeshComponent *Mesh = GetMesh();
		USkeletalMeshComponent *CMesh = InContent->GetHeroMesh();
		Mesh->SetAnimationMode(CMesh->GetAnimationMode());
		Mesh->SetAnimInstanceClass(CMesh->AnimClass);
		Mesh->SetSkeletalMesh(CMesh->SkeletalMesh);

		if (HeroContent->ShouldOverrideTransform())
		{
			HeadVolume->SetRelativeTransform(CHeadVolume->GetRelativeTransform());
			Mesh->SetRelativeTransform(CMesh->GetRelativeTransform());
		}

		UClass *MainWeaponClass = HeroContent->DefaultMainWeaponClass;
		if (MainWeaponClass)
		{
			MainWeapon = ConstructWeaponActor(MainWeaponClass, TEXT("MainWeapon"));
		}

		else
		{
			HERO_ERROR(TEXT("Failed to get relevant MainWeapon class: Skipped MainWeapon Setup"))
		}

		UClass *SubWeaponClass = HeroContent->DefaultSubWeaponClass;
		if (SubWeaponClass)
		{
			SubWeapon = ConstructWeaponActor(SubWeaponClass, TEXT("SubWeapon"));
		}

		else
		{
			HERO_ERROR(TEXT("Failed to get relevant MainWeapon class: Skipped MainWeapon Setup"))
		}

		if (RegisteredSkills.Num() != 0)
		{
			RegisteredSkills.Reset(NUM_TOTAL_SKILLS);
		}

		for (int32 i = 0; i < NUM_TOTAL_SKILLS; i++)
		{
			if (TSubclassOf<USkillComponent> SkillComponentClass = InContent->GetSkillClass(i))
			{
				USkillComponent *SkillComp = ConstructSkillComponent(SkillComponentClass, RegisteredSkillNameArray[i]);
				if (SkillComp)
				{
					RegisteredSkills.Add(SkillComp);
				}
			}

			else
			{
				HERO_WARNING(TEXT("Failed to get relevant SkillComponent class. Skipped SkillComponent Setup: Index: %i"), i)
			}
		}
	}

	else
	{
		HERO_WARNING(TEXT("HeroAsset was already imported: ignore ImportAsset() call"));
	}
}

void AHeroCharacter::ApplyMovementAttribute(FHeroMovementAttribute const& InAttribute)
{
	//GetCharacterMovement()->set
}

void AHeroCharacter::EquipWeapon_Impl(AWeapon *WeaponToEquip)
{
	if (WeaponToEquip && CanEquipWeapon(WeaponToEquip))
	{
		if (EquippedWeapon != WeaponToEquip)
		{
			CharacterStatus = ECharacterStatus::Equipping;

			if (EquippedWeapon)
			{
				EquippedWeapon->DeactivateWeapon();

				USkeletalMeshComponent *Mesh = GetMesh();
				if (Mesh)
				{
					/* We don't have to manually detach equipped weapon from the mesh: it's automatically done in attachment process */
					EquippedWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, EquippedWeapon->GetUnequippedSocketName());
				}
			}

			EquippedWeapon = WeaponToEquip;

			UAnimMontage *EquipAnimation = EquippedWeapon->GetOwnerEquipAnimation();
			if (EquipAnimation)
			{
				FGameEventTrigger::Register(FGameEventTrigger(TEXT("EquipWeapon"), GetUniqueID()), FGameEventNotifyDelegate::CreateUObject(this, &AHeroCharacter::ReceiveEquipWeapon));
				FGameEventTrigger::Register(FGameEventTrigger(TEXT("EquipWeaponEnd"), GetUniqueID()), FGameEventNotifyDelegate::CreateUObject(this, &AHeroCharacter::ReceiveEquipAnimationEnd));
				PlayAnimMontage(EquipAnimation);
			}

			else
			{
				USkeletalMeshComponent *Mesh = GetMesh();
				if (Mesh)
				{
					EquippedWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, EquippedWeapon->GetEquippedSocketName());
				}

				EquippedWeapon->ActivateWeapon();

				bCombatMode = true;
				CharacterStatus = ECharacterStatus::Idle;
			}
		}
	}
}

void AHeroCharacter::UseSkill_Impl(USkillComponent *InTargetSkill)
{
	if (InTargetSkill && CanUseSkill(InTargetSkill))
	{
		if (InTargetSkill->GetSkillStatus() == ESkillStatus::Using && InTargetSkill->GetSkillContent()->bSupportsCancelation)
		{
			InTargetSkill->StopUse();
		}

		else
		{
			InTargetSkill->StartUse();
		}
	}
}

bool AHeroCharacter::CanUseSkill(USkillComponent *InSkill) const
{
	if (InSkill)
	{
		USkillContent *Content = InSkill->GetSkillContent();

		if (Content)
		{
			if (CharacterStatus == ECharacterStatus::Crouching || CharacterStatus == ECharacterStatus::Jumping)
			{
				return Content->bAllowMovementAction;
			}
		}
	}

	return CharacterStatus == ECharacterStatus::Idle;
}

bool AHeroCharacter::CanAttackWithWeapon(AWeapon *InWeapon) const
{
	return CharacterStatus == ECharacterStatus::Idle || CharacterStatus == ECharacterStatus::Crouching || CharacterStatus == ECharacterStatus::Jumping;
}

bool AHeroCharacter::CanReloadWeapon(IReloadable *InReloadableWeapon) const
{
	return CharacterStatus == ECharacterStatus::Idle || CharacterStatus == ECharacterStatus::Crouching;
}

bool AHeroCharacter::CanEquipWeapon(AWeapon *InWeapon) const
{
	return CharacterStatus == ECharacterStatus::Idle || CharacterStatus == ECharacterStatus::Crouching;
}

bool AHeroCharacter::CanUnequipWeapon(AWeapon *InWeapon) const
{
	return CharacterStatus == ECharacterStatus::Idle || CharacterStatus == ECharacterStatus::Crouching;
}

UChildActorComponent* AHeroCharacter::ConstructWeaponProxyComponent(TSubclassOf<AWeapon> InWeaponClass, FName Name)
{
	UChildActorComponent *Result = nullptr;

	if (InWeaponClass)
	{
		/*FActorSpawnParameters SpawnParams;
		SpawnParams.Name = Name;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		Result = GetWorld()->SpawnActor<AWeapon>(InWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams); */

		Result = NewObject<UChildActorComponent>(this, Name, RF_Transient);
		if (Result)
		{
			Result->SetChildActorClass(InWeaponClass);
			Result->RegisterComponent();

			AWeapon *WeaponActor = Cast<AWeapon>(Result->GetChildActor());
			if (WeaponActor)
			{
				WeaponActor->Instigator = this;
				WeaponActor->SetWeaponOwner(this);
			}

			else
			{
				HERO_ERROR(TEXT("Failed to spawn AWeapon Instance"));
			}
		}
	}

	return Result;
}

AWeapon* AHeroCharacter::ConstructWeaponActor(TSubclassOf<AWeapon> InWeaponClass, FName Name)
{
	AWeapon *Result = nullptr;

	if (InWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = Name;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		
		UWorld *World = GetWorld();

		if (World)
		{
			Result = World->SpawnActor<AWeapon>(InWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

			if (Result)
			{
				HERO_LOG(TEXT("Spawned Weapon Info: Name=%s, Owner.Name=%s, Instigator.Name=%s"), *Result->GetName(), (Result->GetOwner() ? *Result->GetOwner()->GetName() : TEXT("NULL")), (Result->Instigator ? *Result->Instigator->GetName() : TEXT("NULL")));

				Result->SetWeaponOwner(this);
				Result->SetEnableInteraction(true);
				Result->DeactivateWeapon();

				UAttackComponent *AttackComp = Result->GetAttackComponent();
				if (AttackComp)
				{
					AttackComp->OnAttackCooldownResolved.Add(WeaponAttackCooldownResolvedDelegate);
				}

				IReloadable *WeaponAsReloadable = Cast<IReloadable>(Result);
				if (WeaponAsReloadable)
				{
					WeaponAsReloadable->AddReloadStartedEvent(WeaponReloadStartedDelegate);
					WeaponAsReloadable->AddReloadCompletedEvent(WeaponReloadCompletedDelegate);
				}
			}

			else
			{
				HERO_ERROR(TEXT("Failed to spawn Weapon Actor: Skipped Weapon Construction Process"));
			}
		}

		else
		{
			HERO_ERROR(TEXT("Failed to get an instance of UWorld: Skipped Weapon Construction Process"));
		}
	}

	return Result;
}

USkillComponent* AHeroCharacter::ConstructSkillComponent(TSubclassOf<USkillComponent> InComponentClass, FName Name)
{
	USkillComponent *Result = nullptr;

	if (InComponentClass)
	{
		Result = NewObject<USkillComponent>(this, InComponentClass, Name, EObjectFlags::RF_Transient);

		if (Result)
		{
			Result->RegisterComponent();

			Result->OnUseStarted.Add(SkillUseStartedDelegate);
			Result->OnUseCanceled.Add(SkillUseCanceledDelegate);
			Result->OnUseCompleted.Add(SkillUseCompletedDelegate);
		}
	}

	else
	{
		HERO_ERROR(TEXT("Failed to get relevant USkillComponent Class for %s from SkillAsset: Skipped this process"), *Name.ToString());
	}

	return Result;
}

void AHeroCharacter::ReceiveSkillUseStarted(FSkillEventParams const& InParams)
{
	CharacterStatus = ECharacterStatus::UsingSkill;
	SkillInUse = InParams.Component;
}

void AHeroCharacter::ReceiveSkillUseCanceled(FSkillEventParams const &InParams, FSkillUseResult const& InResult)
{
	if (InParams.Component == SkillInUse)
	{
		SkillInUse = nullptr;
		CharacterStatus = ECharacterStatus::Idle;
	}
}

void AHeroCharacter::ReceiveSkillUseCompleted(FSkillEventParams const &InParams, FSkillUseResult const& InResult)
{
	if (InParams.Component == SkillInUse)
	{
		SkillInUse = nullptr;
		CharacterStatus = ECharacterStatus::Idle;
	}
}

void AHeroCharacter::ReceiveWeaponAttackStarted(FAttackEventParams const& InParams)
{
	if (InParams.Weapon == EquippedWeapon)
	{
		CharacterStatus = ECharacterStatus::Attacking;
	}
}

void AHeroCharacter::ReceiveWeaponAttackCooldownResolved(FAttackEventParams const& InParams)
{
	if (InParams.Weapon == EquippedWeapon)
	{
		CharacterStatus = ECharacterStatus::Idle;
	}
}

void AHeroCharacter::ReceiveWeaponReloadStarted(FGunWeaponEventParams const& InParams)
{
	if (InParams.Weapon == EquippedWeapon)
	{
		CharacterStatus = ECharacterStatus::Reloading;
	}
}

void AHeroCharacter::ReceiveWeaponReloadCompleted(FGunWeaponEventParams const& InParams)
{
	if (InParams.Weapon == EquippedWeapon)
	{
		CharacterStatus = ECharacterStatus::Idle;
	}
}

void AHeroCharacter::ReceiveEquipWeapon(FGameEventTrigger const& InTrigger)
{
	USkeletalMeshComponent *Mesh = GetMesh();
	if (Mesh)
	{
		EquippedWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, EquippedWeapon->GetEquippedSocketName());
	}
}

void AHeroCharacter::ReceiveEquipAnimationEnd(FGameEventTrigger const& InTrigger)
{
	if (EquippedWeapon)
	{
		if (bSwappingWeapon)
		{
			bSwappingWeapon = false;
			WeaponToEquip = nullptr;
		}

		bCombatMode = true;
		CharacterStatus = ECharacterStatus::Idle;
		EquippedWeapon->ActivateWeapon();
	}
}

void AHeroCharacter::ReceiveUnequipWeapon(FGameEventTrigger const& InTrigger)
{
	USkeletalMeshComponent *Mesh = GetMesh();
	if (Mesh)
	{
		EquippedWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, EquippedWeapon->GetUnequippedSocketName());
	}
}

void AHeroCharacter::ReceiveUnequipAnimationEnd(FGameEventTrigger const& InTrigger)
{
	if (EquippedWeapon)
	{
		EquippedWeapon = nullptr;

		if (bSwappingWeapon)
		{
			SwapWeapon_Equip();
		}

		else
		{
			bCombatMode = false;
			CharacterStatus = ECharacterStatus::Idle;
		}
	}
}

void AHeroCharacter::SwapWeapon_Unequip()
{
	if (bSwappingWeapon)
	{
		CharacterStatus = ECharacterStatus::Equipping;
		PreviousWeapon = EquippedWeapon;

		EquippedWeapon->DeactivateWeapon();

		UAnimMontage *UnequipAnimation = EquippedWeapon->GetOwnerUnequipAnimation();
		if (UnequipAnimation)
		{
			FGameEventTrigger::Register(FGameEventTrigger(TEXT("UnequipWeapon"), GetUniqueID()), FGameEventNotifyDelegate::CreateUObject(this, &AHeroCharacter::ReceiveUnequipWeapon));
			FGameEventTrigger::Register(FGameEventTrigger(TEXT("UnequipWeaponEnd"), GetUniqueID()), FGameEventNotifyDelegate::CreateUObject(this, &AHeroCharacter::ReceiveUnequipAnimationEnd));
			PlayAnimMontage(UnequipAnimation);
		}

		else
		{
			USkeletalMeshComponent *Mesh = GetMesh();
			if (Mesh)
			{
				EquippedWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, EquippedWeapon->GetUnequippedSocketName());
			}

			EquippedWeapon = nullptr;

			SwapWeapon_Equip();
		}
	}
}

void AHeroCharacter::SwapWeapon_Equip()
{
	if (bSwappingWeapon && PreviousWeapon && WeaponToEquip)
	{
		CharacterStatus = ECharacterStatus::Equipping;
		EquippedWeapon = WeaponToEquip;

		UAnimMontage *EquipAnimation = EquippedWeapon->GetOwnerEquipAnimation();
		if (EquipAnimation)
		{
			FGameEventTrigger::Register(FGameEventTrigger(TEXT("EquipWeapon"), GetUniqueID()), FGameEventNotifyDelegate::CreateUObject(this, &AHeroCharacter::ReceiveEquipWeapon));
			FGameEventTrigger::Register(FGameEventTrigger(TEXT("EquipWeaponEnd"), GetUniqueID()), FGameEventNotifyDelegate::CreateUObject(this, &AHeroCharacter::ReceiveEquipAnimationEnd));
			PlayAnimMontage(EquipAnimation);
		}

		else
		{
			USkeletalMeshComponent *Mesh = GetMesh();
			if (Mesh)
			{
				EquippedWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, EquippedWeapon->GetEquippedSocketName());
			}

			EquippedWeapon->ActivateWeapon();

			bSwappingWeapon = false;
			WeaponToEquip = nullptr;
			CharacterStatus = ECharacterStatus::Idle;
		}
	}
}

UAnimInstance* AHeroCharacter::GetCharacterAnimInstance_Implementation() const
{
	return GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
}

void AHeroCharacter::PlayCustomAttackAnimation_Implementation(UAttackType *InType)
{
	if (HeroContent)
	{
		HeroContent->PlayCustomAttackAnimation(GetMesh(), InType);
	}
}

void AHeroCharacter::StopCustomAttackAnimation_Implementation()
{
	if (HeroContent)
	{
		HeroContent->StopCustomAttackAnimation(GetMesh());
	}
}

void AHeroCharacter::PlayCustomBurstAttackAnimation_Implementation(UAttackType *InType)
{
	if (HeroContent)
	{
		HeroContent->PlayCustomBurstAttackAnimation(GetMesh(), InType);
	}
}

AWeapon* AHeroCharacter::GetWeapon_Implementation() const
{
	return GetEquippedWeapon();
}

int32 AHeroCharacter::GetOwnerLevel_Implementation() const
{
	return CurrentLevel;
}

void AHeroCharacter::AddLevelUpEvent(FScriptDelegate const& InDelegate)
{
	OnHeroLevelUp.Add(InDelegate);
}

void AHeroCharacter::RemoveLevelUpEvent(FScriptDelegate const& InDelegate)
{
	OnHeroLevelUp.Remove(InDelegate);
}