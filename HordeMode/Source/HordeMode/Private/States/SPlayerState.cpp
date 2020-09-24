// Fill out your copyright notice in the Description page of Project Settings.


#include "States/SPlayerState.h"
#include "Net/UnrealNetwork.h"



void ASPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Kills);
}

