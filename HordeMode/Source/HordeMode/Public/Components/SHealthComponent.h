// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

//OnHealthChange event;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent*, HealthComponent, float, Health, float, HealthDelta ,const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HORDEMODE_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category = "Health")
		float HealthPoints;

	UFUNCTION()
		void OnRep_Health(float OldHealth);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
		float MaxHealthPoints;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	bool bisDead;

	bool bDoOnce;

public:

	float GetHealth() const { return HealthPoints; } ;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;
	

	UFUNCTION(BlueprintCallable, Category = "Health")
		void Heal(float HealthAmount);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	uint8 TeamNum;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	static bool isFriendly(AActor* ActorA, AActor* ActorB);
};
