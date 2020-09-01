// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileSWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "UnrealNetwork.h"

AProjectileSWeapon::AProjectileSWeapon()
{

}

void AProjectileSWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if(MyOwner && CurrentMag > 0.0f)
	{
		Super::Fire();

		WeaponRecoil();
		if (FireSound)
			PlaySFX(FireSound);

		SpawnProjectile(MyOwner, Grenades);
		
		if (GetLocalRole() == ROLE_Authority)
		{
			projectile.MyOwner = MyOwner;
			projectile.Projectile = Grenades;
			projectile.ReplicationCount++;
		}
	}
}

void AProjectileSWeapon::SpawnProjectile(AActor * MyOwner, TSubclassOf<AActor> Projectile)
{
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (Projectile)
	{
		GetWorld()->SpawnActor<AActor>(Projectile, MuzzleLocation, EyeRotation, SpawnParams);
		UE_LOG(LogTemp, Warning, TEXT("Spawned Grenade"));
	}
}

void AProjectileSWeapon::OnRep_SpawnProjectile()
{
	SpawnProjectile(projectile.MyOwner, projectile.Projectile);
}

void AProjectileSWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AProjectileSWeapon, projectile, COND_SkipOwner);
}