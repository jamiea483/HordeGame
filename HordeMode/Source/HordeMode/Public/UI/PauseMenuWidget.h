// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORDEMODE_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "PauseMenu")
		void SetPauseIndex(int32 value) { PauseMenuIndex += value; };

	UFUNCTION(BlueprintCallable, Category = "PauseMenu")
		int32 GetPauseIndex() { return PauseMenuIndex; };

		UPROPERTY(BlueprintReadWrite, Category = "PauseMenu")
			TArray<class UButton*> PauseMenuButtons;
private:
		int32 PauseMenuIndex;

		UFUNCTION(BlueprintCallable, Category = "Control")
		void Controls();
};
