// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_HitResult.generated.h"

/**
 * GameplayAbilityTargetActor_HitResult
 */
UCLASS(Blueprintable, notplaceable, config = Game)
class STUDIOGAMEPLAYABILITIES_API AGameplayAbilityTargetActor_HitResult : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()

	virtual void ConfirmTargetingAndContinue() override;

	UFUNCTION(BlueprintImplementableEvent, Category = HitResult)
	void K2_PreConfirmTargetingAndContinue();

	UFUNCTION(BlueprintCallable, Category = HitResult)
	void Add(const FHitResult& NewValue);

	UFUNCTION(BlueprintCallable, Category = HitResult)
	void Clear();

	UFUNCTION(BlueprintCallable, Category = HitResult)
	int32 Num() const;

	UPROPERTY(BlueprintReadOnly, Category = HitResult)
	TArray<FHitResult> ValueArray;
};
