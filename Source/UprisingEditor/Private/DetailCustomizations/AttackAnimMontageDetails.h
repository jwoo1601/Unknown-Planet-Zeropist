// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class FAttackAnimMontageDetails : public IDetailCustomization
{

public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder &DetailBuilder) override;

private:

	void BuildCustomizedAnimSections(IDetailCategoryBuilder &InCategoryBuilder, TSharedPtr<IPropertyHandle> AnimSectionNameProp, FText FilterString, FText NameSlotText, bool bAdvancedMenu = false);

	void UpdateMontageSectionData(TSharedPtr<IPropertyHandle> CompositeSectionsProperty);

	TArray<TSharedPtr<FString>> MontageSectionNames;

};