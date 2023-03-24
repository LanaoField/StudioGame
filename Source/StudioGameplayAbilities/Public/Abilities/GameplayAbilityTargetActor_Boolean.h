// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_Boolean.generated.h"

/**
 * GameplayAbilityTargetActor_Boolean
 */
UCLASS(Blueprintable, notplaceable, config = Game)
class STUDIOGAMEPLAYABILITIES_API AGameplayAbilityTargetActor_Boolean : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()

	virtual void ConfirmTargetingAndContinue() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Boolean)
	void K2_PreConfirmTargetingAndContinue();

	UFUNCTION(BlueprintCallable, Category = Boolean)
	void Add(const bool& NewValue);

	UFUNCTION(BlueprintCallable, Category = Boolean)
	void Clear();

	UFUNCTION(BlueprintCallable, Category = Boolean)
	int32 Num() const;

	UPROPERTY(BlueprintReadOnly, Category = Boolean)
	TArray<bool> ValueArray;
};
