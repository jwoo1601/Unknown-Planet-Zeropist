// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "AttackAnimMontageDetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "Animation/AnimMontage.h"
#include "STextBlock.h"
#include "STextComboBox.h"

#include "GameTypes.h"
#include "AttackAnimMontage.h"

#define LOCTEXT_NAMESPACE "AttackAnimMontageDetails"

TSharedRef<IDetailCustomization> FAttackAnimMontageDetails::MakeInstance()
{
	return MakeShareable(new FAttackAnimMontageDetails);
}

void FAttackAnimMontageDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TSharedPtr<IPropertyHandle> AttackModeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UAttackAnimMontage, AttackMode));

	if (AttackModeProperty->IsValidHandle() && DetailBuilder.HasClassDefaultObject())
	{
		uint8 CurAttackMode;

		if (AttackModeProperty->GetValue(CurAttackMode) == FPropertyAccess::Success)
		{
			TSharedPtr<IPropertyHandle> CompositeSectionsProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UAttackAnimMontage, CompositeSections));

			if (CompositeSectionsProperty->IsValidHandle())
			{
				UpdateMontageSectionData(CompositeSectionsProperty);
			}

			IDetailCategoryBuilder &AttackCategory = DetailBuilder.EditCategory("Attack");

			switch ((EAttackMode)CurAttackMode)
			{

			case EAttackMode::Charged:
			{
				TSharedPtr<IPropertyHandle> AnimBurstSectionProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UAttackAnimMontage, BurstSectionName));
				BuildCustomizedAnimSections(AttackCategory, AnimBurstSectionProperty, LOCTEXT("AnimBurstSection", "Anim Burst Section"), LOCTEXT("AnimBurstSection", "Anim Burst Section"));
			}
			
			break;

			case EAttackMode::Auto:
			{
				TSharedPtr<IPropertyHandle> AnimEndSectionProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UAttackAnimMontage, EndSectionName));
				BuildCustomizedAnimSections(AttackCategory, AnimEndSectionProperty, LOCTEXT("AnimEndSection", "Anim End Section"), LOCTEXT("AnimEndSection", "Anim End Section"));
			}
			
			break;

			default:
				break;
			}
		}
		
	}
}

void FAttackAnimMontageDetails::BuildCustomizedAnimSections(IDetailCategoryBuilder &InCategoryBuilder, TSharedPtr<IPropertyHandle> AnimSectionNameProp, FText FilterString, FText NameSlotText, bool bAdvancedMenu/*=false*/)
{
	if (AnimSectionNameProp.IsValid())
	{
		InCategoryBuilder.AddCustomRow(FilterString, bAdvancedMenu)
			.NameContent()
			[
				SNew(STextBlock)
				.Text(NameSlotText)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
			.ValueContent()
			[
				SNew(STextComboBox)
				.OptionsSource(&MontageSectionNames)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.OnSelectionChanged_Lambda([&AnimSectionNameProp] (TSharedPtr<FString> SelectedSectionName, ESelectInfo::Type SelectType)
				{
					if (SelectedSectionName.IsValid())
					{
						AnimSectionNameProp->SetValue(FName(**SelectedSectionName));
					}
				})
			];
		//.Visibility();
	}
}

void FAttackAnimMontageDetails::UpdateMontageSectionData(TSharedPtr<IPropertyHandle> CompositeSectionsProperty)
{
	TSharedPtr<IPropertyHandleArray> SectionsArrayProperty = CompositeSectionsProperty->AsArray();

	if (SectionsArrayProperty.IsValid())
	{
		uint32 NumSections;

		if (SectionsArrayProperty->GetNumElements(NumSections) == FPropertyAccess::Success)
		{
			uint32 NumToReserve = NumSections - MontageSectionNames.Num();
			MontageSectionNames.Reserve(NumToReserve);

			for (uint32 i = 0; i < NumSections; i++)
			{
				TSharedPtr<IPropertyHandle> SectionElementProperty = SectionsArrayProperty->GetElement(i);

				if (SectionElementProperty.IsValid())
				{
					uint32 NumChildren;

					if (SectionElementProperty->GetNumChildren(NumChildren) == FPropertyAccess::Success)
					{
						for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ChildIndex++)
						{
							TSharedPtr<IPropertyHandle> ChildHandle = SectionElementProperty->GetChildHandle(TEXT("SectionName"), false);

							if (ChildHandle.IsValid())
							{
								FName SectionNameValue;

								if (ChildHandle->GetValue(SectionNameValue) == FPropertyAccess::Success)
								{
									MontageSectionNames[i] = MakeShareable<FString>(new FString(SectionNameValue.ToString()));
								}
							}
						}
					}
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE