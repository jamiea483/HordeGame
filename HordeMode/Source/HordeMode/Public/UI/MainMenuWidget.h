// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UENUM(BlueprintType)
enum class EMenuState : uint8
{
	StartMenu,

	MainMenu, 

	Options,

	Audio,

	Gameplay,

	Graphics,

	Multiplayer,

	MultiplayerSettings,

	Lobbys
};

USTRUCT()
struct FServerStruct
{
	GENERATED_BODY()

		FString Name;

		uint32 CurrentPlayers;

		uint32 MaxPlayers;

		FString UserName;
};



class UWidgetSwitcher;
class UPanelWidget;
class UEditableTextBox;
class UAudioWidget;
class UGameplayWidget;
class UEnterPasswordWidget;

/**
 * 
 */
UCLASS()
class HORDEMODE_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Menu")
		int32 GetMainMenuIndex(EMenuState State);

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void SetMainMenuIndex(EMenuState State, float value);

	UFUNCTION(BlueprintCallable, Category = "Menu")
		float GetMainMenuXIndex(EMenuState State);

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void SetMainMenuXIndex(EMenuState State, float value);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UWidgetSwitcher* MenuWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UPanelWidget* ServerList;

	UFUNCTION()
		void SetMaxNumberPlayersIndex(int32 value);

	UFUNCTION(BlueprintCallable, Category = "Menu")
		int32 GetMaxNumberPlayersIndex();

	UFUNCTION()
		void SetPrivateSlotsIndex(int32 value);

	UFUNCTION(BlueprintCallable, Category = "Menu")
		int32 GetPrivateSlotsIndex();

	UPROPERTY( meta = (BindWidget))
		UEditableTextBox* ServerNameText;

	UPROPERTY(BlueprintReadWrite, Category = "Menu", meta = (BindWidget))
		UAudioWidget* WBP_Audio;

	UPROPERTY(BlueprintReadWrite, Category = "Menu", meta = (BindWidget))
		UGameplayWidget* WBP_Gameplay;

	UPROPERTY( meta = (BindWidget))
		UEditableTextBox* PasswordText;

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UEnterPasswordWidget> PasswordWidget;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
		TArray<class UTextBlock*> MultplayerSettingTextArray;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
		TArray<class UButton*> MainMenuButtonArray;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
		TArray<class UButton*> OptionButtonArray;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
		TArray<class UButton*> MultiplayerButtonArray;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
		TArray<class UTextBlock*> AudioTextArray;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
		TArray<class UTextBlock*> GameplayTextArray;

	/**Gets the Index cap
	@MenuYAxis
	*/
	UFUNCTION()
		int32 GetIndexCap(EMenuState State);

	/**Gets the Index cap
	@MenuXAxis
	*/
	UFUNCTION()
		int32 GetIndexCapXAxis(EMenuState State);

	/*Set the lobby list*/
	void SetServerList(TArray<FServerStruct> Servers);

	void RefreshList();

	UFUNCTION()
		void SetMenuState(EMenuState MenuState) { EMenu = MenuState; };

	UFUNCTION()
		EMenuState GetMenuState() { return EMenu; };

	UFUNCTION( Category = "Intro")
		bool GetIntroIsPlaying() { return bIntroIsPlaying; };

	int32 GetServerCount() { return counter; };

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
		void EMenuStateChanged(EMenuState State);

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Intro")
		bool bIntroIsPlaying;

	UPROPERTY(BlueprintReadWrite, Category = "MainMenu")
		EMenuState EMenu;

private:

	TSubclassOf<class UUserWidget> ServerRow;

	uint32 MenuIndex;

	uint32 OptionIndex;

	uint32 AudioIndex;

	uint32 GameplayIndex;

	uint32 GraphicIndex;

	uint32 MultplayerIndex;

	uint32 MultplayerSettingsIndex;

	uint32 LobbyIndex;

	uint32 MaxPlayerIndex;

	uint32 privateSlotIndex;

	uint32 counter;
};
