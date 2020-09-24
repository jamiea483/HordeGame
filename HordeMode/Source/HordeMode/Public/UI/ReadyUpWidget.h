// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Interface/ReadyUpInterface.h"
#include "ReadyUpWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateWidget);
/**
 * 
 */
UCLASS()
class HORDEMODE_API UReadyUpWidget : public UUserWidget, public IReadyUpInterface
{
	GENERATED_BODY()
	
public:
		UFUNCTION(Category = "Connection")
			virtual void UpdateWidget();
			
		UFUNCTION( Category = "UI")
			virtual void ToViewport();	

		UFUNCTION(Category = "UI")
			virtual void TearDownReadyWidget();

		UPROPERTY(BlueprintAssignable, Category = "Events")
			FOnUpdateWidget TimeToUpdateUI;
};