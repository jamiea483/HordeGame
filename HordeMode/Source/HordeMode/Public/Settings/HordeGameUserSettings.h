// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "HordeGameUserSettings.generated.h"

/**
 * 
 */
UCLASS(config = GameUserSettings)
class HORDEMODE_API UHordeGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:

	UHordeGameUserSettings();

	virtual void ApplySettings(bool bCheckForCommanLineOverrides) override;

	void ApplySoundSettings(float MV, float MUV, float SFXV, bool bCheckForCommanLineOverrides);

	void ApplyGameplaySettings(float Pitch, float Yaw, bool bCheckForCommanLineOverrides);

	float GetMasterSoundVolume();
	void SetMasterSoundVolume(float Volume);

	float GetSoundEffectVolume();
	void SetSoundEffectVolume(float Volume);

	float GetMusicVolume();
	void SetMusicVolume(float value);

	float GetPitchSesitivity();
	void SetPitchSesitivity(float value);

	float GetYawSesitivity();
	void SetYawSesitivity(float value);

protected:

	UPROPERTY(Config)
		float MasterSoundVolume;

	UPROPERTY(Config)
		float SoundEffectVolume;

	UPROPERTY(Config)
		float MusicVolume;

	UPROPERTY(Config)
		float PitchSensitivity;

	UPROPERTY(Config)
		float YawSensitivity;

};
