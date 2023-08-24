// Fill out your copyright notice in the Description page of Project Settings.


#include "MapManagerWidget.h"

void UMapManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TerrainLevelSlider)
	{
		TerrainLevelSlider->SetMinValue(0.0);
		TerrainLevelSlider->SetMaxValue(5.0);
		TerrainLevelSlider->SetStepSize(1.0);

		TerrainLevelSlider->SetValue(3.0);
		TerrainLevelSlider->Orientation = EOrientation::Orient_Vertical;

		TerrainLevelSlider->OnValueChanged.AddDynamic(this, &UMapManagerWidget::OnSliderValueChanged);
	}

	if (TerrainLevelText && TerrainLevelSlider)
	{
		TerrainLevelText->SetText(FText::AsNumber(TerrainLevelSlider->GetValue()));
	}
}

void UMapManagerWidget::OnSliderValueChanged(float NewValue)
{
	UpdateText(FText::AsNumber(NewValue));
}

void UMapManagerWidget::UpdateText(const FText& NewText)
{
	if (TerrainLevelText)
	{
		TerrainLevelText->SetText(NewText);
	}
}