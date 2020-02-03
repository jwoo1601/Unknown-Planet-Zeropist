// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "SimulatedSkillComponent.h"
#include "SkillContent.h"
#include "Event/GameEventTrigger.h"
#include "Misc/GameEventManager.h"
#include "Misc/Macros.h"

USimulatedSkillComponent::USimulatedSkillComponent(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	EventManager = CreateDefaultSubobject<UGameEventManager>(TEXT("SimulationEventManager"));
}

bool USimulatedSkillComponent::IsSimulating() const
{
	return bSimulating;
}

void USimulatedSkillComponent::NotifySimulationCompleted(FGameEventHandle const& InHandle)
{
	CheckAndReturn(SimulatedSkillContent);

	EventManager->MarkCompleted(InHandle);
}

void USimulatedSkillComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SimulatedSkillContent = Cast<USimulatedSkillContent>(GetSkillContent());
	if (SimulatedSkillContent && GetOwner())
	{
		SimulationTrigger.EventName = SimulatedSkillContent->TriggerName;
		SimulationTrigger.UniqueId = GetOwner()->GetUniqueID();
	}
}

void USimulatedSkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USimulatedSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckAndReturn(SimulatedSkillContent);

	if (IsSimulating())
	{
		UWorld *World = GetWorld();
		if (World)
		{
			if (bSimulating)
			{
				if (World->TimeSince(SimulationStartTime) < SimulatedSkillContent->SimulationDuration)
				{
					if (World->TimeSince(LastSimulationTime) >= SimulatedSkillContent->SimulationRate)
					{
						FGameEventHandle Handle;
						EventManager->RegisterGameEvent(Handle, FGameEventCallbackDelegate::CreateUObject(this, &USimulatedSkillComponent::ProcessSimulationCompleted), SimulatedSkillContent->SimulationTimeOut, true);

						SimulateSkill(Handle, SimulationCount);

						LastSimulationTime = FPlatformTime::Seconds();
					}
				}

				else
				{
					bTickSimulationEnded = true;
				}
			}
		}
	}
}

void USimulatedSkillComponent::OnRegister()
{
	Super::OnRegister();
}

void USimulatedSkillComponent::OnUnregister()
{
	Super::OnUnregister();
}

void USimulatedSkillComponent::StartSimulation(FGameEventTrigger const& InTrigger)
{
	CheckAndReturn(SimulatedSkillContent);

	if (!bSimulating && bShouldStartSimulation)
	{
		Event_OnPreSimulate();
		FSkillEventParams Params(GetOwnerAsPawn(), this, SimulatedSkillContent);
		OnPreSimulate.Broadcast(Params);

		if (SimulatedSkillContent->SimulationType == ESkillSimulationType::Prompt)
		{
			FGameEventHandle Handle;
			EventManager->RegisterGameEvent(Handle, FGameEventCallbackDelegate::CreateUObject(this, &USimulatedSkillComponent::ProcessSimulationCompleted), SimulatedSkillContent->SimulationTimeOut, true);

			SimulateSkill(Handle, SimulationCount);
		}

		else
		{
			// High Resolution Timer that provides millisecond-accuracy
			SimulationStartTime = FPlatformTime::Seconds();
		}

		bSimulating = true;
	}
}

void USimulatedSkillComponent::ProcessSimulationCompleted(FGameEventHandle const& InHandle)
{
	CheckAndReturn(SimulatedSkillContent);

	if (SimulatedSkillContent->SimulationType == ESkillSimulationType::Prompt || (SimulatedSkillContent->SimulationType == ESkillSimulationType::Tick && bTickSimulationEnded))
	{
		bSimulating = false;

		if (bTickSimulationEnded)
		{
			bTickSimulationEnded = false;
		}

		FSkillUseResult UseResult;
		PostSimulationProcess(UseResult);

		FSkillEventParams Params(GetOwnerAsPawn(), this, SimulatedSkillContent);
		OnPostSimulate.Broadcast(Params);

		NotifyUseCompleted(UseResult);
	}
}

void USimulatedSkillComponent::StopSimulation_Implementation(bool &bOutCanceled, FSkillUseResult &InOutResult)
{
	CheckAndReturn(SimulatedSkillContent);

	if (SimulatedSkillContent->SimulationType == ESkillSimulationType::Tick && !bTickSimulationEnded)
	{
		bSimulating = false;
		bOutCanceled = true;
	}

	else
	{
		bOutCanceled = false;
	}

	InOutResult.bSucceeded = false;
	InOutResult.bSkipCooldown = false;
}

void USimulatedSkillComponent::PostSimulationProcess_Implementation(FSkillUseResult &InOutResult)
{
	CheckAndReturn(SimulatedSkillContent);

	InOutResult.bSucceeded = true;
	InOutResult.bSkipCooldown = false;
}

void USimulatedSkillComponent::StartSkillProcess_Implementation()
{
	CheckAndReturn(SimulatedSkillContent);

	bShouldStartSimulation = true;
	FGameEventTrigger::Register(SimulationTrigger, FGameEventNotifyDelegate::CreateUObject(this, &USimulatedSkillComponent::StartSimulation));
}

void USimulatedSkillComponent::CancelSkillProcess_Implementation(bool &bOutCanceled, FSkillUseResult &InOutResult)
{
	CheckAndReturn(SimulatedSkillContent);

	if (bSimulating)
	{
		StopSimulation(bOutCanceled, InOutResult);
	}
}

void USimulatedSkillComponent::InternalResetProperties_Implementation()
{
	SimulationCount = 0;
	SimulationStartTime = 0.0f;
	LastSimulationTime = 0.0f;
}