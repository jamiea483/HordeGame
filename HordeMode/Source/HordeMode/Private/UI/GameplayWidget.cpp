// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameplayWidget.h"
#include "Settings/HordeGameUserSettings.h"
#include "Components/Slider.h"

void UGameplayWidget::SetHorizaontal(float value)
{
	HorizontalValue += value/2;
	if (HorizontalValue > 1)
		HorizontalValue = 1;
	else if (HorizontalValue < .1)
		HorizontalValue = .1;

	HSS->SetValue(HorizontalValue);
}

float UGameplayWidget::GetHorizaontal()
{
	return HorizontalValue;
}

void UGameplayWidget::SetVertical(float value)
{
	VerticalValue += value/2;
	if (VerticalValue > 1)
		VerticalValue = 1;
	else if (VerticalValue < 0.1)
		VerticalValue = 0.1;

		VSS->SetValue(VerticalValue);
}

float UGameplayWidget::GetVertical()
{
	return VerticalValue;
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
