// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "GameEventManager.h"
#include "Client.h"

UGameEventManager::UGameEventManager(FObjectInitializer const& ObjectInitializer) : UObject(ObjectInitializer)
{

}

void UGameEventManager::Tick(float DeltaTime)
{
//	Super::Tick(DeltaTime);

	if (bNeedsReset)
	{
		ResetEvents();
	}

	else
	{
		for (auto It = UnderlyingMap.CreateIterator(); It; ++It)
		{
			FGameEventHandle &Handle = It.Key();
			FGameEventData &Data = It.Value();

			if (Data.Status == EGameEventStatus::Active)
			{
				Data.ElapsedTime += DeltaTime;

				if (GetWorld()->GetTimeSeconds() >= Data.ExpireTime)
				{
					Data.Status = EGameEventStatus::Expired;

					if (Data.Delegate.IsBound())
					{
						Data.Delegate.Execute(Handle, false, Data.ElapsedTime);
					}

					Handle.Invalidate();
					//				UnderlyingMap.Remove(It.Key()); -> We're going to remove this data on the next tick
				}
			}

			else if (Data.Status == EGameEventStatus::Completed)
			{
				Data.Status = EGameEventStatus::Executing;

				if (Data.Delegate.IsBound())
				{
					Data.Delegate.Execute(Handle, true, Data.ElapsedTime);
				}

				Handle.Invalidate();

				UnderlyingMap.Remove(It.Key());
			}

			else if (Data.Status == EGameEventStatus::Expired && Data.bAutoRemoveWhenExpired)
			{
				UnderlyingMap.Remove(It.Key());
			}
		}
	}
}

bool UGameEventManager::IsTickable() const
{
	return !bPaused;
}

TStatId UGameEventManager::GetStatId() const
{
	return GetStatId();
}

void UGameEventManager::CalculateExpireTime(FGameEventData &InData)
{
	float LeftTime = InData.TimeOutSeconds - InData.ElapsedTime;

	if (LeftTime >= 0.0f)
	{
		InData.ExpireTime = GetWorld()->GetTimeSeconds() + LeftTime;
	}

	else
	{
		if (InData.TimeOutSeconds <= 0.0f)
		{
			InData.ExpireTime = 0.0f;
		}

		else
		{
			InData.Status = EGameEventStatus::Expired;
		}
	}
}

void UGameEventManager::InternalRegisterGameEvent(FGameEventHandle &InOutHandle, FGameEventCallbackUnifiedDelegate const& InDelegate, float TimeOut /*= -1.0f*/, bool bAutoRemoveWhenExpired /*= true*/)
{
	NewHandle(InOutHandle);

	if (InOutHandle.IsValid() && !UnderlyingMap.Contains(InOutHandle))
	{
		FGameEventData Data(bAutoRemoveWhenExpired, InDelegate, 0.0f, (TimeOut <= 0.0f ? DefaultTimeOutSeconds : TimeOut));
		CalculateExpireTime(Data);

		UnderlyingMap.Add(InOutHandle, Data);
	}

	else
	{
		CLIENT_ERROR(TEXT("Newly allocate handle is invalid! Canceled registering new game event"))
	}
}

void UGameEventManager::NewHandle(FGameEventHandle &InOutHandle)
{
	if (!InOutHandle.IsValid())
	{
		if (++LastAssignedHandle == FGameEventHandle::INVALID_GAME_EVENT_HANDLE)
		{
			bNeedsReset = true;
		}

		else
		{
			InOutHandle.Handle = LastAssignedHandle;
		}

	}
}