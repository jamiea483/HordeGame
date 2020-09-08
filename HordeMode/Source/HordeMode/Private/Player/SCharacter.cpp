// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SCharacter.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PickUpComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../HordeMode.h"
#include "Components/SHealthComponent.h"
#include "Net/UnrealNetwork.h"

#include "GameFramework/SpringArmComponent.h"
#include "Weapon/SWeapon.h"
#include "Camera/CameraComponent.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);

	PickupComp  = CreateDefaultSubobject<UPickUpComponent>(TEXT("pickup Component"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;

	WeaponSocketName = "WeaponSocket";
	HolsterSocketName = "BigWeaponHolster";

	AimOffSetPitch = 0;

	bUsingMainWeapon = true;
	bIsDead = false;
	bInPlayArea = true;
}

void ASCharacter::ChangeWeapon()
{
	
	if (BackUpWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChangeWeapon Called."));
		if (!bUsingMainWeapon)
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HolsterSocketName);
			BackUpWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		}
		else
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName );
			BackUpWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HolsterSocketName);
		}

	
		UE_LOG(LogTemp, Warning, TEXT(" Main weapon is %s."), *CurrentWeapon->InteractableName.ToString());
	}
	bSwitchWeaponDoOnce = false;
}

ASWeapon* ASCharacter::SpawnWeapon(TSubclassOf<ASWeapon> Weapon, FName SocketName)
{
	FActorSpawnParameters WeaponSpawnParams;
	WeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASWeapon* WeaponHolder;

	WeaponHolder = GetWorld()->SpawnActor<ASWeapon>(Weapon, FVector::ZeroVector, FRotator::ZeroRotator, WeaponSpawnParams);

	if (WeaponHolder)
	{
		WeaponHolder->SetOwner(this);
		WeaponHolder->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

	}
	return WeaponHolder;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = Camera->FieldOfView;

	if (GetLocalRole() == ROLE_Authority)
	{
		//Spawn Default Weapon
		CurrentWeapon = SpawnWeapon(StartWeapon, WeaponSocketName);
		
		BackUpWeapon = SpawnWeapon(nextWeapon, HolsterSocketName);
	}
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

		if (GetLocalRole() == ROLE_Authority)
		{
			FRotator NewRot = (GetControlRotation() - GetActorRotation());
			NewRot.Normalize();
			AimOffSetPitch = NewRot.Pitch;
		}
}

void ASCharacter::BeginCrouch()
{
	if (bIsPaused)return;	
		Crouch();	
}

void ASCharacter::EndCrouch()
{
	if (bIsPaused)return;	
	UnCrouch();
}

void ASCharacter::BeginJumping()
{	
	if (bIsPaused)return;
	Jump();
}

void ASCharacter::BeginAim()
{	
	if (bIsPaused)return;
	AimDownSight = true;
}

void ASCharacter::EndAim()
{
	AimDownSight = false;
}

void ASCharacter::StartFire()
{
	if (bIsPaused)return;
	if (CurrentWeapon && !bWeaponReload && !bSwitchingWeapon)
	{
		bIsFiring = true;
		if(bUsingMainWeapon)
			CurrentWeapon->StartFire();
		else
			BackUpWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (bIsPaused)return;
	if (CurrentWeapon && !bWeaponReload)
	{
		bIsFiring = false;
		if(bUsingMainWeapon)
			CurrentWeapon->EndFire();
		else
			BackUpWeapon->EndFire();
	}
}

void ASCharacter::Action()
{	
	if (bIsPaused)return;
	if (!PickupComp->GetCanPickup())
	{
		StopFire();
		ReloadWeapon();
		
	}
	else
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerInteract();
		}
		PickupComp->OnInteract();
	}
}

void ASCharacter::ReloadWeapon()
{
	if (bUsingMainWeapon)
	{
		if (CurrentWeapon->GetCurrentMag() != CurrentWeapon->GetMagSize() && !bWeaponReload)
		{
			if (GetLocalRole() < ROLE_Authority)
			{
				ServerReloadAnimation();
			}			
			bWeaponReload = true;
		}
	}
	else
	{
		if (BackUpWeapon->GetCurrentMag() != BackUpWeapon->GetMagSize() && !bWeaponReload)
		{
			if (GetLocalRole() < ROLE_Authority)
			{
				ServerReloadAnimation();
			}
			bWeaponReload = true;
		}
	}
}

void ASCharacter::ServerReloadAnimation_Implementation()
{
	if (bUsingMainWeapon)
		CurrentWeapon->EndFire();
	else
		BackUpWeapon->EndFire();
	bWeaponReload = true;	
}

bool ASCharacter::ServerReloadAnimation_Validate()
{
	return true;
}

void ASCharacter::SwitchWeapon()
{
	if (!bSwitchingWeapon && BackUpWeapon)
	{
		bSwitchWeaponDoOnce = true;
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerSwitchWeapon();
		}

		StopFire();
		bWeaponReload = false;

		if (bUsingMainWeapon)
			bUsingMainWeapon = false;
		else
			bUsingMainWeapon = true;
		bSwitchingWeapon = true;


	}
}

void ASCharacter::ServerSwitchWeapon_Implementation()
{
	SwitchWeapon();
}

bool ASCharacter::ServerSwitchWeapon_Validate()
{
	return true;
}

///Interaction
void ASCharacter::ServerInteract_Implementation()
{
	if (PickupComp)
		PickupComp->OnInteract();
}

bool ASCharacter::ServerInteract_Validate()
{
	return true;
}

USpringArmComponent* ASCharacter::GetSpringArm()
{
	return SpringArm;
}

///HealthComponent

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Ouch."));
	if (Health <= 0.0f && !bIsDead)
	{
		//Died
		if (CurrentWeapon)
		{
			CurrentWeapon->EndFire();
		}
		bIsDead = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		DetachFromControllerPendingDestroy();

		CurrentWeapon->SetOwner(nullptr);
		CurrentWeapon = nullptr;


		SetLifeSpan(6.0f);	
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);

	DOREPLIFETIME(ASCharacter, BackUpWeapon);

	DOREPLIFETIME(ASCharacter, bIsDead);

	DOREPLIFETIME(ASCharacter, bWeaponReload);

	DOREPLIFETIME(ASCharacter, bSwitchingWeapon);

	DOREPLIFETIME(ASCharacter, bUsingMainWeapon);

	DOREPLIFETIME(ASCharacter, bIsFiring);

	DOREPLIFETIME(ASCharacter, AimOffSetPitch);
}