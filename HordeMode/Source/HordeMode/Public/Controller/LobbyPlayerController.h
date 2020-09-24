// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

class IReadyUpInterface;
/**
 * 
 */
UCLASS()
class HORDEMODE_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	ALobbyPlayerController(const FObjectInitializer &ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "PauseMenu")
		TSubclassOf<UUserWidget> PauseMenuWidget;

	class UPauseMenuWidget* PauseMenu;

	UPROPERTY(EditDefaultsOnly, Category = "PauseMenu")
		TSubclassOf<UUserWidget> ReadyUpWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Ready UI")
	void CreateReadyUpUI();

	UFUNCTION(Server, Reliable)
		void ServerChangeReadyState();

public:
	//Gets bIsPause and onlinestate and if not Pause creates pause widget and and sets it to true
//else removes widget.
//OnlineState Locks or releases controls.
	UFUNCTION()
		void PauseGame();

	UFUNCTION()
		void UnPauseGame();

	UFUNCTION()
		void ChangeReadyState();

		IReadyUpInterface* ReadyUpWidget;
		
		UFUNCTION(Client, Reliable)
			void ClientTearDownReadyWidget();

private:
	void TearDownReadyWidget();

	void LoadPauseMenu();

	void TearDown();
};
