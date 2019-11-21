// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"
#include "HordeGameInstance.generated.h"

class AMenuController;
class UMainMenuWidget;
class FOnlineSessionSearch;
enum class EMenuState : uint8;

USTRUCT()
struct FPlayerServerSettings
{
	GENERATED_BODY()

		FString ServerName;
		
	int32 PublicSlots;
	
	int32 PrivateSlots;

	FString SetPassword;
};
/**
 * 
 */
UCLASS()
class HORDEMODE_API UHordeGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
		UHordeGameInstance();

		virtual void Init() override;

		//the player thathas control over the menu base on who press start
		//at the start screen.
		UPROPERTY(BlueprintReadOnly, Category = "Player")
		TArray<int32> Player1;

		UPROPERTY(BlueprintReadWrite, Category = "Player")
		UMainMenuWidget* Menuclass;

		void SetActiveWidget(EMenuState Screen, int32 WidgetNumber);

		UFUNCTION(BlueprintCallable, Category = "Online")
			void Host(const FString ServerName, const FString password, int32 MaxPlayers, int32 PrivateSlots);

		void DestroyExistingSession();

		/**used for destroying function to know if the player is creating a session or leaving a session.
		@CreateSession function.
		@ OnDestroySessionComplete function.
		*/
		bool LeavingSession;

		UFUNCTION(BlueprintCallable, Category = "Online")
			void FindGameSessions();

		void CreateSession();

		UFUNCTION(BlueprintCallable, Category = "Online")
			void Join(int32 Index);

		UFUNCTION(BlueprintCallable, Category = "Online")
			void JoinFunction(const int32 &Index);

		//Allows the game to pause if it is a local game.
		UFUNCTION(BlueprintCallable, Category = "Online")
			bool GetOnlineState() { return bIsOnline; };

		UFUNCTION(BlueprintCallable, Category = "Online")
			void SetOnlineState(bool OnlineState) { bIsOnline = OnlineState; };

		FPlayerServerSettings GetServerSettings() { return ServerSettings; };

		void StartSession();
		
protected:

	bool bIsOnline;

	IOnlineSessionPtr SessionInterface;

	void OnCreateSessionComplete(FName Session, bool Success);

	void OnDestroySessionComplete(FName Session, bool Success);

	void OnFindSessionsComplete(bool Success);

	void OnSessionJoinComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	FPlayerServerSettings ServerSettings;
};
