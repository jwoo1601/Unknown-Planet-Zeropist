// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#include "ZPlayerHUD.h"

AZPlayerHUD::AZPlayerHUD(FObjectInitializer const& ObjectInitializer)
{

}

void AZPlayerHUD::SetCharacterStateVisibility(bool bVisible)
{
	if (CharacterStateWidget)
	{
		CharacterStateWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void AZPlayerHUD::SetCrosshairVisibility(bool bVisible)
{
	if (CrosshairWidget)
	{
		CrosshairWidget->SetVisibility(bVisible? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void AZPlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	Canvas->Deproject(FVector2D(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f), CrosshairWorldLocation, CrosshairWorldDirection);
}

void AZPlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerOwner)
	{
		if (CharacterStateWidgetClass && !CharacterStateWidget)
		{
			CharacterStateWidget = UUserWidget::CreateWidgetOfClass(CharacterStateWidgetClass, GetGameInstance(), GetWorld(), PlayerOwner);
			if (CharacterStateWidget)
			{
				CharacterStateWidget->AddToViewport();
			}
		}

		if (CrosshairWidgetClass && !CrosshairWidget)
		{
			CrosshairWidget = UUserWidget::CreateWidgetOfClass(CrosshairWidgetClass, GetGameInstance(), GetWorld(), PlayerOwner);
			if (CrosshairWidget)
			{
				CrosshairWidget->AddToViewport();
				CrosshairWidget->SetAnchorsInViewport(FAnchors(0.5f));
				CrosshairWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
				CrosshairWidget->SetPositionInViewport(FVector2D::ZeroVector);
			}
		}
	}
}
