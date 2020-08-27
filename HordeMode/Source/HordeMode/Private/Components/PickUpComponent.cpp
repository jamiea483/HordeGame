// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpComponent.h"
#include "InteractBase.h"
#include "SCharacter.h"
#include "SWeapon.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UPickUpComponent::UPickUpComponent()
{

	// ...
}

///Pick up System
void UPickUpComponent::OnInteract()
{
	if (InteractableItem)
	{
		ASCharacter* Char = Cast<ASCharacter>(GetOwner());
		if(Char)
		if (InteractableItem->InteractableName == Char->GetCurrentWeapon()->InteractableName)
		{
			Char->GetCurrentWeapon()->AddtoMaxAmmo(50.f);
			InteractableItem->Destroy();
		}
	}
}

void UPickUpComponent::RegisterWeapon(AInteractBase * Interactable)
{
	HandleRegisterWeapon(Interactable);
}

void UPickUpComponent::UnregisterWeapon(AInteractBase * Interactable)
{
	HandleUnregisterWeapon(Interactable);
}


void UPickUpComponent::HandleRegisterWeapon(AInteractBase * Interactable)
{
	WeaponList.AddUnique(Interactable);
}

void UPickUpComponent::HandleUnregisterWeapon(AInteractBase * Interactable)
{
	WeaponList.Remove(Interactable);
}

void UPickUpComponent::BestInteractable()
{

	if (WeaponList.Num() >= 1)
		GetBestInteractable();
	else
		SetBestInteractable(nullptr);
}

void UPickUpComponent::GetBestInteractable()
{
	InteractableInfo.Dot = -1;
	ASCharacter* Char = Cast<ASCharacter>(GetOwner());
	if (Char)
	for (AInteractBase* Interactable : WeaponList)
	{
		UE_LOG(LogTemp, Warning, TEXT("Best Interactable"));
		//If character is looking toward it is 1 or opposite direction of the interactable is it -1. 
		float DotOfInteractable = FVector::DotProduct(Char->GetCamera()->GetForwardVector(), (Interactable->GetSphereComponent()->GetCenterOfMass() - Char->GetCamera()->GetComponentLocation()).GetSafeNormal());

		if (DotOfInteractable > InteractableInfo.Dot && DotOfInteractable > SightLeniency)
		{
			InteractableInfo.Dot = DotOfInteractable;
			InteractableInfo.Interactable = Interactable;
		}
	}
	if (InteractableInfo.Interactable)
		SetBestInteractable(InteractableInfo.Interactable);
}


/*If an interactable was pass Set the interactableitem name to the interactable and if nothing was pass
Interactableitem equal null*/
void UPickUpComponent::SetBestInteractable(AInteractBase* Interactable)
{
	if (Interactable)
	{
		if (Interactable != InteractableItem)
		{
			bCanPickUp = true;
			InteractableItem = Interactable;
			InteractableName = Interactable->InteractableName;
			ActiveBestInteractable.Broadcast();
		}
	}
	else
	{
		bCanPickUp = false;
		InteractableItem = nullptr;
		ActiveBestInteractable.Broadcast();
	}
}


