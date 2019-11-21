// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class HORDEMODE_API ALobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	

public:
	ALobbyGameModeBase();

	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	void PostLogin(APlayerController* NewPlayer) override;

	void StartGame();

	/** Called when a Controller with a PlayerState leaves the game or is destroyed */
	void Logout(AController* Exiting) override;

private:
	uint32 NumPlayers = 0;

	FTimerHandle StartGame_TimerHandle;
};
