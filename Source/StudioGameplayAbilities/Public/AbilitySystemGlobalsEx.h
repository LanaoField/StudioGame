// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemGlobalsEx.generated.h"

/**
 * AbilitySystemGlobalsEx
 */
UCLASS()
class STUDIOGAMEPLAYABILITIES_API UAbilitySystemGlobalsEx : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	/** Should allocate a project specific GameplayEffectContext struct. Caller is responsible for deallocation */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
