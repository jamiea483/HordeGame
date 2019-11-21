// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class HORDEMODE_API UServerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget))
		UTextBlock* ServerNameText;

	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget))
		UTextBlock* PlayerNumText;

	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget))
		UTextBlock* HostNameText;
};
