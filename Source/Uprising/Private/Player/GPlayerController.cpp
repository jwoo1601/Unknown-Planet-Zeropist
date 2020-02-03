// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "ZPlayerController.h"

AZPlayerController::AZPlayerController(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	AOCharacterRotFixThreshold = 70.f;
	AOCharacterRotSpeed = 5.f;
}