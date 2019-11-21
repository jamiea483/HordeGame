// Fill out your copyright notice in the Description page of Project Settings.


#include "HordeGameUserSettings.h"

UHordeGameUserSettings::UHordeGameUserSettings()
{
}

void UHordeGameUserSettings::ApplySettings(bool bCheckForCommanLineOverrides)
{
	Super::ApplySettings(bCheckForCommanLineOverrides);
}

void UHordeGameUserSettings::ApplySoundSettings(float MV, float MUV, float SFXV, bool bCheckForCommanLineOverrides)
{
	SetMasterSoundVolume(MV);
	SetMusicVolume(MUV);
	SetSoundEffectVolume(SFXV);

	ApplySettings(bCheckForCommanLineOverrides);
}

void UHordeGameUserSettings::ApplyGameplaySettings(float Pitch, float Yaw, bool bCheckForCommanLineOverrides)
{
	SetPitchSesitivity(Pitch);
	SetYawSesitivity(Yaw);

	ApplySettings(bCheckForCommanLineOverrides);
}

float UHordeGameUserSettings::GetMasterSoundVolume()
{
	return MasterSoundVolume;
}

void UHordeGameUserSettings::SetMasterSoundVolume(float Volume)
{
	MasterSoundVolume = Volume;
}

float UHordeGameUserSettings::GetSoundEffectVolume()
{
	return SoundEffectVolume;
}

void UHordeGameUserSettings::SetSoundEffectVolume(float Volume)
{
	SoundEffectVolume = Volume;
}

float UHordeGameUserSettings::GetMusicVolume()
{
	return MusicVolume;
}

void UHordeGameUserSettings::SetMusicVolume(float Volume)
{
	MusicVolume = Volume;
}

float UHordeGameUserSettings::GetPitchSesitivity()
{
	return PitchSensitivity;
}

void UHordeGameUserSettings::SetPitchSesitivity(float value)
{
	PitchSensitivity = value;
}

float UHordeGameUserSettings::GetYawSesitivity()
{
	return YawSensitivity;
}

void UHordeGameUserSettings::SetYawSesitivity(float value)
{
	YawSensitivity = value;
}


