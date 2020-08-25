// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSCharacter.h"
#include "Components/InputComponent.h"
#include "SHealthComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "HordeMode.h"


#include "Camera/CameraComponent.h"
#include "SPlayerController.h"
#include "SWeapon.h"
#include "HordeGameUserSettings.h"



APlayerSCharacter::APlayerSCharacter() {

}

// Called every frame
void APlayerSCharacter::Tick(float DeltaTime)
{
	float TargetFOV = AimDownSight ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(Camera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	Camera->SetFieldOfView(NewFOV);

	if (bIsMoving)
		bIsAimAssistOn = AimAssist(DeltaTime);
	if (!bInPlayArea && !bIsDead)
	{
		ElaspeTime += DeltaTime;
		if (ElaspeTime > 0.1)
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
void APlayerSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Binds movement
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerSCharacter::MoveRight);

	//Bind Looking
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerSCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerSCharacter::AddControllerYawInput);

	//Binds Movement Actions
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerSCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerSCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerSCharacter::BeginJumping);
	PlayerInputComponent->BindAction("AimDownSights", IE_Pressed, this, &APlayerSCharacter::BeginAim);
	PlayerInputComponent->BindAction("AimDownSights", IE_Released, this, &APlayerSCharacter::EndAim);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerSCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerSCharacter::StopFire);
	PlayerInputComponent->BindAction("Reload/PickUpWeapon", IE_Pressed, this, &APlayerSCharacter::ReloadWeapon);
	PlayerInputComponent->BindAction("ChangeWeapon", IE_Pressed, this, &APlayerSCharacter::SwitchWeapon);
}

FVector APlayerSCharacter::GetPawnViewLocation() const
{
	if (Camera)
	{
		return Camera->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void APlayerSCharacter::MoveForward(float value)
{
	if (bIsPaused)return;
	AddMovementInput(GetActorForwardVector() * value);
}

void APlayerSCharacter::MoveRight(float value)
{
	if (bIsPaused)return;
	bIsMoving = value == 0 ? false : true;
	AddMovementInput(GetActorRightVector() * value);
}

void APlayerSCharacter::AddControllerPitchInput(float Val)
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

void APlayerSCharacter::AddControllerYawInput(float Val)
{
	if (bIsPaused)return;
	float NewValue = bIsAimAssistOn ? Val / 2 : Val;
	UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
	if (userSettings)
	{
		NewValue *= userSettings->GetYawSesitivity();
	}
	Super::AddControllerYawInput(NewValue);
}

/**Creates a line trace from the center of the camera to 10000 units away
and creates a query to ignore it owning actor and the play area.
Then if the line trace hits something it check if it has a health component than
get its team number if it does it lerps to the center of the actor and sets the controller Rotation.*/
bool APlayerSCharacter::AimAssist(float DeltaTime)
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

					FVector MagDirection = Hit.GetActor()->GetActorLocation() - Hit.ImpactPoint;
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
