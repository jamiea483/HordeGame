// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "UnrealNetwork.h"

void ASPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Kills);

}

