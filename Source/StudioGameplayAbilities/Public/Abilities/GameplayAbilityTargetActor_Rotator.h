// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_Rotator.generated.h"

/**
 * GameplayAbilityTargetActor_Rotator
 */
UCLASS(Blueprintable, notplaceable, config = Game)
class STUDIOGAMEPLAYABILITIES_API AGameplayAbilityTargetActor_Rotator : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()

	virtual void ConfirmTargetingAndContinue() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Rotator)
	void K2_PreConfirmTargetingAndContinue();

	UFUNCTION(BlueprintCallable, Category = Rotator)
	void Add(const FRotator& NewValue);

	UFUNCTION(BlueprintCallable, Category = Rotator)
	void Clear();

	UFUNCTION(BlueprintCallable, Category = Rotator)
	int32 Num() const;

	UPROPERTY(BlueprintReadOnly, Category = Rotator)
	TArray<FRotator> ValueArray;
};
