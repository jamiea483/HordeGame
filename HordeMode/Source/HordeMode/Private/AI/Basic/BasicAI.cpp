// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAI.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "HordeGameUserSettings.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "SCharacter.h"
#include "SHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"



static int32 DebugTrackerBotDrawing = 0;
FAutoConsoleVariableRef TrackerBotDebug(TEXT("Horde.DebugBots"),
	DebugTrackerBotDrawing,
	TEXT("Draw Debug Lines for traker bots"),
	ECVF_Cheat);

// Sets default values
ABasicAI::ABasicAI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCanEverAffectNavigation(false);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health Component"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ABasicAI::OnHealthChanged);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComp->SetSphereRadius(350);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(MeshComp);

	
	RequiredDistanceToTarget = 100;
	MovementForce = 10000;

	BlastRadius = 350;
	ExplosionDamage = 100;
	SelfDamageInterval = 0.2;
}

// Called when the game starts or when spawned
void ABasicAI::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		FVector Nextpoint = GetNextPathPoint();
	}
	

}

FVector ABasicAI::GetNextPathPoint()
{
	AActor* BestActor = nullptr;
	float nearestTarget = FLT_MAX;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || USHealthComponent::isFriendly( TestPawn, this))
		{
			continue;
		}
		USHealthComponent* HealthComponent = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
		{
			float Distance = (TestPawn->GetActorLocation() - GetActorLocation()).Size();
			if (Distance < nearestTarget)
			{
				BestActor = TestPawn;
				nearestTarget = Distance;
			}
		}
	}

	if (BestActor)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestActor);

		GetWorldTimerManager().ClearTimer(TimerHandle_PathTimer);

		GetWorldTimerManager().SetTimer(TimerHandle_PathTimer, this, &ABasicAI::RefreshPath, 4.0f, false);
		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			//Returns next point in the path
			return NavPath->PathPoints[1];
		}
	}

		//failed to find path.
		return GetActorLocation();
}
	

void ABasicAI::OnHealthChanged(USHealthComponent * HealthComponent, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Took damage."));

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

		//Explode on hitpoints == 0
	if (Health <= 0.0f)
	{
		SelfDestruct();
	}

}

void ABasicAI::SelfDestruct()
{
	if (bExploded)return;

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
	if (userSettings)
	{
		UGameplayStatics::SpawnSoundAttached(ExplosionSound, RootComponent, "", GetActorLocation(), EAttachLocation::SnapToTarget, false, (userSettings->GetSoundEffectVolume() / 100), 1.f, 0.0f, nullptr, nullptr, true);
	}

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		//Apply damage
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), BlastRadius, nullptr, IgnoreActors, this, GetInstigatorController(), true, ECC_Visibility);

		SetLifeSpan(2.0);
	}
}

void ABasicAI::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}

// Called every frame
void ABasicAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority && !bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();


		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
		}
		else
		{
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			BotMovement(ForceDirection, DeltaTime);

			if (DebugTrackerBotDrawing)
			{
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Green, false, 0.0f, 0, 1.0f);
			}
			
		}
		if (DebugTrackerBotDrawing)
		{
			DrawDebugSphere(GetWorld(), NextPathPoint, 20.0f, 12, FColor::Green, false, 0.0f, 0, 1.0f);
		}
	}

}

void ABasicAI::BotMovement(FVector &ForceDirection, float DeltaTime)
{
	ElapseTime += DeltaTime;
	if (ElapseTime >= 0.1)
	{
		ElapseTime = 0;
		ForceDirection *= MovementForce;
		//Keep moving to target
		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelChange);
		
	}
}

void ABasicAI::DamageSelf()
{
	UE_LOG(LogTemp, Warning, TEXT("damage self."));
	UGameplayStatics::ApplyDamage(this, 20.0f, GetInstigatorController(), this, nullptr);
}

void ABasicAI::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!bStartedSelfDestruction && !bExploded)
	{
		//Timer for the to bot to kill it self
		ASCharacter* MyChar = Cast<ASCharacter>(OtherActor);
		if (MyChar && !USHealthComponent::isFriendly(OtherActor, this))
		{
			
			if (GetLocalRole() == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(TimeHandle_SelfDamage, this, &ABasicAI::DamageSelf, SelfDamageInterval, true, 0.0f);
			}
			bStartedSelfDestruction = true;

			UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
			if (userSettings)
			{
				UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent, "", GetActorLocation(), EAttachLocation::SnapToTarget, false, (userSettings->GetSoundEffectVolume() / 100), 1.f, 0.0f, nullptr, nullptr, true);
			}
		}
	}

}


