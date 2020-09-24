// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ReadyUpWidget.h"


void UReadyUpWidget::UpdateWidget()
{
	TimeToUpdateUI.Broadcast();
}

void UReadyUpWidget::ToViewport()
{
	AddToViewport();
}

void UReadyUpWidget::TearDownReadyWidget()
{
	RemoveFromParent();
}