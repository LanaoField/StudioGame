// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectTypesEx.generated.h"


/**
 * GameplayEffectContextEx
 */
USTRUCT()
struct STUDIOGAMEPLAYABILITIES_API FGameplayEffectContextEx : public FGameplayEffectContext
{
	GENERATED_USTRUCT_BODY()

	FGameplayEffectContextEx();
	FGameplayEffectContextEx(AActor* InInstigator, AActor* InEffectCauser);
	virtual ~FGameplayEffectContextEx();

	virtual FGameplayAbilityTargetDataHandle GetTargetData() const;

	virtual void AddTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FGameplayEffectContext* Duplicate() const override;

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override;

protected:
	FGameplayAbilityTargetDataHandle TargetData;
};

template<>
struct TStructOpsTypeTraits< FGameplayEffectContextEx > : public TStructOpsTypeTraitsBase2< FGameplayEffectContextEx >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TArray<FHitResult> Data is copied around
	};
};