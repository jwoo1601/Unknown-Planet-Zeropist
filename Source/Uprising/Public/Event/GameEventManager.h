// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"

#include "GameEventManager.generated.h"

class UGameEventManager;

USTRUCT(BlueprintType)
struct UPRISING_API FGameEventHandle
{
	GENERATED_USTRUCT_BODY()

	friend class UGameEventManager;

	FGameEventHandle() : Handle(INVALID_GAME_EVENT_HANDLE) { }

	FORCEINLINE bool IsValid() const
	{
		return Handle != INVALID_GAME_EVENT_HANDLE;
	}

	FORCEINLINE void Invalidate()
	{
		Handle = 0;
	}

	FORCEINLINE bool operator==(FGameEventHandle const& Other) const
	{
		return Other.Handle == Handle;
	}

	FORCEINLINE bool operator!=(FGameEventHandle const& Other) const
	{
		return Other.Handle != Handle;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("%i"), Handle);
	}

	uint32 Value() const
	{
		return Handle;
	}

	enum { INVALID_GAME_EVENT_HANDLE = 0 };

private:

	UPROPERTY(Transient)
	uint32 Handle;

	friend uint32 GetTypeHash(FGameEventHandle InHandle)
	{
		return InHandle.Handle;
	}

};

enum class EGameEventStatus : uint8
{
	Active,
	Paused,
	Completed,
	Executing,
	Expired
};

DECLARE_DELEGATE_TwoParams(FGameEventCallbackDelegate, FGameEventHandle&, bool, bCompleted, float, ElapsedTime);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FGameEventCallbackDynamicDelegate, FGameEventHandle&, InHandle, bool, bCompleted, float, ElapsedTime);

struct FGameEventCallbackUnifiedDelegate
{

	FGameEventCallbackDelegate Delegate;
	FGameEventCallbackDynamicDelegate DynamicDelegate;
	TFunction<void(FGameEventHandle const&, bool, float)> FuncCallback;

	FGameEventCallbackUnifiedDelegate() { }
	FGameEventCallbackUnifiedDelegate(FGameEventCallbackDelegate const& InDelegate) : Delegate(InDelegate) { }
	FGameEventCallbackUnifiedDelegate(FGameEventCallbackDynamicDelegate const& InDynDelegate) : DynamicDelegate(InDynDelegate) { }
	FGameEventCallbackUnifiedDelegate(TFunction<void(FGameEventHandle&, bool, float)> &&InFunc) : FuncCallback(InFunc) { }

	inline void Execute(FGameEventHandle &InHandle, bool bCompleted, float ElapsedTime)
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
			Delegate.Execute(InHandle, bCompleted, ElapsedTime);
		}

		else if (DynamicDelegate.IsBound())
		{
			DynamicDelegate.Execute(InHandle, bCompleted, ElapsedTime);
		}

		else if (FuncCallback)
		{
			FuncCallback(InHandle, bCompleted, ElapsedTime);
		}
	}

	FORCEINLINE bool IsBound() const
	{
		return (Delegate.IsBound() || DynamicDelegate.IsBound() || FuncCallback);
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
		FuncCallback = nullptr;
	}

};

struct FGameEventData
{

	EGameEventStatus Status;

	uint8 bAutoRemoveWhenExpired : 1;

	FGameEventCallbackUnifiedDelegate Delegate;

	float ElapsedTime;

	float ExpireTime;
	
	float TimeOutSeconds;

	FGameEventData(bool bInAutoRemoveWhenExpired, FGameEventCallbackUnifiedDelegate InDelegate, float InExpireTime, float InTimeOutSeconds) : bAutoRemoveWhenExpired(bInAutoRemoveWhenExpired), Delegate(InDelegate), ExpireTime(InExpireTime), TimeOutSeconds(InTimeOutSeconds) { }

	FORCEINLINE bool IsCompleted() const
	{
		return Status == EGameEventStatus::Completed;
	}

	FORCEINLINE bool IsExpired() const
	{
		return Status == EGameEventStatus::Expired;
	}

};

UCLASS(EditInlineNew)
class UPRISING_API UGameEventManager : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:

	UGameEventManager(FObjectInitializer const& ObjectInitializer);

	template<typename UserClass>
	FORCEINLINE void RegisterGameEvent(FGameEventHandle &InOutHandle, UserClass *InObject, typename FGameEventCallbackDelegate::TUObjectMethodDelegate_ThreeVars<UserClass, FGameEventHandle&, bool, float>::FMethodPtr InCallbackMethod, float TimeOut = -1.0f, bool bAutoRemoveWhenExpired = true)
	{
		InternalRegisterGameEvent(InOutHandle, FGameEventCallbackUnifiedDelegate(FGameEventCallbackDelegate::CreateUObject(InObject, InCallbackMethod)), TimeOut, bAutoRemoveWhenExpired);
	}

	template<typename UserClass>
	FORCEINLINE void RegisterGameEvent(FGameEventHandle &InOutHandle, UserClass *InObject, typename FGameEventCallbackDelegate::TUObjectMethodDelegate_OneVar_Const<UserClass, FGameEventHandle&, bool, float>::FMethodPtr InCallbackMethod, float TimeOut = -1.0f, bool bAutoRemoveWhenExpired = true)
	{
		InternalRegisterGameEvent(InOutHandle, FGameEventCallbackUnifiedDelegate(FGameEventCallbackDelegate::CreateUObject(InObject, InCallbackMethod)), TimeOut, bAutoRemoveWhenExpired);
	}

	FORCEINLINE void RegisterGameEvent(FGameEventHandle &InOutHandle, FGameEventCallbackDelegate InCallbackDelegate, float TimeOut = -1.0f, bool bAutoRemoveWhenExpired = true)
	{
		InternalRegisterGameEvent(InOutHandle, FGameEventCallbackUnifiedDelegate(InCallbackDelegate), TimeOut, bAutoRemoveWhenExpired);
	}

	FORCEINLINE void RegisterGameEvent(FGameEventHandle &InOutHandle, FGameEventCallbackDynamicDelegate InCallbackDelegate, float TimeOut = -1.0f, bool bAutoRemoveWhenExpired = true)
	{
		InternalRegisterGameEvent(InOutHandle, FGameEventCallbackUnifiedDelegate(InCallbackDelegate), TimeOut, bAutoRemoveWhenExpired);
	}

	FORCEINLINE void RegisterGameEvent(FGameEventHandle &InOutHandle, TFunction<void(FGameEventHandle&, bool, float)> &&InCallbackFunc, float TimeOut = -1.0f, bool bAutoRemoveWhenExpired = true)
	{
		InternalRegisterGameEvent(InOutHandle, FGameEventCallbackUnifiedDelegate(MoveTemp(InCallbackFunc)), TimeOut, bAutoRemoveWhenExpired);
	}

	FORCEINLINE void RegisterGameEvent(FGameEventHandle &InOutHandle, float TimeOut = -1.0f, bool bAutoRemoveWhenExpired = true)
	{
		InternalRegisterGameEvent(InOutHandle, FGameEventCallbackUnifiedDelegate(), TimeOut, bAutoRemoveWhenExpired);
	}

	FORCEINLINE bool EventExists(FGameEventHandle const& InHandle)
	{
		return UnderlyingMap.Contains(InHandle);
	}

	FORCEINLINE void MarkCompleted(FGameEventHandle const& InHandle)
	{
		FGameEventData *DataPtr = UnderlyingMap.Find(InHandle);

		if (DataPtr && DataPtr->Status == EGameEventStatus::Active)
		{
			DataPtr->Status = EGameEventStatus::Completed;
		}
	}

	FORCEINLINE bool IsEventExpired(FGameEventHandle const& InHandle) const
	{
		auto *DataPtr = UnderlyingMap.Find(InHandle);

		if (DataPtr)
		{
			return DataPtr->IsExpired();
		}

		return false;
	}

	inline void MarkExpired(FGameEventHandle const& InHandle)
	{
		FGameEventData *DataPtr = UnderlyingMap.Find(InHandle);

		if (DataPtr && DataPtr->Status == EGameEventStatus::Active)
		{
			DataPtr->Status = EGameEventStatus::Expired;

			if (DataPtr->bAutoRemoveWhenExpired)
			{
				UnderlyingMap.Remove(InHandle);
			}
		}
	}

	FORCEINLINE void PauseAll()
	{
		if (!bPaused)
		{
			bPaused = true;
		}
	}

	FORCEINLINE void UnpauseAll()
	{
		if (bPaused)
		{
			RecalculateExpireTime();
			bPaused = false;
		}
	}

	FORCEINLINE bool IsPaused(FGameEventHandle const& InHandle)
	{
		FGameEventData *DataPtr = UnderlyingMap.Find(InHandle);

		return DataPtr && DataPtr->Status == EGameEventStatus::Paused;
	}

	FORCEINLINE void Pause(FGameEventHandle const& InHandle)
	{
		FGameEventData *DataPtr = UnderlyingMap.Find(InHandle);

		if (DataPtr && DataPtr->Status == EGameEventStatus::Active)
		{
			DataPtr->Status = EGameEventStatus::Paused;
		}
	}

	FORCEINLINE void Unpause(FGameEventHandle const& InHandle)
	{
		FGameEventData *DataPtr = UnderlyingMap.Find(InHandle);

		if (DataPtr && DataPtr->Status == EGameEventStatus::Paused)
		{
			CalculateExpireTime(*DataPtr);
			DataPtr->Status = EGameEventStatus::Active;
		}
	}

	inline void ResetEvents()
	{
		for (auto It = UnderlyingMap.CreateIterator(); It; ++It)
		{
			FGameEventData &Data = It.Value();
			
			if (Data.Status == EGameEventStatus::Active)
			{
				if (Data.Delegate.IsBound())
				{
					Data.Delegate.Execute(It.Key());
				}
			}
		}

		UnderlyingMap.Reset();
	}

public:

	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override;

	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

private:

	void CalculateExpireTime(FGameEventData &InData);

	FORCEINLINE void RecalculateExpireTime()
	{
		for (auto It = UnderlyingMap.CreateIterator(); It; ++It)
		{
			CalculateExpireTime(It.Value());
		}
	}

	void InternalRegisterGameEvent(FGameEventHandle &InOutHandle, FGameEventCallbackUnifiedDelegate const& InDelegate, float TimeOut = -1.0f, bool bAutoRemoveWhenExpired = true);

	void NewHandle(FGameEventHandle &InOutHandle);

private:

	UPROPERTY()
	uint8 bPaused : 1;

	UPROPERTY()
	uint8 bNeedsReset : 1;

	UPROPERTY()
	uint32 LastAssignedHandle;

	TMap<FGameEventHandle, FGameEventData> UnderlyingMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimeOut", meta = (AllowPrivateAccess = "true", UIMin = "0.0", ClampMin = "0.0"))
	float DefaultTimeOutSeconds;

public:

	FORCEINLINE bool IsPaused() const { return bPaused; }
	FORCEINLINE void SetDefaultTimeOutSeconds(float InTimeOutSeconds) { DefaultTimeOutSeconds = InTimeOutSeconds; }
	FORCEINLINE float GetDefaultTimeOutSeconds() const { return DefaultTimeOutSeconds; }

};

