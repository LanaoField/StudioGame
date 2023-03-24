// Copyright 2018 CQUnreal, Inc. All Rights Reserved.

#pragma once

#include "FlyingAIInterface.generated.h"

UINTERFACE()
class STUDIOGAME_API UFlyingAIInterface : public UInterface
{
	GENERATED_BODY()
};

class STUDIOGAME_API IFlyingAIInterface
{
	GENERATED_BODY()

public:
	/* Optional custom movement input. If not provided, default AddMovementInput behavior will be called on the pawn or character*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FlyingAIInterface")
	void AddMovementInputFlying(FVector WorldDirection, float ScaleValue);	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FlyingAIInterface")
	void OnLocomotionBegin();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FlyingAIInterface")
	void OnLocomotionEnd(const bool bLocomotionSuccess);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FlyingAIInterface")
	void OnLocomotionAbort();
};