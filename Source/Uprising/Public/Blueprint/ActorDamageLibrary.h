// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Misc/DamageMisc.h"

#include "ActorDamageBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UActorDamageBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Damage", meta = (WorldContext = "WorldContextObject"))
	static void ActorReactToDamage(UObject const* WorldContextObject, FDamageableActorState const& InCurrentActorState, int32 InitialDamage, FDamageableActorState &OutFinalActorState, int32 &OutFinalDamage);
	
};
