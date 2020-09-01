// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazards/PlayArea.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
APlayArea::APlayArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComp->SetupAttachment(RootComponent);
}


