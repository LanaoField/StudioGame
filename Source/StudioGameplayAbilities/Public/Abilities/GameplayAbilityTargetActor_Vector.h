// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_Vector.generated.h"

/**
 * GameplayAbilityTargetActor_Vector
 */
UCLASS(Blueprintable, notplaceable, config = Game)
class STUDIOGAMEPLAYABILITIES_API AGameplayAbilityTargetActor_Vector : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()

	virtual void ConfirmTargetingAndContinue() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Vector)
	void K2_PreConfirmTargetingAndContinue();

	UFUNCTION(BlueprintCallable, Category = Vector)
	void Add(const FVector& NewValue);

	UFUNCTION(BlueprintCallable, Category = Vector)
	void Clear();

	UFUNCTION(BlueprintCallable, Category = Vector)
	int32 Num() const;

	UPROPERTY(BlueprintReadOnly, Category = Vector)
	TArray<FVector> ValueArray;
};
