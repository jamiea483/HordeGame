// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BasicAI.generated.h"

class USHealthComponent;
class USphereComponent;
class UParticleSystem;
class USoundCue;

UCLASS()
class HORDEMODE_API ABasicAI : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasicAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		USphereComponent* SphereComp;

	FVector GetNextPathPoint();

	//Next point in Navigation path
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrakerBot")
	float MovementForce;

	float ElapseTime;

	UPROPERTY(EditDefaultsOnly, Category = "TrakerBot")
	bool bUseVelChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrakerBot")
	float RequiredDistanceToTarget;

	UFUNCTION()
		void OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	//Material to pulse on damage taken
	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		UParticleSystem* ExplosionEffect;

	bool bExploded;
	 
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	float BlastRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	float SelfDamageInterval;

	FTimerHandle TimeHandle_SelfDamage;

	void DamageSelf();

	bool bStartedSelfDestruction;

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		USoundCue* ExplosionSound;

	FTimerHandle TimerHandle_PathTimer;

	void RefreshPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BotMovement(FVector &ForceDirection, float DeltaTime);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
