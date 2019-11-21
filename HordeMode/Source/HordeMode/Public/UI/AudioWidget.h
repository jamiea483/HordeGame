// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AudioWidget.generated.h"

class USlider;
class AMenuController;
/**
 * 
 */
UCLASS()
class HORDEMODE_API UAudioWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Audio", meta = (BindWidget))
		USlider* MVS;

	UPROPERTY(BlueprintReadWrite, Category = "Audio", meta = (BindWidget))
		USlider* MSSV;

	UPROPERTY(BlueprintReadWrite, Category = "Audio", meta = (BindWidget))
		USlider* SFXVS;

public:

	UFUNCTION(BlueprintCallable, Category = "Audio")
		void SetAudioForWidget();

	UFUNCTION(BlueprintCallable, Category = "Audio")
		void SetMVSValue(float value);

	UFUNCTION(BlueprintCallable, Category = "Audio")
		void SetMSSVValue(float value);

	UFUNCTION(BlueprintCallable, Category = "Audio")
		void SetSFXVSValue(float value);

	void SaveAudioSettings();



private:
	float MasterVolume;

	float SoundEffect;

	float MusicVolume;
};
