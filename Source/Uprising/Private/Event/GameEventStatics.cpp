// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "GameEventStatics.h"

void UGameEventStatics::BreakGameEventTrigger(FGameEventTrigger const& Trigger, FName &EventName, int32 &UniqueId)
{
	EventName = Trigger.EventName;
	UniqueId = (int32)(Trigger.UniqueId);
}

FGameEventTrigger UGameEventStatics::MakeGameEventTrigger(FName EventName, int32 UniqueId)
{
	return FGameEventTrigger::NewEventTrigger(EventName, (uint32)UniqueId);
}

FGameEventTrigger UGameEventStatics::NewGameEventTriggerFromUObject(FName EventName, UObject *InObject)
{
	return FGameEventTrigger::NewEventTrigger(EventName, (InObject ? InObject->GetUniqueID() : 0U));
}

bool UGameEventStatics::RegisterTrigger(FGameEventTrigger const& Trigger, FGameEventNotifyDynamicDelegate const& NotifyCallback, int32 NotifyCount)
{
	return FGameEventTrigger::Register(Trigger, NotifyCallback, NotifyCount);
}

void UGameEventStatics::UnregisterTrigger(FGameEventTrigger const& Trigger)
{
	FGameEventTrigger::Unregister(Trigger);
}

bool UGameEventStatics::Trigger(FGameEventTrigger const& InTrigger)
{
	return InTrigger.Trigger();
}