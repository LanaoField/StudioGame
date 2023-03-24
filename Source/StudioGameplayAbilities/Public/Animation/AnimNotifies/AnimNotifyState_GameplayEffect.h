// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_GameplayEffect.generated.h"


class UAbilitySystemComponent;


/**
 * Sweep
 */
UCLASS(editinlinenew, const, hidecategories = Object, collapsecategories, meta = (DisplayName = "GameplayEffect"))
class STUDIOGAMEPLAYABILITIES_API UAnimNotifyState_GameplayEffect : public UAnimNotifyState
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	float Level;

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	TMap<FName, float> SetByCallerMagnitudeNames;

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	TMap<FGameplayTag, float> SetByCallerMagnitudeTags;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override;

	virtual void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

private:
	UPROPERTY(Transient)
	TMap<AActor*, FActiveGameplayEffectHandle> EffectHandles;
};
