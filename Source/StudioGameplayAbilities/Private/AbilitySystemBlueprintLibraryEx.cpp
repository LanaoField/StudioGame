// Copyright 2022 CQUnreal. All Rights Reserved.

#include "AbilitySystemBlueprintLibraryEx.h"


GENERATED_TARGETDATA_BODY(UAbilitySystemBlueprintLibraryEx, Boolean, bool)
GENERATED_TARGETDATA_BODY(UAbilitySystemBlueprintLibraryEx, Float, float)
GENERATED_TARGETDATA_BODY(UAbilitySystemBlueprintLibraryEx, Int32, int32)
GENERATED_TARGETDATA_BODY(UAbilitySystemBlueprintLibraryEx, Vector, FVector)
GENERATED_TARGETDATA_BODY(UAbilitySystemBlueprintLibraryEx, Rotator, FRotator)
GENERATED_TARGETDATA_BODY(UAbilitySystemBlueprintLibraryEx, Transform, FTransform)
GENERATED_TARGETDATA_BODY(UAbilitySystemBlueprintLibraryEx, HitResult, FHitResult)

bool UAbilitySystemBlueprintLibraryEx::DoesEffectContainerSpecHaveEffects(const FGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidEffects();
}

bool UAbilitySystemBlueprintLibraryEx::DoesEffectContainerSpecHaveTargets(const FGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidTargets();
}

FGameplayEffectContainerSpec UAbilitySystemBlueprintLibraryEx::AddTargetsToEffectContainerSpec(const FGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	FGameplayEffectContainerSpec NewSpec = ContainerSpec;
	NewSpec.AddTargets(HitResults, TargetActors);
	return NewSpec;
}

TArray<FActiveGameplayEffectHandle> UAbilitySystemBlueprintLibraryEx::ApplyExternalEffectContainerSpec(const FGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
}
