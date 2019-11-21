// Fill out your copyright notice in the Description page of Project Settings.


#include "EnterPasswordWidget.h"
#include "Components/EditableTextBox.h"
#include "HordeGameInstance.h"



bool UEnterPasswordWidget::Initialize()
{
	Super::Initialize();

	EnteredPassword->OnTextCommitted.AddDynamic(this, &UEnterPasswordWidget::CommittedPassword);

		return true;
}

void UEnterPasswordWidget::CommittedPassword(const FText & InText, ETextCommit::Type InCommitType)
{
		if (EnteredPassword->GetText().ToString() == Pass)
		{
			UHordeGameInstance* GI = GetGameInstance<UHordeGameInstance>();
			if (GI)
			{
				GI->JoinFunction(index);
			}
		}
		RemoveFromParent();
}
