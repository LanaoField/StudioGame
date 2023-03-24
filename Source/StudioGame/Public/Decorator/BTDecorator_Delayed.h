// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Delayed.generated.h"

struct FBTDelayedMemory
{
	float DelayTime;

	float BecomeTime;
};

/**
 * UBTDecorator_Delayed
 */
UCLASS(HideCategories = (Condition))
class STUDIOGAME_API UBTDecorator_Delayed : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	/** called when execution flow controller becomes active */
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** called when execution flow controller becomes inactive */
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** size of instance memory */
	virtual uint16 GetInstanceMemorySize() const override;

public:
	UPROPERTY(EditAnywhere, Category = Delayed)
	float Delay;

	UPROPERTY(EditAnywhere, Category = Delayed)
	float RandomDeviation;
};
