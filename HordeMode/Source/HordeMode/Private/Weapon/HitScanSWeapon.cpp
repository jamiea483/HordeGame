// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/HitScanSWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "../../HordeMode.h"

#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef WeaponDebug(TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug lines for Weapons"),
	ECVF_Cheat);

// Sets default values
AHitScanSWeapon::AHitScanSWeapon()
{
	TraceTargetName = "Target";
}

void AHitScanSWeapon::Fire()
{
	Super::Fire();

	AActor* MyOwner = GetOwner();
	if (MyOwner && CurrentMag > 0.0f)
	{

		WeaponRecoil();

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Bullet spread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FCollisionQueryParams QueryParams;
		CreateLineTraceCollisionQuery(QueryParams, MyOwner);

		FVector TraceEnd = EyeLocation + (ShotDirection * 100000);

		//Particle "Target" parameter
		FVector TraceEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult OutHit;
		if (GetWorld()->LineTraceSingleByChannel(OutHit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			//Blocking Hit! process Damage

			AActor* HitActor = OutHit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(OutHit.PhysMaterial.Get());

			//if(SurfaceType)

			ApplyDamage(SurfaceType, HitActor, ShotDirection, OutHit, MyOwner);

			PlayImpactEffect(SurfaceType, OutHit.ImpactPoint);

			//Overrides Tranceendpoint if it hits something.
			TraceEndPoint = OutHit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0.0f, 1.0f);
		}

		PlayFireEffect(TraceEndPoint);

		//sends server trace end point to clients so they can play effects every time struct is updated.
		if (GetLocalRole() == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TraceEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
			HitScanTrace.ReplicationCount++;
		}
	}
}

void AHitScanSWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector Impactpoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVALNERABLE: SelectedEffect = FleshImpactEffect;
		break;

	default:SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = Impactpoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Impactpoint, ShotDirection.Rotation());
	}
}

void AHitScanSWeapon::ApplyDamage(EPhysicalSurface SurfaceType, AActor * HitActor, FVector &ShotDirection, FHitResult &OutHit, AActor * MyOwner)
{
	float ActualDamage = BaseDamage;
	if (SurfaceType == SURFACE_FLESHVALNERABLE)
	{
		ActualDamage *= 3.0f;
	}
	UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, OutHit, MyOwner->GetInstigatorController(), MyOwner, DamageType);
}

//play cosmetic FX from server every time Struct is updated.
void AHitScanSWeapon::OnRep_HitScanLineTrace()
{
	PlayFireEffect(HitScanTrace.TraceTo);

	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

//Gun Effecfs
void AHitScanSWeapon::PlayFireEffect(const FVector &TraceEndPoint)
{
	Super::PlayFireEffect();
	if (SmokeBullutEffect)
	{
		if (FireSound)
			PlaySFX(FireSound);
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* SmokeComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeBullutEffect, MuzzleLocation);

		if (SmokeComp)
		{
			SmokeComp->SetVectorParameter(TraceTargetName, TraceEndPoint);

			DrawDebugSphere(GetWorld(), TraceEndPoint, 12.0f, 12, FColor::Blue, false, 5, 0, 1.0f);
		}
	}
}

void AHitScanSWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AHitScanSWeapon, HitScanTrace, COND_SkipOwner);
}