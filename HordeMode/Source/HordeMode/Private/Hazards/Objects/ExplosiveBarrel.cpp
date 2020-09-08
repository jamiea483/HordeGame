// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazards/Objects/ExplosiveBarrel.h"
#include "Components/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "../HordeMode.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	BlastRadius = CreateDefaultSubobject<URadialForceComponent>(TEXT("Radial Force Component"));
	BlastRadius->SetupAttachment(RootComponent);
	BlastRadius->Radius = 400;
	BlastRadius->bIgnoreOwningActor = true;
	BlastRadius->bImpulseVelChange = true;
	BlastRadius->bAutoActivate = false;

	

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health Component"));

	bHasExploded = false;
	
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComp->OnHealthChanged.AddDynamic(this, &AExplosiveBarrel::OnHealthChanged);

}



// Called every frame
void AExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AExplosiveBarrel::OnRep_isExploded()
{
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation(), false);
	}

	if (ExplodedMaterial)
	{
		MeshComp->SetMaterial(0, ExplodedMaterial);
	}
}

void AExplosiveBarrel::OnHealthChanged(USHealthComponent * HealthComponent, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	
	if (Health <= 0 && !bHasExploded)
	{
		bHasExploded = true;

		//charges the barrel force to a vector
		FVector BarreljumpForce = GetActorUpVector() * ExplosionForce;
		MeshComp->AddImpulse(BarreljumpForce, NAME_None, true);

		BlastRadius->FireImpulse();
		
		OnRep_isExploded();
	}

}

void AExplosiveBarrel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AExplosiveBarrel, bHasExploded);

}