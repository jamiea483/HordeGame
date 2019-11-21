// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnterPasswordWidget.generated.h"


class UEditableTextBox;
/**
 * 
 */
UCLASS()
class HORDEMODE_API UEnterPasswordWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* EnteredPassword;

	UPROPERTY(BlueprintReadOnly, Category = "Password")
	FString Pass;

	UPROPERTY(BlueprintReadOnly, Category = "Password")
	int32 index;

protected:
	virtual bool Initialize() override;

	void CommittedPassword(const FText& InText, ETextCommit::Type InCommitType);
};
