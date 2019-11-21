// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuController.generated.h"

class UMainMenuWidget;
class UHordeGameInstance;
/**
 * 
 */
UCLASS()
class HORDEMODE_API AMenuController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Allows the PlayerController to set up custom input bindings. */
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	UMainMenuWidget* MenuWidget;

	UHordeGameInstance* GI;
private:
	

	void MenuYAxis(float value);
	void MenuXAxis(float value);

	//Sets Who is player one at start screen  
	void Start();

	void Confirm();
	void Back();

	//So the player has to tap the direction every time
	bool bDoOnce;

	//So the player has to tap the direction every time
	bool bDoOnceX;

};
	