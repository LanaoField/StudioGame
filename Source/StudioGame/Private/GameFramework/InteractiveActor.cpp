// Copyright 2022 CQUnreal. All Rights Reserved.

#include "GameFramework/InteractiveActor.h"


AInteractiveActor::AInteractiveActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AInteractiveActor::SetLifeSpan(float InLifespan)
{
	Super::SetLifeSpan(InLifespan);

	if (InLifespan > 0.f)
	{
		SetActorEnableCollision(false);		
	}
}

void AInteractiveActor::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	PresseInteraction(nullptr, nullptr, Cast<UPrimitiveComponent>(RootComponent), ButtonPressed);
	ReleaseInteraction(nullptr, nullptr, Cast<UPrimitiveComponent>(RootComponent), ButtonPressed);
}

void AInteractiveActor::NotifyActorOnInputTouchBegin(const ETouchIndex::Type FingerIndex)
{
	Super::NotifyActorOnInputTouchBegin(FingerIndex);

	PresseInteraction(nullptr, nullptr, Cast<UPrimitiveComponent>(RootComponent), FKey());
}

void AInteractiveActor::NotifyActorOnInputTouchEnd(const ETouchIndex::Type FingerIndex)
{
	Super::NotifyActorOnInputTouchEnd(FingerIndex);

	ReleaseInteraction(nullptr, nullptr, Cast<UPrimitiveComponent>(RootComponent), FKey());
}

void AInteractiveActor::PresseInteraction_Implementation(AActor* InteractiveActor, UPrimitiveComponent* InteractiveComponent, UPrimitiveComponent* TriggerComponent, FKey Key)
{
	OnPresseInteraction.Broadcast(InteractiveActor, InteractiveComponent, TriggerComponent);
}

void AInteractiveActor::ReleaseInteraction_Implementation(AActor* InteractiveActor, UPrimitiveComponent* InteractiveComponent, UPrimitiveComponent* TriggerComponent, FKey Key)
{
	OnReleaseInteraction.Broadcast(InteractiveActor, InteractiveComponent, TriggerComponent);
}

void AInteractiveActor::PenddingDestory(float InLifespan)
{
	ReceivePenddingDestory(InLifespan);

	OnPenddingDestory.Broadcast(InLifespan);
}

