// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class HORDEMODE_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

		UFUNCTION(BlueprintCallable, Category = "Ready State")
		bool GetReadyState() { return bIsReady; };

		UFUNCTION(BlueprintCallable, Category = "Ready State")
			void ChangePlayerReadyState();
private:

	UPROPERTY(ReplicatedUsing = OnRep_ReadyState, visibleanywhere)
		bool bIsReady;

protected:
	UFUNCTION()
		void OnRep_ReadyState();

	UFUNCTION()
		void UpdatePlayerWidget();

};
