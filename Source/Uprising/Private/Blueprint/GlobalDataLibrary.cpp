// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "GlobalDataBPLibrary.h"
#include "Client.h"
#include "Data/GlobalData.h"


UGlobalDataBPLibrary::UGlobalDataBPLibrary(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer) { }

UGlobalData* UGlobalDataBPLibrary::GetGlobalData(bool &IsValid) {
	IsValid = false;
	UGlobalData *Instance = Cast<UGlobalData>(GEngine->GameSingleton);

	if (!Instance)
		return nullptr;
	if (!Instance->IsValidLowLevel())
		return nullptr;

	IsValid = true;
	return Instance;
}

