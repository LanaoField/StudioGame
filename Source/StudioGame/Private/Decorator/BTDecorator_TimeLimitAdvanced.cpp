// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Decorator/BTDecorator_TimeLimitAdvanced.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"


UBTDecorator_TimeLimitAdvanced::UBTDecorator_TimeLimitAdvanced(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "TimeLimitAdvanced";
	bNotifyActivation = true;
	bNotifyTick = true;
	bTickIntervals = true;

	// time limit always abort current branch
	bAllowAbortLowerPri = false;
	bAllowAbortNone = false;
	FlowAbortMode = EBTFlowAbortMode::Self;

	TimeBlackboardKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_TimeLimitAdvanced, TimeBlackboardKey));
}

void UBTDecorator_TimeLimitAdvanced::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	uint8* RawMemory = SearchData.OwnerComp.GetNodeMemory(this, SearchData.OwnerComp.FindInstanceContainingNode(this));
	if (RawMemory)
	{
		float TimeLimit = 0.f;
		const UBlackboardComponent* MyBlackboard = SearchData.OwnerComp.GetBlackboardComponent();
		if (MyBlackboard && !TimeBlackboardKey.IsNone())
		{
			TimeLimit = MyBlackboard->GetValueAsFloat(TimeBlackboardKey.SelectedKeyName);
		}
		else
		{
			UE_LOG(LogTemp, Log,TEXT("TimeLimitAdvanced Get Time From BBKey Failed!"));
		}

		FBTAuxiliaryMemory* DecoratorMemory = GetSpecialNodeMemory<FBTAuxiliaryMemory>(RawMemory);
		DecoratorMemory->NextTickRemainingTime = TimeLimit;
		DecoratorMemory->AccumulatedDeltaTime = 0.0f;
	}
}

void UBTDecorator_TimeLimitAdvanced::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (bSuccesOnTimeLimit)
	{
		OwnerComp.RequestExecution(EBTNodeResult::Succeeded);
	}
	else
	{
		OwnerComp.RequestExecution(this);
	}
}

FString UBTDecorator_TimeLimitAdvanced::GetStaticDescription() const
{
	
	return FString::Printf(TEXT("%s: %s after %s "), *Super::GetStaticDescription(),
		*UBehaviorTreeTypes::DescribeNodeResult(bSuccesOnTimeLimit ? EBTNodeResult::Succeeded : EBTNodeResult::Failed), *TimeBlackboardKey.SelectedKeyName.ToString());
}

void UBTDecorator_TimeLimitAdvanced::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	FBTAuxiliaryMemory* DecoratorMemory = GetSpecialNodeMemory<FBTAuxiliaryMemory>(NodeMemory);
	if (OwnerComp.GetWorld())
	{
		const float TimeLeft = DecoratorMemory->NextTickRemainingTime > 0 ? DecoratorMemory->NextTickRemainingTime : 0;
		Values.Add(FString::Printf(TEXT("%s in %ss"),
			*UBehaviorTreeTypes::DescribeNodeResult(bSuccesOnTimeLimit ? EBTNodeResult::Succeeded : EBTNodeResult::Failed),
			*FString::SanitizeFloat(TimeLeft)));
	}
}

#if WITH_EDITOR

FName UBTDecorator_TimeLimitAdvanced::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Decorator.TimeLimit.Icon");
}

#endif	// WITH_EDITOR
