// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayAbilityTypesEx.generated.h"


class UGameplayEffect;
class UGameplayTargetType;

/**
 * Struct defining a list of gameplay effects, a tag, and targeting info
 * These containers are defined statically in blueprints or assets and then turn into Specs at runtime
 */
USTRUCT(BlueprintType)
struct STUDIOGAMEPLAYABILITIES_API FGameplayEffectContainer
{
	GENERATED_BODY()

public:
	FGameplayEffectContainer() {}

	/** Sets the way that targeting happens */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TSubclassOf<class UGameplayTargetType> TargetType;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffectClasses;
};

/** A "processed" version of GameplayEffectContainerSpec that can be passed around and eventually applied */
USTRUCT(BlueprintType)
struct STUDIOGAMEPLAYABILITIES_API FGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:
	FGameplayEffectContainerSpec()
	{

	}

	/** Computed target data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	/** Returns true if this has any valid effect specs */
	bool HasValidEffects() const;

	/** Returns true if this has any valid targets */
	bool HasValidTargets() const;

	/** Adds new targets to target data */
	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
};