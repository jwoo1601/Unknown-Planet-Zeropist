// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameEventTrigger.h"

#include "GameEventStatics.generated.h"

UCLASS()
class CLIENT_API UGameEventStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Trigger", meta = (NativeBreakFunc))
	static void BreakGameEventTrigger(FGameEventTrigger const& Trigger, FName &EventName, int32 &UniqueId);

	UFUNCTION(BlueprintPure, Category = "Trigger", meta = (NativeMakeFunc))
	static struct FGameEventTrigger MakeGameEventTrigger(FName EventName, int32 UniqueId);

	UFUNCTION(BlueprintPure, Category = "Trigger")
	static FGameEventTrigger NewGameEventTriggerFromUObject(FName EventName, UObject *InObject);

	UFUNCTION(BlueprintCallable, Category = "Trigger")
	static bool RegisterTrigger(FGameEventTrigger const& Trigger, FGameEventNotifyDynamicDelegate const& NotifyCallback, int32 NotifyCount);

	UFUNCTION(BlueprintCallable, Category = "Trigger")
	static void UnregisterTrigger(FGameEventTrigger const& Trigger);

	UFUNCTION(BlueprintCallable, Category = "Trigger")
	static bool Trigger(FGameEventTrigger const& InTrigger);

};