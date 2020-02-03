// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Client.h"

#include "SkillOwnerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USkillOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
*
*/
class CLIENT_API ISkillOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level")
	int32 GetOwnerLevel() const;

//	UFUNCTION(BlueprintNativeEvent, Category = "Event")
	virtual void AddLevelUpEvent(FScriptDelegate const& InDelegate) = 0;

//	UFUNCTION(BlueprintNativeEvent, Category = "Event")
	virtual void RemoveLevelUpEvent(FScriptDelegate const& InDelegate) = 0;

};
