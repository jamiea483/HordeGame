// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PickUpComponent.h"

// Sets default values
AInteractBase::AInteractBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetSphereRadius(.0f);

	if (SphereComp)
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AInteractBase::OnBeginOverlap);
		SphereComp->OnComponentEndOverlap.AddDynamic(this, &AInteractBase::OnOverlapEnd);
	}

}

// Called when the game starts or when spawned
void AInteractBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractBase::IncreaseSizeOfPickupSphere()
{
	SphereComp->SetSphereRadius(100.0f);
}

void AInteractBase::DecreaseSizeOfPickupSphere()
{
	SphereComp->SetSphereRadius(.0f);
}

void AInteractBase::OnBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->ActorHasTag("Instigator") && GetOwner() == nullptr)
	{
		UPickUpComponent* PickupComponent = Cast<UPickUpComponent>(OtherActor->GetComponentByClass(UPickUpComponent::StaticClass()));
		if(PickupComponent)
		PickupComponent->RegisterWeapon(this);
	}
}

void AInteractBase::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Instigator"))
	{
		UPickUpComponent* PickupComponent = Cast<UPickUpComponent>(OtherActor->GetComponentByClass(UPickUpComponent::StaticClass()));
		if (PickupComponent)
			PickupComponent->UnregisterWeapon(this);
	}
}