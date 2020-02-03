// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "IHeroEditorModule.h"

class FHeroEditorModule : public IHeroEditorModule
{

public:
	
	FHeroEditorModule()
	{

	}

	virtual void StartupModule() override
	{
		MenuExtensibilityManager = MakeShareable<FExtensibilityManager>(new FExtensibilityManager());
		ToolbarExtensibilityManager = MakeShareable<FExtensibilityManager>(new FExtensibilityManager());
	}

	virtual void ShutdownModule() override
	{
		MenuExtensibilityManager.Reset();
		ToolbarExtensibilityManager.Reset();
	}

	virtual TSharedRef<IHeroEditor> CreateHeroEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost> InitToolkitHost, class UHeroContent *InHeroContent) override
	{

	}

	virtual TArray<FHeroEditorToolbarExtender>& GetAllHeroEditorToolbarExtenders() override
	{
		return HeroEditorToolbarExtenders;
	}

	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override
	{
		return MenuExtensibilityManager;
	}

	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override
	{
		return ToolbarExtensibilityManager;
	}

private:

	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolbarExtensibilityManager;

	TArray<FHeroEditorToolbarExtender> HeroEditorToolbarExtenders;

};

IMPLEMENT_MODULE(FHeroEditorModule, HeroEditor);