// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "SPowerupActor.h"
#include "SCharacter.h"
#include "TimerManager.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal Component"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComp->SetupAttachment(RootComponent);

	CoolDownDuration = 10;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority)
	{
		Respawn();
	}
	
}

void ASPickupActor::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp,Warning, TEXT("Powerup class is null in %s. please add a power."), *GetName());
		return;
	}

	FActorSpawnParameters Spawnparams;
	Spawnparams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerUpInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerUpClass, GetActorTransform(), Spawnparams);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (GetLocalRole() == ROLE_Authority && PowerUpInstance)
	{
			ASCharacter* MyPawn = Cast<ASCharacter>(OtherActor);
			if (MyPawn)
			{
				PowerUpInstance->ActivatePowerUp(OtherActor);
				PowerUpInstance = nullptr;

				GetWorldTimerManager().SetTimer(TimerHandle_SpawnPowerUp, this, &ASPickupActor::Respawn, CoolDownDuration);
			}
	}

}


