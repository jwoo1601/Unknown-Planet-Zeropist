// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GlobalDataBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UGlobalDataBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UGlobalDataBPLibrary(const FObjectInitializer &ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "Global Data")
	static class UGlobalData* GetGlobalData(bool &IsValid);
	
};
