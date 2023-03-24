// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_Int32.generated.h"

/**
 * GameplayAbilityTargetActor_Integer
 */
UCLASS(Blueprintable, notplaceable, config = Game)
class STUDIOGAMEPLAYABILITIES_API AGameplayAbilityTargetActor_Int32 : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()

	virtual void ConfirmTargetingAndContinue() override;

	UFUNCTION(BlueprintImplementableEvent, Category = Integer)
	void K2_PreConfirmTargetingAndContinue();

	UFUNCTION(BlueprintCallable, Category = Integer)
	void Add(const int32& NewVector);

	UFUNCTION(BlueprintCallable, Category = Integer)
	void Clear();

	UFUNCTION(BlueprintCallable, Category = Integer)
	int32 Num() const;

	UPROPERTY(BlueprintReadOnly, Category = Integer)
	TArray<int32> ValueArray;
};
