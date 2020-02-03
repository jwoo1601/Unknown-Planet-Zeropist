// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UserWidget.h"

#include "ZPlayerHUD.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CLIENT_API AZPlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	AZPlayerHUD(FObjectInitializer const& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetCharacterStateVisibility(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void SetCrosshairVisibility(bool bVisible);

public:

	/* AHUD Interface */
	virtual void DrawHUD() override;
	/* ~AHUD Interface */

	/* AActor Interface */
	virtual void BeginPlay() override;
	/* ~AActor Interface */

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> CharacterStateWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

private:

	UPROPERTY(BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	UUserWidget *CharacterStateWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	UUserWidget *CrosshairWidget;

	FVector CrosshairWorldLocation;
	FVector CrosshairWorldDirection;

public:

	FORCEINLINE UUserWidget* GetCharacterStateWidget() const { return CharacterStateWidget; }
	FORCEINLINE UUserWidget* GetCrosshairWidget() const { return CrosshairWidget; }
	FORCEINLINE FVector GetCrosshairWorldLocation() const { return CrosshairWorldLocation; }
	FORCEINLINE FVector GetCrosshairWorldDirection() const { return CrosshairWorldDirection; }
	
};
