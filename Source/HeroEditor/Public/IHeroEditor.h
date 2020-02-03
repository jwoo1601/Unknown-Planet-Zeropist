// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class IHeroEditor : public FWorkflowCentricApplication
{
public:
	
	/* Sets the hero content of this editor */
	virtual void SetHeroContent(class UHeroContent *HeroContent) = 0;

};