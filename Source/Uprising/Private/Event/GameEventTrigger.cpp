// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "GameEventTrigger.h"
#include "Data/GlobalData.h"

bool FGameEventTrigger::Trigger() const
{
	UGlobalData *GDataInst = UGlobalData::Get();

	if (GDataInst)
	{
		if (!EventName.IsNone() && GDataInst->TriggerMap.Contains(*this))
		{
			GDataInst->TriggerMap[*this].Notify(*this);

			return true;
		}
	}

	return false;
}

bool FGameEventTrigger::Register(FGameEventTrigger const& InTrigger, FGameEventNotifyUnifiedDelegate InNotifyDelegate, uint32 NotifyCount/* = 1*/)
{
	UGlobalData *GDataInst = UGlobalData::Get();

	if (GDataInst)
	{
		if (GDataInst->TriggerMap.Contains(InTrigger) || InTrigger.EventName.IsNone())
		{
			return false;
		}

		GDataInst->TriggerMap.Add(InTrigger, FGameEventNotifyData(InNotifyDelegate, NotifyCount));
		return true;
	}

	return false;
}

void FGameEventTrigger::Unregister(FGameEventTrigger const& InTrigger)
{
	UGlobalData *GDataInst = UGlobalData::Get();

	if (GDataInst)
	{
		if (GDataInst->TriggerMap.Contains(InTrigger))
		{
			GDataInst->TriggerMap.Remove(InTrigger);
		}
	}
}