// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Decorator/BTDecorator_DistanceTo.h"
#include "Misc/AssertionMacros.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"


UBTDecorator_DistanceTo::UBTDecorator_DistanceTo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MinDistance(0.f)
	, MaxDistance(0.f)
	, Delay(0.5f)
	, DelayDeviation(0.f)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	bNotifyTick = true;

	NodeName = "DistanceTo Condition";

	Target.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_DistanceTo, Target));
	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_DistanceTo, Target), AActor::StaticClass());
}

void UBTDecorator_DistanceTo::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		Target.ResolveSelectedKey(*BBAsset);
	}
}

bool UBTDecorator_DistanceTo::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	APawn* Pawn = AIController ? Cast<APawn>(AIController->GetPawn()) : nullptr;

	UBlackboardComponent* MyBlackboard = Cast<UBlackboardComponent>(OwnerComp.GetBlackboardComponent());

	float DistanceTo = 0.f;

	if (Target.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(Target.GetSelectedKeyID());
		AActor* TargetActor = Cast<AActor>(KeyValue);

		DistanceTo = TargetActor ? FVector::Distance(TargetActor->GetActorLocation(), Pawn->GetActorLocation()) : 0.f;
	}
	else if (Target.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		const FVector TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(Target.GetSelectedKeyID());

		DistanceTo = FVector::Distance(TargetLocation, Pawn->GetActorLocation());
	}

	if ((MinDistance <= 0.f || MinDistance >= DistanceTo) && (MaxDistance <= 0.f || MaxDistance <= DistanceTo))
	{
		FBTDistanceToMemory* DistanceToMemory = (FBTDistanceToMemory*)NodeMemory;
		return DistanceToMemory && DistanceToMemory->ElapsedTime <= DistanceToMemory->DelayTime;
	}


	return false;
}

void UBTDecorator_DistanceTo::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTDistanceToMemory* DistanceToMemory = (FBTDistanceToMemory*)NodeMemory;
	DistanceToMemory->ElapsedTime = 0.f;
	DistanceToMemory->DelayTime = FMath::FRandRange(Delay - DelayDeviation, Delay + DelayDeviation);
}

void UBTDecorator_DistanceTo::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTDistanceToMemory* DistanceToMemory = (FBTDistanceToMemory*)NodeMemory;
	DistanceToMemory->ElapsedTime = 0.f;
	DistanceToMemory->DelayTime = 0.f;
}

void UBTDecorator_DistanceTo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	APawn* Pawn = AIController ? Cast<APawn>(AIController->GetPawn()) : nullptr;

	UBlackboardComponent* MyBlackboard = Cast<UBlackboardComponent>(OwnerComp.GetBlackboardComponent());

	float DistanceTo = 0.f;

	if (Target.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(Target.GetSelectedKeyID());
		AActor* TargetActor = Cast<AActor>(KeyValue);

		DistanceTo = TargetActor ? FVector::Distance(TargetActor->GetActorLocation(), Pawn->GetActorLocation()) : 0.f;
	}
	else if (Target.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		const FVector TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(Target.GetSelectedKeyID());

		DistanceTo = FVector::Distance(TargetLocation, Pawn->GetActorLocation());
	}

	FBTDistanceToMemory* DistanceToMemory = (FBTDistanceToMemory*)NodeMemory;


	if ((MinDistance <= 0.f || MinDistance < DistanceTo) && (MaxDistance <= 0.f || MaxDistance > DistanceTo))
	{
		DistanceToMemory->ElapsedTime += DeltaSeconds;
	}
	else
	{
		DistanceToMemory->ElapsedTime = 0.f;
	}



	if (DistanceToMemory->ElapsedTime > DistanceToMemory->DelayTime)
	{
		ConditionalFlowAbort(OwnerComp, EBTDecoratorAbortRequest::ConditionPassing);
	}
}

uint16 UBTDecorator_DistanceTo::GetInstanceMemorySize() const
{
	return sizeof(FBTDistanceToMemory);
}