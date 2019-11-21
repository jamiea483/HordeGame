// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayWidget.h"
#include "HordeGameUserSettings.h"
#include "Components/Slider.h"

void UGameplayWidget::SetHorizaontal(float value)
{
	HorizontalValue += value;
	if (HorizontalValue > 1)
		HorizontalValue = 1;
	else if (HorizontalValue < 0)
		HorizontalValue = 0;

	HSS->SetValue(HorizontalValue);
}

void UGameplayWidget::SetVertical(float value)
{
	VerticalValue += value;
	if (VerticalValue > 1)
		VerticalValue = 1;
	else if (VerticalValue < 0)
		VerticalValue = 0;

		VSS->SetValue(VerticalValue);
}

void UGameplayWidget::SetGameplayWidget()
{
	UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
	if (userSettings)
	{
		HorizontalValue = userSettings->GetYawSesitivity();
		HSS->SetValue(HorizontalValue);
		VerticalValue = userSettings->GetPitchSesitivity();
		VSS->SetValue(VerticalValue);
	}
}

void UGameplayWidget::SaveGameplaySettings()
{
	UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
	if (userSettings)
	{
		userSettings->ApplyGameplaySettings( VerticalValue, HorizontalValue, true);
	}
}
