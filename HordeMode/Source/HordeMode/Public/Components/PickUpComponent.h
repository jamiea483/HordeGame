// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractInterface.h"
#include "PickUpComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActiveBestInteractable);

USTRUCT(BlueprintType)
struct FInteractableVariables
{
	GENERATED_BODY()
public:
	float Dot;

	class AInteractBase* Interactable;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HORDEMODE_API UPickUpComponent : public UActorComponent, public IInteractInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPickUpComponent();

public:	
	UFUNCTION(BlueprintCallable, Category = "Interact")
		void BestInteractable();

	bool GetCanPickup() { return bCanPickUp; };

	UFUNCTION(BlueprintCallable, Category = "Interact")
		virtual void OnInteract();
	UFUNCTION(BlueprintCallable, Category = "Interact")
		virtual void RegisterWeapon(class AInteractBase* Interactable);
	UFUNCTION(BlueprintCallable, Category = "Interact")
		virtual void UnregisterWeapon(class AInteractBase* Interactable);

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnActiveBestInteractable ActiveBestInteractable;



protected:
	/**Adds interactable to weaponlist as a unique*/
	UFUNCTION(BlueprintCallable, Category = "Interact")
		void HandleRegisterWeapon(class AInteractBase* Interactable);
	UFUNCTION(BlueprintCallable, Category = "Interact")
		void HandleUnregisterWeapon(class AInteractBase* Interactable);

	/**Updates mainInteractable and InteractableName*/
	UFUNCTION(BlueprintCallable, Category = "Interact")
		void GetBestInteractable();
	UFUNCTION(BlueprintCallable, Category = "Interact")
		void SetBestInteractable(class AInteractBase* Interactable);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
		FName InteractableName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
		AInteractBase* InteractableItem;

	/**How far can look away from the iteractable*/
	UPROPERTY(EditDefaultsOnly, Category = "Interact")
		float SightLeniency;

	UPROPERTY()
		FInteractableVariables InteractableInfo;

	/**An Array that holds all the weapon in range of the character that he can Pick up*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
		TArray<class AInteractBase*> WeaponList;

	/**An Array that holds all the weapon in range of the character that he can Pick up*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Player")
		bool bCanPickUp;

	virtual void GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const override;
};
