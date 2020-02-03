// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "SkillComponent.h"
#include "Client.h"
#include "Hero/HeroCharacter.h"
#include "SkillContent.h"
#include "SkillOwnerInterface.h"
#include "Misc/Macros.h"

USkillComponent::USkillComponent(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

	ProcessOwnerLevelUpDelegate.BindUFunction(this, TEXT("ProcessHeroLevelUp"));
}

bool USkillComponent::IsUsable() const
{
	return SkillContent && SkillContent->UsageType == ESkillUsageType::Active;
}

bool USkillComponent::CanUse() const
{
	return IsUsable() && bIsActive && CurrentStatus == ESkillStatus::Idle && !IsBeingDestroyed() && InternalCanUse();
}

void USkillComponent::StartUse()
{
	CheckAndReturnf(SkillContent, TEXT("Failed to start using skill: SkillContent is NULL!"));

	CLIENT_LOG(TEXT("Called %s::%s, IsUsable: %d, SkillStatus: %d, CurrentCooldown: %d"), *GetClass()->GetName(), TEXT(__FUNCTION__), IsUsable(), (uint8)CurrentStatus, CurrentCooldown);

	if (CanUse())
	{
		CurrentStatus = ESkillStatus::Using;
		CurrentCooldown = ActualCooldown;

		Event_OnUseStarted();
		FSkillEventParams Params(PawnOwner, this, SkillContent);
		OnUseStarted.Broadcast(Params);

		StartSkillProcess();
	}
}

void USkillComponent::StopUse()
{
	CheckAndReturn(SkillContent);

	if (IsUsable() && CurrentStatus == ESkillStatus::Using)
	{
		bool bCanceled;
		FSkillUseResult Result;

		CancelSkillProcess(bCanceled, Result);

		if (bCanceled)
		{
			Event_OnUseCanceled(Result);
			FSkillEventParams Params(PawnOwner, this, SkillContent);
			OnUseCanceled.Broadcast(Params, Result);

			if (Result.bSkipCooldown)
			{
				ResetProperties();
			}

			else
			{
				CurrentStatus = ESkillStatus::CoolingDown;
			}
		}
	}
}

void USkillComponent::NotifyUseCompleted(FSkillUseResult const& InResult)
{
	CheckAndReturn(SkillContent);

	if (CurrentStatus == ESkillStatus::Using)
	{
		PostSkillProcess(InResult);

		Event_OnUseCompleted(InResult);
		FSkillEventParams Params(PawnOwner, this, SkillContent);
		OnUseCompleted.Broadcast(Params, InResult);

		if (InResult.bSkipCooldown)
		{
			ResetProperties();
		}

		else
		{
			CurrentStatus = ESkillStatus::CoolingDown;
		}
	}
}

void USkillComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (SkillContentClass)
	{
		SkillContent = SkillContentClass->GetDefaultObject<USkillContent>();
	}
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	CheckAndReturn(SkillContent);

	ActualCooldown = SkillContent->GetCooldownforLevel(ISkillOwnerInterface::Execute_GetOwnerLevel(PawnOwner));
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckAndReturn(SkillContent);

	if (SkillContent->UsageType == ESkillUsageType::Passive && bIsActive)
	{
		TickPassiveEffect(DeltaTime);

		Event_OnPassiveEffectApplied();
		FSkillEventParams Params(PawnOwner, this, SkillContent);
		OnPassiveEffectApplied.Broadcast(Params);
	}

	if (CurrentStatus == ESkillStatus::CoolingDown)
	{
		CurrentCooldown -= DeltaTime;

		if (CurrentCooldown <= 0.0f)
		{
			CurrentCooldown = 0.0f;

			ResetProperties();

			Event_OnCooldownResolved();
			FSkillEventParams Params(PawnOwner, this, SkillContent);
			OnCooldownResolved.Broadcast(Params);			

			CurrentStatus = ESkillStatus::Idle;
		}
	}
}

void USkillComponent::Activate(/* Deprecated */ bool bReset)
{
	if (CanActivate())
	{
		SetComponentTickEnabled(true);
		bIsActive = true;

		OnSkillActivated();
		OnComponentActivated.Broadcast(this, bReset);
	}
}

void USkillComponent::Deactivate()
{
	if (CanDeactivate())
	{
		SetComponentTickEnabled(false);
		bIsActive = false;

		OnSkillDeactivated();
		OnComponentDeactivated.Broadcast(this);
	}
}

bool USkillComponent::CanDeactivate() const
{
	return CurrentStatus == ESkillStatus::Idle;
}

void USkillComponent::OnRegister()
{
	Super::OnRegister();

	if (GetClass()->GetDefaultObject() != this)
	{
		PawnOwner = Cast<APawn>(GetOwner());
		checkf(PawnOwner, TEXT("Owner of USkillComponent must be APawn or derived type of it"));

		ISkillOwnerInterface *SkillOwner = Cast<ISkillOwnerInterface>(GetOwner());
		checkf(SkillOwner, TEXT("Owner of USkillComponent must implement ISkillOwnerInterface"));

		SkillOwner->AddLevelUpEvent(ProcessOwnerLevelUpDelegate);
	}
}

void USkillComponent::OnUnregister()
{
	Super::OnUnregister();

	PawnOwner = nullptr;

	GetOwnerAsSkillOwner()->RemoveLevelUpEvent(ProcessOwnerLevelUpDelegate);
}

void USkillComponent::ResetCooldown()
{
	CheckAndReturn(SkillContent);

	if (CurrentCooldown > 0.0f)
	{
		CurrentCooldown = 0.0f;
	}
}

void USkillComponent::HandleInterceptedAttackInput_Impl(EInputEvent InputType)
{
	CheckAndReturn(SkillContent);

	HandleInterceptedAttackInput(InputType);

	FSkillEventParams Params(PawnOwner, this, SkillContent);
	OnInterceptedAttackInput.Broadcast(Params, InputType);
}

void USkillComponent::PostSkillProcess(FSkillUseResult const& InResult)
{
	InternalPostSkillProcess(InResult);
}

void USkillComponent::ResetProperties()
{
	InternalResetProperties();
}

void USkillComponent::ProcessOwnerLevelUp(const FActorLevelUpEvent &e)
{
	if (SkillContent)
	{
		ActualCooldown = SkillContent->GetCooldownforLevel(e.NewLevel);
	}
}