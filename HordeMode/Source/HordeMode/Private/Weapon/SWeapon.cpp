// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SWeapon.h"

#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "HordeMode.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "HordeGameUserSettings.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PlayArea.h"
#include "SCharacter.h"




// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetSphereRadius(.0f);

	if (SphereComp)
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASWeapon::OnBeginOverlap);
		SphereComp->OnComponentEndOverlap.AddDynamic(this, &ASWeapon::OnOverlapEnd);
	}

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
		ASCharacter* Char = Cast<ASCharacter>(GetOwner());
		if (Char)
			Char->SetWeaponReloading(true);
			Reload();
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

void ASWeapon::IncreaseSizeOfPickupSphere()
{
	SphereComp->SetSphereRadius(100.0f);
}

void ASWeapon::DecreaseSizeOfPickupSphere()
{
	SphereComp->SetSphereRadius(.0f);
}

//Gun Effecfs
void ASWeapon::PlayFireEffect()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
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

void ASWeapon::WeaponRecoil()
{
	ASCharacter* MyOwner = Cast<ASCharacter>(GetOwner());
	if (MyOwner)
	{
		MyOwner->AddControllerPitchInput(WeaponLift);
	}
}

void ASWeapon::OnBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->ActorHasTag("Player") && GetOwner() == nullptr)
	{
		ASCharacter* Char = Cast<ASCharacter>(OtherActor);
		if (Char)
		{
			Char->WeaponList.AddUnique(this);
		}
	}
}

void ASWeapon::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		ASCharacter* Char = Cast<ASCharacter>(OtherActor);
		if (Char)
		{
			Char->WeaponList.Remove(this);
		}
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

