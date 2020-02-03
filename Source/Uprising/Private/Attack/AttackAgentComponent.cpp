// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "AttackAgentComponent.h"

#include "Uprising.h"
#include "Hero/HeroCharacter.h"

#include "AttackComponent.h"

#include "Weapon/WeaponOwnerInterface.h"

#define TerminateIf(Condition) { if (Condition) { return; } }
#define TerminateIfExec(Condition, Exec) { if (Condition) { Exec; return; } }
#define TerminateIfWithVal(Condition, RetVal) { if (Condition) { return RetVal; } }
#define TerminateIfWithValExec(Condition, RetVal, Exec) { if (Condition) { Exec; return RetVal; } }

bool UAttackAgentComponent::QueryAttackCondition() const
{
	ENetRole OwnerRole = GetOwnerRole();

	// if OwnerRole == Authority, it must be a running server
	if (OwnerRole >= ENetRole::ROLE_Authority)
	{
		return CheckAttackCondition();
	}

	else if (OwnerRole >= ENetRole::ROLE_SimulatedProxy)
	{
		return CheckAttackCondition() && ServerQueryAttackCondition();
	}

	return false;
}

bool UAttackAgentComponent::ServerQueryAttackCondition() const
{
	return CheckAttackCondition();
}

bool UAttackAgentComponent::CheckAttackCondition() const
{
	return Status == EAttackStatus::Idle && IsActive() && IsRegistered() && !IsBeingDestroyed() && RelevantAttackComponent && RelevantAttackComponent->CheckCondition();
}

void UAttackAgentComponent::BeginAttack()
{
	if (CheckAttackCondition())
	{
		FName AttackID = RelevantAttackComponent->GetFName();
		
		BeginAttackProcess();
	}
}

FWeaponContext::FWeaponContext(AWeapon *InWeapon)
{
	OwningWeapon = InWeapon;
	WeaponOwner = InWeapon ? CastChecked<APawn>(InWeapon->GetOwner()) : nullptr;
}

bool FWeaponContext::IsAttackAllowed(UAttackType *AttackType)
{
	if (OwningWeapon.Get())
	{
		return OwningWeapon->CanStartAttack(AttackType) && IWeaponOwnerInterface::Execute_CanStartAttack(WeaponOwner.Get(), AttackType);
	}

	return true;
}

bool FWeaponContext::IsAttackSimulationAllowed(UAttackType *AttackType, int SimulationCount)
{
	if (OwningWeapon.Get())
	{
		return OwningWeapon->CanStartAttackSimulation(AttackType, SimulationCount) && IWeaponOwnerInterface::Execute_CanStartAttackSimulation(WeaponOwner.Get(), AttackType, SimulationCount);
	}

	return true;
}

bool FWeaponContext::AttackAnimationExists(FName AttackTypeName, EAttackAnimation::Target TargetType)
{
	if (TargetType == EAttackAnimation::Target_Weapon)
	{
		if (OwningWeapon.Get())
		{
			return OwningWeapon->AttackAnimationExists(AttackTypeName);
		}
	}

	else if (TargetType == EAttackAnimation::Target_WeaponOwner)
	{
		if (WeaponOwner.Get())
		{
			return IWeaponOwnerInterface::Execute_AttackAnimationExists(WeaponOwner.Get(), AttackTypeName);
		}
	}

	return false;
}

void FWeaponContext::PlayAttackAnimationOn(FName AttackTypeName, EAttackAnimation::Target TargetType)
{
	if (TargetType == EAttackAnimation::Target_Weapon)
	{
		if (OwningWeapon.Get())
		{
			return OwningWeapon->PlayAttackAnimation(AttackTypeName);
		}
	}

	else if (TargetType == EAttackAnimation::Target_WeaponOwner)
	{
		if (WeaponOwner.Get())
		{
			return IWeaponOwnerInterface::Execute_PlayAttackAnimation(WeaponOwner.Get(), AttackTypeName);
		}
	}
}

void FWeaponContext::AnimJumpToSection(EAttackAnimation::SectionType SectionType)
{
	if (TargetType == EAttackAnimation::Target_Weapon)
	{
		if (OwningWeapon.Get())
		{
			return OwningWeapon->AnimJumpToSection(SectionType);
		}
	}

	else if (TargetType == EAttackAnimation::Target_WeaponOwner)
	{
		if (WeaponOwner.Get())
		{
			return IWeaponOwnerInterface::Execute_AnimJumpToSection(WeaponOwner.Get(), SectionType);
		}
	}
}

UAttackComponent::UAttackComponent(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentSimulationCount = 0;
	CurrentState = EAttackState::Idle;
}

void UAttackComponent::AddAttackDamage(int32 DeltaDamage)
{
	if (DeltaDamage != 0)
	{
		PendingDamageToApply += DeltaDamage;
	}
}

void UAttackComponent::AddDamageMultiplier(float DeltaMultiplier)
{
	if (DeltaMultiplier != 0)
	{
		PendingDamageMultiplierToApply += DeltaMultiplier;
	}

bool UAttackComponent::CanStartAttack() const
{
	return Weapon && AttackInstigator && !IsBeingDestroyed() && CurrentState == EAttackState::Idle && InternalCanAttack();
}

void UAttackComponent::StartAttack(UAttackType *InAttackType)
{
	if (InAttackType && CanAttack())
	{
		CurrentType = InAttackType;

		CurrentState = EAttackState::Attacking;
		CurrentMode = CurrentType->AttackMode;
		
		AController *Controller = AttackInstigator->Controller;
		if (Controller)
		{
			bool bMovable = CurrentType->bCanMoveDuringAttack;

			if (bMovable)
			{
				if (Controller->IsMoveInputIgnored())
				{
					Controller->ResetIgnoreMoveInput();
				}
			}

			else
			{
				Controller->SetIgnoreMoveInput(true);

				UMovementComponent *MoveComp = AttackInstigator->FindComponentByClass<UMovementComponent>();
				if (MoveComp)
				{
					MoveComp->StopMovementImmediately();
				}
			}
		}

		FAttackEventParams Params(this, AttackInstigator, Weapon, CurrentType);
		Event_OnPreAttack();
		OnPreAttack.Broadcast(Params);

		ATTACK_LOG(TEXT("UAttackComponent::StartAttack"));

		PlayAttackAnimation();
	}

	else
	{
		ATTACK_ERROR(TEXT("Failed to start attack process: InAttackType is NULL!"));
	}
}

FORCEINLINE void UAttackComponent::StopAttack()
{
	TerminateIf(!CheckValidState());

	if (CurrentState == EAttackState::Attacking)
	{
		if (CurrentMode == EAttackMode::Charged)
		{
			BurstChargedAttack();
		}

		else
		{
			StopAttackAnimation();
		}
	}
}

FORCEINLINE bool UAttackComponent::IsFullyCharged() const
{
	TerminateIfWithVal(!(CheckValidState() && bAttackProcessStarted), false);

	return CurrentMode == EAttackMode::Charged && ElapsedTime >= CurrentType->MaxChargingTime;
}

void UAttackComponent::PlayAttackAnimation()
{
	TerminateIf(!CheckValidState());

	if (CurrentType->bUseCustomAttackAnimation)
	{
		IWeaponOwnerInterface::Execute_PlayCustomAttackAnimation(AttackInstigator, CurrentType);
	}

	else
	{
		ATTACK_LOG(TEXT("AttackInstigator=%i, ImplementsWeaponOwnerInterface?=%i"), AttackInstigator, (AttackInstigator ? (int32)(AttackInstigator->GetClass()->Implements<UWeaponOwnerInterface>()) : 0));

		CurrentAnimInstance = IWeaponOwnerInterface::Execute_GetCharacterAnimInstance(AttackInstigator);
		checkf(CurrentAnimInstance, TEXT("NULL is not allowed on IWeaponOwner::GetAttackAnimInstance()"));

		CurrentAnimMontage = CurrentType->AnimMontageData.AttackAnimMontage;
		checkf(CurrentAnimMontage, TEXT("UAnimMontage instance of current attack animation must not be NULL"))
			
		CurrentAnimInstance->Montage_Play(CurrentAnimMontage);

		ATTACK_LOG(TEXT("UAttackComponent::PlayAttackAnimation - Played AnimMontage"));

		FName StartSectionName = CurrentType->AnimMontageData.StartSectionName;
		if (!StartSectionName.IsNone() && CurrentAnimMontage->IsValidSectionName(StartSectionName))
		{
			CurrentAnimInstance->Montage_JumpToSection(StartSectionName, CurrentAnimMontage);

			ATTACK_LOG(TEXT("UAttackComponent::PlayAttackAnimation - Jumped to Start Section"));
		}		
	}
}

void UAttackComponent::StopAttackAnimation()
{
	TerminateIf(!CheckValidState());

	if (CurrentMode == EAttackMode::Continuous)
	{
		if (CurrentType->bUseCustomAttackAnimation)
		{
			IWeaponOwnerInterface::Execute_StopCustomAttackAnimation(AttackInstigator);
		}

		else
		{
			bAnimShouldJumpToDest = true;
		}
	}
}

void UAttackComponent::NotifyAnimationStarted()
{
	TerminateIf(!CheckValidState());

	ATTACK_LOG(TEXT("UAttackComponent::NotifyAnimationStarted"));

	bIsPlayingAnimation = true;

	FAttackEventParams Params(this, AttackInstigator, Weapon, CurrentType);
	Event_OnAnimationStarted(CurrentType->bUseCustomAttackAnimation, CurrentAnimMontage);
	OnAnimationStarted.Broadcast(Params, CurrentType->bUseCustomAttackAnimation, CurrentAnimMontage);

	StartAttackProcess();
}

void UAttackComponent::StartAttackProcess()
{
	TerminateIf(!CheckValidState());

	bAttackProcessStarted = true;

	FAttackEventParams Params(this, AttackInstigator, Weapon, CurrentType);
	Event_OnAttackStarted();
	OnAttackStarted.Broadcast(Params);
}

void UAttackComponent::NotifyStartSimulation()
{
	TerminateIf(!CheckValidState());

	ATTACK_LOG(TEXT("UAttackComponent::NotifySimulationStarted"));

	if (bAttackProcessStarted && CanStartSimulation())
	{
		StartAttackSimulation(CurrentType);
	}
}

void UAttackComponent::StartAttackSimulation(UAttackType *InAttackType)
{
	TerminateIf(!CheckValidState());

	if (GetWorld())
	{
		ATTACK_LOG(TEXT("UAttackComponent::StartAttackSimulation"));

		++CurrentSimulationCount;

		FAttackEventParams Params(this, AttackInstigator, Weapon, InAttackType);
		Event_OnSimulationStarted(CurrentSimulationCount);
		OnSimulationStarted.Broadcast(Params, CurrentSimulationCount);

		/* FAttackSimulationHandle Handle;
		++LastAssignedSimulationHandle;
		Handle.Handle = LastAssignedSimulationHandle;

		ATTACK_LOG(TEXT("LastAssignedSimulationHandle = %i"), LastAssignedSimulationHandle);
		//checkf(Handle.IsValid(), TEXT("Exceeded Max Simulation Count!"))

		PendingSimulationHandleMap.Add(Handle, GetWorld()->GetTimeSeconds()); */
		
		SimulateAttack(Handle, InAttackType, CurrentSimulationCount);
	}

	else
	{
		ATTACK_ERROR(TEXT("Failed to start attack simulation: GetWorld() returned NULL!"))
	}
}

void UAttackComponent::NotifySimulationCompleted(FAttackSimulationHandle Handle, FAttackSimulationResult &InResult)
{
	if (GetWorld())
	{
		ATTACK_LOG(TEXT("UAttackComponent::NotifySimulationCompleted: Simulation Handle: %i"), Handle.Handle);

		if (Handle.IsValid())
		{
			ATTACK_LOG(TEXT("Simulation Handle is Valid"))

			float SimulationStartTime;
			if (PendingSimulationHandleMap.RemoveAndCopyValue(Handle, SimulationStartTime))
			{
				ATTACK_LOG(TEXT("Simulation Handle exists in the map"))

				if (SimulationTimeOut <= 0.0f || GetWorld()->TimeSince(SimulationStartTime) < SimulationTimeOut)
				{
					ATTACK_LOG(TEXT("Simulation satisfies TimeOut condition"))

					if (InResult.bAnyHit)
					{
						ATTACK_LOG(TEXT("Hit occured during Simulation"))

						ApplyAttackDamage(InResult);
					}

					FAttackEventParams Params(this, AttackInstigator, Weapon, InResult.AttackType);
					Event_OnSimulationCompleted(InResult);
					OnSimulationCompleted.Broadcast(Params, InResult);

					Handle.Invalidate();
				}
			}
			
			// This simulation result is discarded
			else;
		}
	}

	else
	{
		ATTACK_ERROR(TEXT("Failed to complete attack simulation: GetWorld() returned NULL!"))
	}
}

void UAttackComponent::ApplyAttackDamage(FAttackSimulationResult &InResult)
{
	if (InResult.AttackType)
	{
		for (auto Result : InResult.AttackResults)
		{

			if (!Result.bHit || Result.HitInfo.GetComponent() == nullptr)
			{
				continue;
			}

			FAttackEventParams Params(this, AttackInstigator, Weapon, CurrentType);
			Event_OnAttackHit(Result.HitInfo);
			OnAttackHit.Broadcast(Params, Result.HitInfo);

			int32 FinalDamage = CalculateInitialAttackDamage(InResult.AttackType, Result.HitInfo) + PendingDamageToApply;
			float DamageMultiplier = PendingDamageMultiplierToApply;

			if (InResult.AttackType->AttackMode == EAttackMode::Charged)
			{

				if (InResult.AttackType->ChargedDamageMultiplier)
				{
					DamageMultiplier += InResult.AttackType->ChargedDamageMultiplier->GetFloatValue(InResult.AttackType->MaxChargingTime);
				}

				else
				{
					ATTACK_WARNING(TEXT("Failed to apply ChargedDamage Multiplier: UAttackType::ChargedDamageMultiplier is NULL!"))
				}
			}

			Result.bHeadShot = IsHeadShot(Result.HitInfo);
			if (Result.bHeadShot && Weapon->IsHeadShotAllowed() && Weapon->IsHeadShotAllowedOnTarget(Result.HitInfo.GetActor()))
			{
				DamageMultiplier += InResult.AttackType->HeadShotDamageMultiplier;
			}

			if (DamageMultiplier != 0)
			{
				FinalDamage *= DamageMultiplier;
			}

			Result.CalculatedDamage = FinalDamage;

			if (FinalDamage != 0)
			{
				if (Result.DamageTypeClass == nullptr)
				{
					Result.DamageTypeClass = UDamageType::StaticClass();
				}

				ATTACK_LOG(TEXT("Applied damage to %s: Damage: %i, Instigator: %s, DamageCauser: %i"), (Result.HitInfo.GetActor()? *Result.HitInfo.GetActor()->GetName() : TEXT("NULL")), FinalDamage, *AttackInstigator->GetName(), *Weapon->GetName());

				InternalApplyAttackDamage(Result.HitInfo.GetActor(), Result, AttackInstigator->GetController(), Weapon);
			}
		}
	}

	else
	{
		ATTACK_ERROR(TEXT("Failed to apply damage using the given simulation result: FAttackSimulationResult::AttackType is NULL!"))
	}
}

void UAttackComponent::NotifyJumpToDest()
{
	TerminateIf(!CheckValidState());

	if (bAnimShouldJumpToDest)
	{
		if (CurrentMode == EAttackMode::Continuous)
		{
			if (CurrentType->bUseCustomAttackAnimation)
			{
				IWeaponOwnerInterface::Execute_StopCustomAttackAnimation(AttackInstigator);
			}

			else
			{
				TerminateIfExec(!CheckValidAnimState(), ATTACK_ERROR(TEXT("Failed to jump to dest section: either UAttackComponent::CurrentAnimInstance or UAttackComponent::CurrentAnimMontage is NULL")));

				if (!TryJumpToSection(CurrentType->AnimMontageData.EndSectionName))
				{
					CurrentAnimInstance->Montage_Stop(CurrentAnimMontage->BlendOut.GetBlendTime(), CurrentAnimMontage);
					NotifyAnimationEnded();

					ATTACK_WARNING(TEXT("Failed to jump to End section: Invalid Section Name"));
				}
			}
		}

		else if (CurrentMode == EAttackMode::Charged)
		{
			if (CurrentType->bUseCustomAttackAnimation)
			{
				IWeaponOwnerInterface::Execute_PlayCustomBurstAttackAnimation(AttackInstigator, CurrentType);
			}

			else
			{
				TerminateIfExec(!CheckValidAnimState(), ATTACK_ERROR(TEXT("Failed to jump to dest section: either UAttackComponent::CurrentAnimInstance or UAttackComponent::CurrentAnimMontage is NULL")));

				if (!TryJumpToSection(CurrentType->AnimMontageData.BurstSectionName))
				{
					CurrentAnimInstance->Montage_Stop(CurrentAnimMontage->BlendOut.GetBlendTime(), CurrentAnimMontage);
					NotifyAnimationEnded();

					ATTACK_WARNING(TEXT("Failed to jump to Burst section: Invalid Section Name"));
				}
			}
		}
	}
}

bool UAttackComponent::TryJumpToSection(FName const& SectionName)
{
	TerminateIfWithVal(!CheckValidState(), false);

	if (!SectionName.IsNone() && CurrentAnimMontage->IsValidSectionName(SectionName))
	{
		CurrentAnimInstance->Montage_JumpToSection(SectionName, CurrentAnimMontage);
		return true;
	}

	return false;
}

bool UAttackComponent::CheckValidState() const
{
	return Weapon && AttackInstigator && CurrentType;
}

bool UAttackComponent::CheckValidAnimState() const
{
	return CurrentAnimInstance && CurrentAnimMontage;
}

void UAttackComponent::BurstChargedAttack()
{
	TerminateIf(!(CheckValidState() && CurrentMode == EAttackMode::Charged));

	FAttackEventParams Params(this, AttackInstigator, Weapon, CurrentType);
	Event_OnBurstChargedAttack(ElapsedTime, CurrentType->MaxChargingTime);
	OnBurstChargedAttack.Broadcast(Params, ElapsedTime, CurrentType->MaxChargingTime);

	if (IsFullyCharged())
	{
		bAnimShouldJumpToDest = true;
	}

	else
	{ 
		if (CurrentType->bUseCustomAttackAnimation)
		{
			IWeaponOwnerInterface::Execute_PlayCustomBurstAttackAnimation(AttackInstigator, CurrentType);
		}

		else
		{
			TerminateIfExec(!CheckValidAnimState(), ATTACK_ERROR(TEXT("Failed to burst charged attack: either UAttackComponent::CurrentAnimInstance or UAttackComponent::CurrentAnimMontage is NULL")));

			if (TryJumpToSection(CurrentType->AnimMontageData.BurstSectionName))
			{
				CurrentAnimInstance->Montage_Stop(CurrentAnimMontage->BlendOut.GetBlendTime(), CurrentAnimMontage);
				NotifyAnimationEnded();

				ATTACK_WARNING(TEXT("Failed to jump to Burst section: Invalid Section Name"));
			}
		}
	}
}

void UAttackComponent::NotifyAnimationEnded()
{
	TerminateIf(!CheckValidState());

	if (CurrentState == EAttackState::Attacking)
	{
		bIsPlayingAnimation = false;

		FAttackEventParams Params(this, AttackInstigator, Weapon, CurrentType);
		Event_OnAnimationEnded(CurrentType->bUseCustomAttackAnimation, CurrentAnimMontage);
		OnAnimationEnded.Broadcast(Params, CurrentType->bUseCustomAttackAnimation, CurrentAnimMontage);

		PostAttackProcess();
	}
}

void UAttackComponent::PostAttackProcess()
{
	TerminateIf(!CheckValidState());

	if (!bIsPlayingAnimation)
	{
		InternalPostAttackProcess();

		FAttackEventParams Params(this, AttackInstigator, Weapon, CurrentType);
		OnAttackCompleted.Broadcast(Params);

		if (!CurrentType->bCanMoveDuringAttack)
		{
			AController *Controller = AttackInstigator->Controller;
			if (Controller)
			{
				if (Controller->IsMoveInputIgnored())
				{
					Controller->SetIgnoreMoveInput(false);
				}
			}
		}

		CurrentState = EAttackState::CoolingDown;
		if (GetOwner())
		{
			GetOwner()->GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, FTimerDelegate::CreateUObject(this, &UAttackComponent::HandleAttackCooldownResolved), CurrentType->AttackCooldown, false);
		}

		else
		{
			HandleAttackCooldownResolved();
			ATTACK_WARNING(TEXT("Couldn't get an instancne of UAttackComponent's Owner: Skipped Cooldown Process"))
		}
	}
}

void UAttackComponent::ResetStates()
{
	CurrentType = nullptr;
	bAttackProcessStarted = false;
	bAnimShouldJumpToDest = false;
	CurrentSimulationCount = 0;
	CurrentAnimInstance = nullptr;
	CurrentAnimMontage = nullptr;
	ElapsedTime = 0.0f;
	PendingDamageToApply = 0;
	PendingDamageMultiplierToApply = 0.0f;
}

void UAttackComponent::HandleAttackCooldownResolved()
{
	TerminateIf(!CheckValidState())

	if (GetOwner())
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(AttackCooldownTimerHandle);
	}

	else
	{
		ATTACK_WARNING(TEXT("Couldn't get an instancne of UAttackComponent's Owner: Skipped Clearing Cooldown Timer"))
	}

	FAttackEventParams Params(this, AttackInstigator, Weapon, CurrentType);
	Event_OnAttackCooldownResolved();
	OnAttackCooldownResolved.Broadcast(Params);

	ResetStates();
	CurrentState = EAttackState::Idle;
}

/* void UAttackComponent::OnRegister()
{
	Super::OnRegister();

	if (GetDefault<UAttackComponent>() != this)
	{
		Weapon = Cast<AWeapon>(GetOwner());

		if (Weapon)
		{
			FScriptDelegate WeaponOwnerSetupDelegate;
			WeaponOwnerSetupDelegate.BindUFunction(this, TEXT("ReceiveWeaponOwnerSetup"));
			Weapon->OnWeaponOwnerSetup.Add(WeaponOwnerSetupDelegate);
		}

		else
		{
			ATTACK_ERROR(TEXT("Failed to initialize UAttackComponent: Weapon is NULL!"))
		}
	}
} */

void UAttackComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (WeaponContext == nullptr)
	{
		WeaponContext = new FWeaponContext(Cast<AWeapon>(GetOwner()));
	}
}

void UAttackComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	if (WeaponContext)
	{
		delete WeaponContext;
	}
}

void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TerminateIf(!CheckValidState());

	if (bAttackProcessStarted)
	{
		ElapsedTime += DeltaTime;

		if (IsFullyCharged())
		{
			BurstChargedAttack();
		}
	}
}

void UAttackComponent::ReceiveWeaponOwnerSetup(FWeaponEventParams const& InParams)
{
	AttackInstigator = InParams.WeaponOwner;
}

int32 UAttackComponent::CalculateInitialAttackDamage_Implementation(UAttackType *InType, FHitResult const& InHitInfo) const
{
	TerminateIfWithValExec(InType == nullptr, 0, ATTACK_ERROR(TEXT("Failed to calculate initial attack damage: InType is NULL!")));

	int32 FinalDamage = FMath::RandRange(InType->MinDamage, InType->MaxDamage);

	return FinalDamage > 0 ? FinalDamage : 0;
}

bool UAttackComponent::IsHeadShot_Implementation(FHitResult const& InHitInfo) const
{
	bool bHeadShot = false;

	if (InHitInfo.bBlockingHit && InHitInfo.GetActor())
	{
		AHeroCharacter *HeroTarget = Cast<AHeroCharacter>(InHitInfo.GetActor());
		if (HeroTarget)
		{
			bHeadShot = HeroTarget->IsHeadVolume(InHitInfo.GetComponent());
		}
	}

	return bHeadShot;
}

void UAttackComponent::InternalApplyAttackDamage_Implementation(AActor *Target, FAttackResult const& InResult, AController *InInstigator, AActor *InDamageCauser)
{
	TerminateIf(!CheckValidState());

	if (Target)
	{
		Target->TakeDamage(InResult.CalculatedDamage, FDefaultDamageEvent(InResult.CalculatedDamage, InResult.HitInfo, InResult.DamageTypeClass), InInstigator, InDamageCauser);
	}
}

APawn* UAttackComponent::GetAttackInstigator() const { return AttackInstigator; }
IWeaponOwnerInterface* UAttackComponent::GetWeaponOwner() const { return CastChecked<IWeaponOwnerInterface>(AttackInstigator); }
AWeapon* UAttackComponent::GetWeapon() const { return Weapon; }
UAttackType* UAttackComponent::GetCurrentAttackType() const { return CurrentType; }