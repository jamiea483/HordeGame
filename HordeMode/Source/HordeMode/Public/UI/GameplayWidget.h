// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"

class USlider;
/**
 * 
 */
UCLASS()
class HORDEMODE_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Audio", meta = (BindWidget))
		USlider* VSS;

	UPROPERTY(BlueprintReadWrite, Category = "Audio", meta = (BindWidget))
		USlider* HSS;

public:

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void SetHorizaontal(float value);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		float GetHorizaontal();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void SetVertical(float value);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		float GetVertical();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void SetGameplayWidget();

	void SaveGameplaySettings();

private:
	/**min is 0 and max is 1*/
	float HorizontalValue;

	/**min is 0 and max is 1*/
	float VerticalValue;

};
