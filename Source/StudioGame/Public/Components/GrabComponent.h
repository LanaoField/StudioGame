// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GrabComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Grab), meta = (BlueprintSpawnableComponent))
class STUDIOGAME_API UGrabComponent : public USphereComponent
{
	GENERATED_UCLASS_BODY()
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void PressPointerKey(FKey Key);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void ReleasePointerKey(FKey Key);

	void UpdateInteractiveComponent(float DeltaTime);

protected:
	UPROPERTY(Transient)
	UPrimitiveComponent* InteractiveComponent;
};
