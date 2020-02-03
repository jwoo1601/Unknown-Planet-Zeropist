// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"

DECLARE_LOG_CATEGORY_EXTERN(UprisingEditor, All, All);

class FUprisingEditorModule : public IModuleInterface
{

public:

	/* IModuleInterface Interface */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/* ~IModuleInterface Module*/

};