// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayArea.generated.h"

class UBoxComponent;
class USceneComponent;

UCLASS()
class HORDEMODE_API APlayArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayArea();

protected:

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		USceneComponent* RootComp;


	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		UBoxComponent* BoxComp;


};
