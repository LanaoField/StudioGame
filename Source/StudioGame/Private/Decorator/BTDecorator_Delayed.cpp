// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Decorator/BTDecorator_Delayed.h"
#include "BehaviorTree/BehaviorTreeComponent.h"


UBTDecorator_Delayed::UBTDecorator_Delayed(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delay(5.f)
	, RandomDeviation(0.f)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	bNotifyTick = true;

	NodeName = "Delayed Condition";
}

bool UBTDecorator_Delayed::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	FBTDelayedMemory* DelayedMemory = (FBTDelayedMemory*)NodeMemory;
	return DelayedMemory->BecomeTime <= 0.f || OwnerComp.GetWorld()->TimeSeconds - DelayedMemory->BecomeTime < DelayedMemory->DelayTime;
}

void UBTDecorator_Delayed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTDelayedMemory* DelayedMemory = (FBTDelayedMemory*)NodeMemory;
	DelayedMemory->DelayTime = FMath::FRandRange(Delay - RandomDeviation, Delay + RandomDeviation);
	DelayedMemory->BecomeTime = OwnerComp.GetWorld()->TimeSeconds;
}

void UBTDecorator_Delayed::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTDelayedMemory* DelayedMemory = (FBTDelayedMemory*)NodeMemory;
	DelayedMemory->BecomeTime = 0.f;
}

void UBTDecorator_Delayed::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTDelayedMemory* DelayedMemory = (FBTDelayedMemory*)NodeMemory;
	if (OwnerComp.GetWorld()->TimeSeconds - DelayedMemory->BecomeTime >= DelayedMemory->DelayTime)
	{
		ConditionalFlowAbort(OwnerComp, EBTDecoratorAbortRequest::ConditionPassing);
	}
}

uint16 UBTDecorator_Delayed::GetInstanceMemorySize() const
{
	return sizeof(FBTDelayedMemory);
}