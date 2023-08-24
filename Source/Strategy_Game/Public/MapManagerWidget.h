// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Slider.h"
#include "Components/TextBlock.h"

#include "MapManagerWidget.generated.h"

UCLASS()
class STRATEGY_GAME_API UMapManagerWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected: 

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		USlider* TerrainLevelSlider;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		UTextBlock* TerrainLevelText;

public:

	UFUNCTION(BlueprintCallable)
		void UpdateText(const FText& NewText);

	UFUNCTION(BlueprintCallable)
		void OnSliderValueChanged(float NewValue);

	virtual void NativeConstruct() override;

};
