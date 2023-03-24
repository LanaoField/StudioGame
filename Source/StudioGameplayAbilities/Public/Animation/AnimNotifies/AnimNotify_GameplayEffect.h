// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_GameplayEffect.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

/**
 * Send GameplayEffect
 */
UCLASS(editinlinenew, const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Gameplay Effect"))
class STUDIOGAMEPLAYABILITIES_API UAnimNotify_GameplayEffect : public UAnimNotify
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	float Level;

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	float Duration;

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	bool bLockDuration;

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	TMap<FName, float> SetByCallerMagnitudeNames;

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	TMap<FGameplayTag, float> SetByCallerMagnitudeTags;

#if ENGINE_MAJOR_VERSION > 4
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
#else
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#endif
	virtual FString GetNotifyName_Implementation() const override;
};
