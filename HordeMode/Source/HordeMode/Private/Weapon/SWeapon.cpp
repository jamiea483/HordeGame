// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "HordeMode.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "HordeGameUserSettings.h"

#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlayArea.h"
#include "SCharacter.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef WeaponDebug(TEXT("COOP.DebugWeapons"), 
	DebugWeaponDrawing, 
	TEXT("Draw Debug lines for Weapons"), 
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;
	
	BaseDamage = 20;
	RateOfFire = 600;

	MuzzleSocketName = "MuzzleSocket";

	SetReplicates(true);
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
	BulletSpread = 2.0f;

	Holster = EWeaponHolsterSize::BigWeapon;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;

	UE_LOG(LogTemp, Warning, TEXT("Weapon has %f left."), MaxAmmo);
}


bool ASWeapon::Reload()
{
	UE_LOG(LogTemp, Warning, TEXT("Mag size is %f."), MagSize);
	if (CurrentMag < MagSize)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerReload();
		}

		if (MaxAmmo > MagSize)
		{
			float NeededAmmo = MagSize - CurrentMag;
			MaxAmmo -= NeededAmmo;
			CurrentMag = MagSize;
			bClipIsEmpty = false;
			if (ReloadSound)
				PlaySFX(ReloadSound);
		}
		else 
		{
			CurrentMag = MaxAmmo;	
			MaxAmmo = 0.0f;
			bClipIsEmpty = false;
			if (ReloadSound)
				PlaySFX(ReloadSound);
		}
		return true;
	}
	else
	{	
		UE_LOG(LogTemp, Warning, TEXT("Weapon doesn't need to reload."));
		return false;
	}

}



//Fires weapon
void ASWeapon::Fire()
{
	//if not server call serverfire on server and run this function
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	// Trace the world, from pawn eyes to crosshair location
	AActor* MyOwner = GetOwner();
	if (MyOwner && CurrentMag > 0.0f)
	{
			CurrentMag--;
	
			WeaponRecoil();
			if (FireSound)
				PlaySFX(FireSound);


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
		LastFireTime = GetWorld()->TimeSeconds;
	}
	else
	{
		if (EmptyClipSound)
		{
			PlaySFX(EmptyClipSound);
		}
		
	}
}

void ASWeapon::CreateLineTraceCollisionQuery(FCollisionQueryParams &QueryParams, AActor * MyOwner)
{
	//Sets the Query so it doesn't collide with the play area trigger and selfactor.
	for (TActorIterator<APlayArea> Itr(GetWorld()); Itr; ++Itr)
	{
		APlayArea* PlayZone = Cast<APlayArea>(*Itr);
		if (PlayZone)
		{
			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			QueryParams.AddIgnoredActor(PlayZone);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;
		}
	}
}

void ASWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector Impactpoint)
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

void ASWeapon::ApplyDamage(EPhysicalSurface SurfaceType, AActor * HitActor, FVector &ShotDirection, FHitResult &OutHit, AActor * MyOwner)
{
	float ActualDamage = BaseDamage;
	if (SurfaceType == SURFACE_FLESHVALNERABLE)
	{
		ActualDamage *= 3.0f;
	}
	UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, OutHit, MyOwner->GetInstigatorController(), MyOwner, DamageType);
}



void ASWeapon::StartFire()
{
	if (CurrentMag <= 0)
	{
		bClipIsEmpty = true;
	}
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	ASCharacter* Char = Cast<ASCharacter>(GetOwner());
	if(Char && Char->GetIsFiring())
	GetWorldTimerManager().SetTimer(TimeHandle_TimeBeteenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(TimeHandle_TimeBeteenShots);
	if (bClipIsEmpty)
	{
		Reload();
	}
}

//Gun Effecfs
void ASWeapon::PlayFireEffect(const FVector &TraceEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	if (SmokeBullutEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* SmokeComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeBullutEffect, MuzzleLocation);

		if (SmokeComp)
		{
			SmokeComp->SetVectorParameter(TraceTargetName, TraceEndPoint);

			DrawDebugSphere(GetWorld(), TraceEndPoint, 12.0f, 12, FColor::Blue, false, 5, 0, 1.0f);
		}
	}

	// Shake Owners Camera
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
	 APlayerController* MyController = Cast<APlayerController>(MyOwner->GetController());
	 if (MyController)
	 {
		 MyController->ClientPlayCameraShake(ShakingCam);
	 }
	}
}

void ASWeapon::PlaySFX(USoundCue* SoundToPlay)
{
	UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
	if (userSettings)
	{
		UGameplayStatics::SpawnSoundAttached(SoundToPlay, RootComponent, "", GetActorLocation(), EAttachLocation::SnapToTarget, false, (userSettings->GetSoundEffectVolume() / 100)*(userSettings->GetMasterSoundVolume() / 100) , 1.f, 0.0f, nullptr, nullptr, true);
	}
}

//play cosmetic FX from server every time Struct is updated.
void ASWeapon::OnRep_HitScanLineTrace()
{
	PlayFireEffect(HitScanTrace.TraceTo);

	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::WeaponRecoil()
{
	ASCharacter* MyOwner = Cast<ASCharacter>(GetOwner());
	if (MyOwner)
	{
		MyOwner->AddControllerPitchInput(WeaponLift);
	}
}

//Play Fire Function on server
void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::ServerReload_Implementation()
{
	Reload();
}

bool ASWeapon::ServerReload_Validate()
{
	return true;
}

void ASWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);

}