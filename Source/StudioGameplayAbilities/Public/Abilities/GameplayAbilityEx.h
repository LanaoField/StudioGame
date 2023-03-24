// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypesEx.h"
#include "GameplayAbilityEx.generated.h"

/**
 * GameplayAbilityEx
 */
UCLASS()
class STUDIOGAMEPLAYABILITIES_API UGameplayAbilityEx : public UGameplayAbility
{
	GENERATED_UCLASS_BODY()

public:
	/** Input binding stub. */
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	/** Invoke a gameplay cue on the ability owner */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (GameplayTagFilter = "GameplayCue"), DisplayName = "Execute MultiHitResults EffectContext", meta = (ScriptName = "ExecuteGameplayCueWithTargetData"))
	virtual void K2_ExecuteGameplayCueWithTargetData(FGameplayTag GameplayCueTag, const FGameplayAbilityTargetDataHandle& TargetDataHandle, UObject* SourceObject = nullptr, AActor* Instigator = nullptr, AActor* EffectCauser = nullptr, FVector Origin = FVector::ZeroVector);

	/** Removes GameplayEffect from owner that match the given handle array */
	UFUNCTION(BlueprintCallable, Category = Ability, DisplayName = "RemoveGameplayEffectFromOwnerWithHandle", meta = (ScriptName = "RemoveGameplayEffectFromOwnerWithHandleArray"))
	void BP_RemoveGameplayEffectFromOwnerWithHandleArray(TArray<FActiveGameplayEffectHandle> HandleArray, int32 StacksToRemove = -1);
	
	/** Apply a previously created gameplay effect class to the owner of this ability */
	UFUNCTION(BlueprintCallable, Category = Ability, DisplayName = "ApplyGameplayEffectClassToOwner", meta = (ScriptName = "ApplyGameplayEffectClassToOwner"))
	FActiveGameplayEffectHandle K2_ApplyGameplayEffectClassToOwner(TSubclassOf<UGameplayEffect> GameplayEffectClass);

	/** Apply a previously created gameplay effect class to a target */
	UFUNCTION(BlueprintCallable, Category = Ability, DisplayName = "ApplyGameplayEffectClassToTarget", meta = (ScriptName = "ApplyGameplayEffectClassToTarget"))
	TArray<FActiveGameplayEffectHandle> K2_ApplyGameplayEffectClassToTarget(TSubclassOf<UGameplayEffect> GameplayEffectClass, FGameplayAbilityTargetDataHandle TargetData);

public:
	/** Skill tag Removal */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnCompleteAbility", meta = (ScriptName = "OnCompleteAbility"))
	void K2_OnCompleteAbility(bool bWasCancelled);

	/** Blueprint input binding stub. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "InputPressed", meta = (ScriptName = "InputPressed"))
	void K2_InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "InputReleased", meta = (ScriptName = "InputReleased"))
	void K2_InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

public:
	/** Map of gameplay tags to gameplay effect containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffects)
	TMap<FGameplayTag, FGameplayEffectContainer> EffectContainerMap;

	/** Make gameplay effect container spec to be applied later, using the passed in container */
	UFUNCTION(BlueprintCallable, Category = Ability, meta=(AutoCreateRefTerm = "EventData"))
	virtual FGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Applies a gameplay effect container spec that was previously created */
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FGameplayEffectContainerSpec& ContainerSpec);

	/** Applies a gameplay effect container, by creating and then applying the spec */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);
};
