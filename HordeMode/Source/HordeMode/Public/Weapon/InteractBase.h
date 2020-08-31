// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractBase.generated.h"

class USphereComponent;

UCLASS()
class HORDEMODE_API AInteractBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USkeletalMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USphereComponent* SphereComp;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		USphereComponent* GetSphereComponent() { return SphereComp; };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName InteractableName;
private:
	//Collision
	UFUNCTION()
		virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
