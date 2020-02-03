// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "TickableEditorObject.h"
#include "UObject/GCObject.h"

#include "IHeroEditor.h"

namespace HeroEditorModes
{
	extern const FName HeroEditorMode;
}

namespace HeroEditorTabs
{
	extern const FName DetailsTab;
	extern const FName ViewportTab;

}

class FHeroEditor : public IHeroEditor, public FGCObject, public FEditorUndoClient, public FTickableEditorObject
{

public:

	FHeroEditor();

	virtual ~FHeroEditor();

	void InitHeroEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost> &InitToolkitHost, UHeroContent *InHeroContent);

	/* IHeroEditor Interface */
	virtual void SetHeroContent(UHeroContent *HeroContent) override;
	/* ~IHeroEditor Interface*/

	/* FGCObject Interface */
	virtual void AddReferencedObjects(FReferenceCollector &ReferenceCollector) override;
	/* ~FGCObject Interface */

	/* FEditorUndoClient Interface */
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	/* ~FEditorUndoClient Interface */

	/* FTickableEditorObject Interface */
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	/* ~FTickableEditorObject Interface */

private:

	void BindCommands();

	void ExtendMenu();
	void ExtendToolbar();

public:

	UHeroContent *HeroContent;

	TSharedPtr<FExtender> ToolbarExtender;

	TSharedPtr<FExtender> MenuExtender;

};