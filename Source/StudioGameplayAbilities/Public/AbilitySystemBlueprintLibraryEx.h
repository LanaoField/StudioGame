// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypesEx.h"
#include "Abilities/GameplayAbilityTypesEx.h"
#include "Abilities/GameplayAbilityTargetTypesEx.h"
#include "AbilitySystemBlueprintLibraryEx.generated.h"


template <typename T1, typename T2>
FGameplayAbilityTargetDataHandle MakeGameplayAbilityTargetData(const T2& Data)
{
	FGameplayAbilityTargetDataHandle NewTargetDataHandle;
	NewTargetDataHandle.Add(new T1(Data));
	return NewTargetDataHandle;
}

template <typename T1, typename T2>
FGameplayAbilityTargetDataHandle MakeGameplayAbilityTargetDataFromArray(TArray<T2> Array)
{
	FGameplayAbilityTargetDataHandle NewTargetDataHandle;
	for (const T2& Data : Array)
	{
		NewTargetDataHandle.Add(new T1(Data));
	}
	return NewTargetDataHandle;
}

template <typename T>
TArray<T> GetGameplayAbilityTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	TArray<T> Results;
	for (int32 Index = 0; Index < TargetDataHandle.Num(); Index++)
	{
		if (!TargetDataHandle.IsValid(Index))
		{
			continue;
		}

		const FGameplayAbilityTargetData* TargetData = TargetDataHandle.Get(Index);
		if (TargetData && TargetData->GetScriptStruct() == T::StaticStruct())
		{
			Results.Add(*static_cast<const T*>(TargetData));
		}
	}

	return Results;
}

template <typename T>
TArray<T> GetGameplayAbilityTargetDataByEffectContext(const FGameplayEffectContextHandle& EffectContextHandle)
{
	TArray<T> Results;

	const FGameplayEffectContext* EffectContext = EffectContextHandle.IsValid() ? EffectContextHandle.Get() : nullptr;
	if (EffectContext && EffectContext->GetScriptStruct() == FGameplayEffectContextEx::StaticStruct())
	{
		const FGameplayEffectContextEx* EffectContextEx = static_cast<const FGameplayEffectContextEx*>(EffectContext);
		if (EffectContextEx != nullptr)
		{
			FGameplayAbilityTargetDataHandle TargetDataHandle = EffectContextEx->GetTargetData();
			for (int32 Index = 0; Index < TargetDataHandle.Num(); Index++)
			{
				if (!TargetDataHandle.IsValid(Index))
				{
					continue;
				}

				const FGameplayAbilityTargetData* TargetData = TargetDataHandle.Get(Index);
				if (TargetData && TargetData->GetScriptStruct() == T::StaticStruct())
				{
					Results.Add(*static_cast<const T*>(TargetData));
				}
			}
		}
	}

	return Results;
}

#ifndef GENERATED_TARGETDATA_BODY
#define GENERATED_TARGETDATA_BODY(Class, Name, Type) \
FGameplayAbilityTargetDataHandle Class::MakeTargetDataWith##Name(const Type& InValue)	\
{	\
	return MakeGameplayAbilityTargetData<FGameplayAbilityTargetData_##Name>(InValue);	\
}	\
\
FGameplayAbilityTargetDataHandle Class::MakeTargetDataWith##Name##FromArray(TArray<Type> InArray)	\
{	\
	return MakeGameplayAbilityTargetDataFromArray<FGameplayAbilityTargetData_##Name>(InArray);	\
}	\
\
TArray<FGameplayAbilityTargetData_##Name> Class::GetTargetDataWith##Name(const FGameplayAbilityTargetDataHandle& TargetDataHandle)	\
{	\
	return GetGameplayAbilityTargetData<FGameplayAbilityTargetData_##Name>(TargetDataHandle);	\
}	\
\
TArray<FGameplayAbilityTargetData_##Name> Class::GetTargetDataWith##Name##ByEffectContext(const FGameplayEffectContextHandle& EffectContextHandle)	\
{	\
	return GetGameplayAbilityTargetDataByEffectContext<FGameplayAbilityTargetData_##Name>(EffectContextHandle);	\
}
#endif


/** Blueprint library for ability system. Many of these functions are useful to call from native as well */
UCLASS()
class STUDIOGAMEPLAYABILITIES_API UAbilitySystemBlueprintLibraryEx : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithBoolean(const bool& InValue);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithBooleanFromArray(TArray<bool> InArray);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Boolean> GetTargetDataWithBoolean(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Boolean> GetTargetDataWithBooleanByEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithFloat(const float& InValue);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithFloatFromArray(TArray<float> InArray);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Float> GetTargetDataWithFloat(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Float> GetTargetDataWithFloatByEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithInt32(const int32& InValue);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithInt32FromArray(TArray<int32> InArray);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Int32> GetTargetDataWithInt32(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Int32> GetTargetDataWithInt32ByEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithVector(const FVector& InValue);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithVectorFromArray(TArray<FVector> InArray);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Vector> GetTargetDataWithVector(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Vector> GetTargetDataWithVectorByEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithRotator(const FRotator& InValue);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithRotatorFromArray(TArray<FRotator> InArray);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Rotator> GetTargetDataWithRotator(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Rotator> GetTargetDataWithRotatorByEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithTransform(const FTransform& InValue);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithTransformFromArray(TArray<FTransform> InArray);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Transform> GetTargetDataWithTransform(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_Transform> GetTargetDataWithTransformByEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithHitResult(const FHitResult& InValue);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle MakeTargetDataWithHitResultFromArray(TArray<FHitResult> InArray);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_HitResult> GetTargetDataWithHitResult(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static TArray<FGameplayAbilityTargetData_HitResult> GetTargetDataWithHitResultByEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);

	/** Checks if spec has any effects */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveEffects(const FGameplayEffectContainerSpec& ContainerSpec);

	/** Checks if spec has any targets */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveTargets(const FGameplayEffectContainerSpec& ContainerSpec);
	
	/** Adds targets to a copy of the passed in effect container spec and returns it */
	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
	static FGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(const FGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/** Applies container spec that was made from an ability */
	UFUNCTION(BlueprintCallable, Category = Ability)
	static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FGameplayEffectContainerSpec& ContainerSpec);
};