// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_GameplayEvent.generated.h"


class UAbilitySystemComponent;

/**
 * Send GameplayEvent
 */
UCLASS(editinlinenew, const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Gameplay Event"))
class STUDIOGAMEPLAYABILITIES_API UAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, Category = AnimNotify)
	FGameplayEventData Payload;

#if ENGINE_MAJOR_VERSION > 4
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
#else
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#endif
	virtual FString GetNotifyName_Implementation() const override;

	virtual void HandleGameplayEvent(FGameplayEventData& InPayload);
};
