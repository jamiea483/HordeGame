// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include "GameFramework/Actor.h"
#include "GameMode/HordeModeGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	SetIsReplicatedByDefault(true);
	bisDead = false;
	// ...

	TeamNum = 255;

	MaxHealthPoints = 100;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only hook if we are Server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}

	}
	HealthPoints = MaxHealthPoints;
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = HealthPoints - OldHealth;

	OnHealthChanged.Broadcast(this, HealthPoints, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Took Damage."));
	if (Damage <= 0)return;

	
	if ( DamageCauser != DamagedActor && isFriendly(DamagedActor, DamageCauser))return;

	HealthPoints = FMath::Clamp(HealthPoints - Damage, 0.0f, MaxHealthPoints);

	bisDead = HealthPoints <= 0.0f;

	OnHealthChanged.Broadcast(this,HealthPoints,Damage, DamageType, InstigatedBy, DamageCauser);

	if (bisDead && !bDoOnce)
	{
		bDoOnce = true;
		AHordeModeGameMode* GM = Cast<AHordeModeGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}

}

void USHealthComponent::Heal(float HealthAmount)
{
	if (HealthAmount <= 0.0f || HealthPoints <= 0.0f)return;

	HealthPoints = FMath::Clamp(HealthPoints + HealthAmount, 0.0f, MaxHealthPoints);

	OnHealthChanged.Broadcast(this, HealthPoints, -HealthAmount, nullptr, nullptr, nullptr);
}

bool USHealthComponent::isFriendly(AActor * ActorA, AActor * ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)return true;

	USHealthComponent* PawnAHealth = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* PawnBHealth = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));


	if (PawnAHealth == nullptr || PawnBHealth == nullptr) return true;

	return PawnAHealth->TeamNum == PawnBHealth->TeamNum;
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, HealthPoints);

}
