// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "GameEventTrigger.generated.h"

USTRUCT(BlueprintType, meta = (HasNativeBreak = "Client.GameEventStatics.BreakGameEventTrigger", HasNativeMake = "Client.GameEventStatics.MakeGameEventTrigger"))
struct CLIENT_API FGameEventTrigger
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FName EventName;

	UPROPERTY()
	uint32 UniqueId;

	FGameEventTrigger() { }

	FGameEventTrigger(FName InEventName, uint32 InUniqueId) : EventName(InEventName), UniqueId(InUniqueId) { }

	FORCEINLINE bool operator==(FGameEventTrigger const& Other) const
	{
		return EventName == Other.EventName && UniqueId == Other.UniqueId;
	}

	bool Trigger() const;

	FORCEINLINE static bool TriggerEvent(FName EventName, uint32 UniqueId)
	{
		return NewEventTrigger(EventName, UniqueId).Trigger();
	}

	FORCEINLINE static FGameEventTrigger NewEventTrigger(FName EventName, uint32 UniqueId)
	{
		return FGameEventTrigger(EventName, UniqueId);
	}

	static bool Register(FGameEventTrigger const& InTrigger, struct FGameEventNotifyUnifiedDelegate InNotifyDelegate, uint32 NotifyCount = 1);

	static void Unregister(FGameEventTrigger const& InTrigger);

	friend FORCEINLINE uint32 GetTypeHash(FGameEventTrigger const& InTrigger)
	{
		return InTrigger.EventName.GetComparisonIndex();
	} 

};

DECLARE_DELEGATE_OneParam(FGameEventNotifyDelegate, FGameEventTrigger const&);
DECLARE_DYNAMIC_DELEGATE_OneParam(FGameEventNotifyDynamicDelegate, FGameEventTrigger const&, InTrigger);

struct FGameEventNotifyUnifiedDelegate
{

	FGameEventNotifyDelegate Delegate;
	FGameEventNotifyDynamicDelegate DynamicDelegate;
	TFunction<void(FGameEventTrigger const&)> Function;

	FGameEventNotifyUnifiedDelegate(FGameEventNotifyDelegate const& InDelegate) : Delegate(InDelegate) { }
	FGameEventNotifyUnifiedDelegate(FGameEventNotifyDynamicDelegate const& InDynamicDelegate) : DynamicDelegate(InDynamicDelegate) { }
	FGameEventNotifyUnifiedDelegate(TFunction<void(FGameEventTrigger const&)> &&InFunction) : Function(InFunction) { }

	inline void Execute(FGameEventTrigger const& InTrigger)
	{
		if (Delegate.IsBound())
		{
#if STATS
			TStatId StatId = TStatId();
			UObject* Object = Delegate.GetUObject();
			if (Object)
			{
				StatId = Object->GetStatID();
			}
			FScopeCycleCounter Context(StatId);
#endif
			Delegate.Execute(InTrigger);
		}

		else if (DynamicDelegate.IsBound())
		{
			DynamicDelegate.Execute(InTrigger);
		}

		else if (Function)
		{
			Function(InTrigger);
		}
	}

	FORCEINLINE bool IsBound() const
	{
		return (Delegate.IsBound() || DynamicDelegate.IsBound() || Function);
	}

	inline bool IsBoundToObject(void const* Object) const
	{
		if (Delegate.IsBound())
		{
			return Delegate.IsBoundToObject(Object);
		}

		else if (DynamicDelegate.IsBound())
		{
			return DynamicDelegate.IsBoundToObject(Object);
		}

		return false;
	}

	FORCEINLINE void Unbind()
	{
		Delegate.Unbind();
		DynamicDelegate.Unbind();
		Function = nullptr;
	}

};

struct FGameEventNotifyData
{

	FGameEventNotifyData(FGameEventNotifyUnifiedDelegate InDelegate, uint32 NotifyCount) : Delegate(InDelegate), LeftCallCount(NotifyCount) { }

	FORCEINLINE void Notify(FGameEventTrigger const& InTrigger)
	{
		if (LeftCallCount > 0)
		{
			--LeftCallCount;
			Delegate.Execute(InTrigger);
		}

		if (LeftCallCount <= 0)
		{
			FGameEventTrigger::Unregister(InTrigger);
		}
	}

private:

	FGameEventNotifyUnifiedDelegate Delegate;

	uint32 LeftCallCount;

};

// TMap<struct FGameEventTrigger, struct FGameEventNotifyData> GEventTriggerMap;

/* struct CLIENT_API FGameEventTriggerStatics
{
	friend struct FGameEventTrigger;

	static 

}; */