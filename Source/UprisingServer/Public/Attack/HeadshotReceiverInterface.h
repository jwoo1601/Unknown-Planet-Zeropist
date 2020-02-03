// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HeadshotReceiverInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHeadshotReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

class UPRISINGSERVER_API IHeadshotReceiverInterface
{
	GENERATED_BODY()
		
public:

	UFUNCTION(BlueprintNativeEvent, Category = "Headshot")
	bool IsHeadshot(FHitResult const& InHitInfo) const;
	
};