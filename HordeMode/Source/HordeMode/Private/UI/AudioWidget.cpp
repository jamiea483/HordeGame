// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AudioWidget.h"
#include "Settings/HordeGameUserSettings.h"
#include "Components/Slider.h"

void UAudioWidget::SetAudioForWidget()
{
		UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
		if (userSettings)
		{
			MasterVolume = userSettings->GetMasterSoundVolume();
			MVS->SetValue(MasterVolume / 100);
			MusicVolume = userSettings->GetMusicVolume();
			MSSV->SetValue(MusicVolume / 100);
			SoundEffect = userSettings->GetSoundEffectVolume();
			SFXVS->SetValue(SoundEffect / 100);
			
			UE_LOG(LogTemp, Warning, TEXT("UserSettings was found."));
		}
}


void UAudioWidget::SaveAudioSettings()
{
	UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
	if (userSettings)
	{
		userSettings->ApplySoundSettings(MasterVolume, MusicVolume, SoundEffect, true);
	}
}
void UAudioWidget::SetMVSValue(float value)
{
	MasterVolume += value;
	if (MasterVolume > 100)
		MasterVolume = 100;
	else if (MasterVolume < 0)
		MasterVolume = 0;
	
	MVS->SetValue(MasterVolume/100);
}

int32 UAudioWidget::GetMVSValue()
{
	return MasterVolume;
}

void UAudioWidget::SetMSSVValue(float value)
{
	MusicVolume += value;
	if (MusicVolume > 100)
		MusicVolume = 100;
	else if (MusicVolume < 0)
		MusicVolume = 0;

	MSSV->SetValue(MusicVolume / 100);
}

int32 UAudioWidget::GetMSSVValue()
{
	return MusicVolume;
}

void UAudioWidget::SetSFXVSValue(float value)
{
	SoundEffect += value;
	if (SoundEffect > 100)
		SoundEffect = 100;
	else if (SoundEffect < 0)
		SoundEffect = 0;

	SFXVS->SetValue(SoundEffect / 100);
}

int32 UAudioWidget::GetSFXVSValue()
{
	return SoundEffect;
}
