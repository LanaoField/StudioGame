// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractiveInterface.h"
#include "InteractiveActor.generated.h"


class AInteractiveActor;
class UPrimitiveComponent;
class UMotionControllerComponent;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_ThreeParams(FPresseInteractionSignature, AInteractiveActor, OnPresseInteraction, AActor*, InteractiveActor, UPrimitiveComponent*, InteractiveComponent, UPrimitiveComponent*, TriggerComponent);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_ThreeParams(FReleaseInteractionSignature, AInteractiveActor, OnReleaseInteraction, AActor*, InteractiveActor, UPrimitiveComponent*, InteractiveComponent, UPrimitiveComponent*, TriggerComponent);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FPenddingDestorySignature, AInteractiveActor, OnPenddingDestory, float, InLifespan);

UCLASS()
class STUDIOGAME_API AInteractiveActor : public AActor, public IInteractiveInterface
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void SetLifeSpan(float InLifespan) override;

	/** Event when this actor is clicked by the mouse when using the clickable interface. */
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

	/** Event when this actor is touched when click events are enabled. */
	virtual void NotifyActorOnInputTouchBegin(const ETouchIndex::Type FingerIndex) override;
	virtual void NotifyActorOnInputTouchEnd(const ETouchIndex::Type FingerIndex) override;

	// Begin IInteractiveInterface
	virtual void PresseInteraction_Implementation(class AActor* InteractiveActor, class UPrimitiveComponent* InteractiveComponent, class UPrimitiveComponent* TriggerComponent, FKey Key) override;
	virtual void ReleaseInteraction_Implementation(class AActor* InteractiveActor, class UPrimitiveComponent* InteractiveComponent, class UPrimitiveComponent* TriggerComponent, FKey Key) override;
	// End IInteractiveInterface

	virtual void PenddingDestory(float InLifespan);

	/** Called when the actor has been explicitly griped. */
	UFUNCTION(BlueprintImplementableEvent, meta = (Keywords = "grip", DisplayName = "PenddingDestory"))
	void ReceivePenddingDestory(float InLifespan);
	
public:
	UPROPERTY(BlueprintAssignable, Category = Game)
	FPresseInteractionSignature OnPresseInteraction;

	UPROPERTY(BlueprintAssignable, Category = Game)
	FReleaseInteractionSignature OnReleaseInteraction;

	UPROPERTY(BlueprintAssignable, Category = Game)
	FPenddingDestorySignature OnPenddingDestory;
};
