// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"

AProjectileSWeapon::AProjectileSWeapon()
{

}

void AProjectileSWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (Grenades)
		{
			GetWorld()->SpawnActor<AActor>(Grenades, MuzzleLocation, EyeRotation, SpawnParams);
			UE_LOG(LogTemp, Warning, TEXT("Spawned Grenade"));
		}
	}
	
	
}
