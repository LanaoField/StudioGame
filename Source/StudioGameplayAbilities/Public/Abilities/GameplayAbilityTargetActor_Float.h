// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_Float.generated.h"

/**
 * GameplayAbilityTargetActor_Float
 */
UCLASS(Blueprintable, notplaceable, config = Game)
class STUDIOGAMEPLAYABILITIES_API AGameplayAbilityTargetActor_Float : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()

	virtual void ConfirmTargetingAndContinue() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Float)
	void K2_PreConfirmTargetingAndContinue();

	UFUNCTION(BlueprintCallable, Category = Float)
	void Add(const float& NewValue);

	UFUNCTION(BlueprintCallable, Category = Float)
	void Clear();

	UFUNCTION(BlueprintCallable, Category = Float)
	int32 Num() const;

	UPROPERTY(BlueprintReadOnly, Category = Float)
	TArray<float> ValueArray;
};
