// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_Transform.generated.h"

/**
 * GameplayAbilityTargetActor_Transform
 */
UCLASS(Blueprintable, notplaceable, config = Game)
class STUDIOGAMEPLAYABILITIES_API AGameplayAbilityTargetActor_Transform : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()

	virtual void ConfirmTargetingAndContinue() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Transform)
	void K2_PreConfirmTargetingAndContinue();

	UFUNCTION(BlueprintCallable, Category = Transform)
	void Add(const FTransform& NewValue);

	UFUNCTION(BlueprintCallable, Category = Transform)
	void Clear();

	UFUNCTION(BlueprintCallable, Category = Transform)
	int32 Num() const;

	UPROPERTY(BlueprintReadOnly, Category = Transform)
	TArray<FTransform> ValueArray;
};
