// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

class UPauseMenuWidget;
/**
 * 
 */
UCLASS()
class HORDEMODE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//Gets bIsPause and onlinestate and if not Pause creates pause widget and and sets it to true
//else removes widget.
//OnlineState Locks or releases controls.
	UFUNCTION()
		void PauseGame();

	UFUNCTION()
		void UnPauseGame();

protected:

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "PauseMenu")
		TSubclassOf<UUserWidget> PauseMenuWidget;

	UPauseMenuWidget* PauseMenu;

private:

	void LoadPauseMenu();

	void TearDown();

};
