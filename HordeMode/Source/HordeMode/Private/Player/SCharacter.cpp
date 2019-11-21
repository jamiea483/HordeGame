// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "HordeMode.h"
#include "SHealthComponent.h"
#include "UnrealNetwork.h"


#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SWeapon.h"
#include "SPlayerController.h"
#include "HordeGameUserSettings.h"




// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;

	WeaponSocketName = "WeaponSocket";

	AimOffSetPitch = 0;

	bIsDead = false;
	bInPlayArea = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = Camera->FieldOfView;

	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters WeaponSpawnParams;
		WeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//Spawn Default Weapon
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StartWeapon, FVector::ZeroVector, FRotator::ZeroRotator, WeaponSpawnParams);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		}
	}
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);	
}

/**Creates a line trace from the center of the camera to 10000 units away
and creates a query to ignore it owning actor and the play area.
Then if the line trace hits something it check if it has a health component than
get its team number if it does it lerps to the center of the actor and sets the controller Rotation.*/
bool ASCharacter::AimAssist(float DeltaTime)
{
	if (CurrentWeapon)
	{
	
		FVector EyeLocation;
		FRotator EyeRotation;
		GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		CurrentWeapon->CreateLineTraceCollisionQuery(QueryParams, this);

		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, TRACING_ENEMY, QueryParams))
		{
			USHealthComponent* HitTarget = Cast<USHealthComponent>(Hit.GetActor()->GetComponentByClass(USHealthComponent::StaticClass()));
			if (HitTarget)
			{
				USHealthComponent* MyTeam = Cast<USHealthComponent>(GetComponentByClass(USHealthComponent::StaticClass()));
				if (HitTarget->TeamNum != MyTeam->TeamNum)
				{
					ASPlayerController* MyController = Cast<ASPlayerController>(Controller);
					if (!MyController)return false;

					FVector MagDirection= Hit.GetActor()->GetActorLocation() - Hit.ImpactPoint;
					MagDirection.Normalize();
					FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(MagDirection);
					TargetRotation = FMath::RInterpTo(MyController->GetControlRotation(), TargetRotation, DeltaTime, MagnetismStrenght);
					MyController->SetControlRotation(FRotator(GetControlRotation().Pitch, TargetRotation.Yaw, GetControlRotation().Roll));
					UE_LOG(LogTemp, Warning, TEXT("Aim Assist is true."));
					return true;
				}
			}
		}
	}
	return false;
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = AimDownSight ? ZoomedFOV : DefaultFOV ;
	float NewFOV = FMath::FInterpTo(Camera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	Camera->SetFieldOfView(NewFOV);
	
	bIsAimAssistOn = AimAssist(DeltaTime);
		if (Role == ROLE_Authority)
		{
			FRotator NewRot = (GetControlRotation() - GetActorRotation());
			NewRot.Normalize();
			AimOffSetPitch = NewRot.Pitch;
		}
		if (!bInPlayArea && !bIsDead)
		{
			ElaspeTime += DeltaTime;
			if (ElaspeTime < 0.1)
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayCountdown = %f"), PlayCountDown);
				PlayCountDown -= ElaspeTime;
				ElaspeTime = 0;
				if (PlayCountDown <= 0)
				{
					PlayCountDown = 0;
					OnHealthChanged(HealthComp, 0.0, 0.0, nullptr, GetController(), this);
				}
			}	
		}

	
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//Binds movement
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	//Bind Looking
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	//Binds Movement Actions
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::BeginJumping);
	PlayerInputComponent->BindAction("AimDownSights", IE_Pressed, this, &ASCharacter::BeginAim);
	PlayerInputComponent->BindAction("AimDownSights", IE_Released, this, &ASCharacter::EndAim);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::ReloadWeapon);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (Camera)
	{
		return Camera->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void ASCharacter::MoveForward(float value)
{
	if (bIsPaused)return;	
		AddMovementInput(GetActorForwardVector() * value);
}

void ASCharacter::MoveRight(float value)
{
	if (bIsPaused)return;	
		AddMovementInput(GetActorRightVector() * value);
}

void ASCharacter::AddControllerPitchInput(float Val)
{
	if (bIsPaused)return;
	float NewValue = bIsAimAssistOn ? Val / 2 : Val;
	UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
	if (userSettings)
	{
		NewValue *= userSettings->GetPitchSesitivity();
	}
	Super::AddControllerPitchInput(NewValue);
}

void ASCharacter::AddControllerYawInput(float Val)
{
	if (bIsPaused)return;
	float NewValue = bIsAimAssistOn ? Val / 2 : Val ;
		UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
	if (userSettings)
	{
		NewValue *= userSettings->GetYawSesitivity();
	}
	Super::AddControllerYawInput(NewValue);	
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
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (bIsPaused)return;
	if (CurrentWeapon)
	{
		CurrentWeapon->EndFire();
	}
}

void ASCharacter::ReloadWeapon()
{	
	if ( bIsPaused)return;
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

USpringArmComponent* ASCharacter::GetSpringArm()
{
	return SpringArm;
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
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

		SetLifeSpan(6.0f);	
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);

	DOREPLIFETIME(ASCharacter, bIsDead);

	DOREPLIFETIME(ASCharacter, AimOffSetPitch);
	
}