// Copyright 2022 CQUnreal. All Rights Reserved.


#include "Components/GrabComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Interfaces/InteractiveInterface.h"

UGrabComponent::UGrabComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrabComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateInteractiveComponent(DeltaTime);
}

void UGrabComponent::UpdateInteractiveComponent(float DeltaTime)
{
	float MinDistance = BIG_NUMBER;

	UPrimitiveComponent* NewInteractiveComponent = nullptr;

	TArray<UPrimitiveComponent*> InteractiveComponents;
	GetOverlappingComponents(InteractiveComponents);
	for (int32 i = 0; i < OverlappingComponents.Num(); i++)
	{
		UPrimitiveComponent* OverlappingComponent = InteractiveComponents[i];

		AActor* OverlappingActor = OverlappingComponent->GetOwner();
		if (OverlappingActor == nullptr || !OverlappingActor->Implements<UInteractiveInterface>())
		{
			continue;
		}

		const float Distance = FVector::Distance(OverlappingComponent->GetComponentLocation(), GetComponentLocation());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;

			NewInteractiveComponent = OverlappingComponent;
		}
	}

	if (NewInteractiveComponent != InteractiveComponent)
	{
		if (InteractiveComponent != nullptr)
		{
			IInteractiveInterface::Execute_EndFocus(InteractiveComponent->GetOwner(), GetOwner()->GetInstigator(), this, InteractiveComponent);
		}

		InteractiveComponent = NewInteractiveComponent;
		if (InteractiveComponent != nullptr)
		{
			IInteractiveInterface::Execute_BeginFocus(InteractiveComponent->GetOwner(), GetOwner()->GetInstigator(), this, InteractiveComponent);
		}
	}

	if (InteractiveComponent != nullptr)
	{
		IInteractiveInterface::Execute_Focus(InteractiveComponent->GetOwner(), GetOwner()->GetInstigator(), this, InteractiveComponent);
	}	
}


void UGrabComponent::PressPointerKey(FKey Key)
{
	if (InteractiveComponent != nullptr)
	{
		IInteractiveInterface::Execute_PresseInteraction(InteractiveComponent->GetOwner(), GetOwner()->GetInstigator(), this, InteractiveComponent, Key);
	}
}

void UGrabComponent::ReleasePointerKey(FKey Key)
{
	if (InteractiveComponent != nullptr)
	{
		IInteractiveInterface::Execute_ReleaseInteraction(InteractiveComponent->GetOwner(), GetOwner()->GetInstigator(), this, InteractiveComponent, Key);
	}
}