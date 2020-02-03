// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "IHeroEditor.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHeroEditor, Log, All);

class IHeroEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{

public:

	virtual TSharedRef<IHeroEditor> CreateHeroEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost> InitToolkitHost, class UHeroContent *InHeroContent) = 0;

	DECLARE_DELEGATE_RetVal_TwoParams(TSharedRef<FExtender>, FHeroEditorToolbarExtender, const TSharedRef<FUICommandList> /*InCommandList*/, const TSharedRef<IHeroEditor> /*InHeroEditor*/);
	virtual TArray<FHeroEditorToolbarExtender>& GetAllHeroEditorToolbarExtenders() = 0;

};