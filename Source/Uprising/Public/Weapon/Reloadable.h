// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Reloadable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UReloadable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CLIENT_API IReloadable
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Reload")
	bool CanReload() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Reload")
	void Reload();

	virtual void AddReloadStartedEvent(FScriptDelegate const& InDelegate) = 0;
	virtual void AddReloadCompletedEvent(FScriptDelegate const& InDelegate) = 0;

	virtual void RemoveReloadStartedEvent(FScriptDelegate const& InDelegate) = 0;
	virtual void RemoveReloadCompletedEvent(FScriptDelegate const& InDelegate) = 0;

};
