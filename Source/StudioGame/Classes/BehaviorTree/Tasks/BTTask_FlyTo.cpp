// Copyright 2018 CQUnreal, Inc. All Rights Reserved.

#include "BTTask_FlyTo.h"
#include "Interfaces/FlyingAIInterface.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"


UBTTask_FlyTo::UBTTask_FlyTo(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, FlyingTime(0.f)
{
	NodeName = "Fly To";

#if ENGINE_MAJOR_VERSION < 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 2)
	bUseGameplayTasks = GET_AI_CONFIG_VAR(bEnableBTAITasks);
	bNotifyTick = !bUseGameplayTasks;
#endif
	bNotifyTaskFinished = true;
	AcceptableRadius = GET_AI_CONFIG_VAR(AcceptanceRadius);

	// accept only actors and vectors
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FlyTo, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FlyTo, BlackboardKey));
}

EBTNodeResult::Type UBTTask_FlyTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* MyController = OwnerComp.GetAIOwner();
	APawn* MyPawn = MyController ? MyController->GetPawn() : nullptr;

	if (MyPawn && MyPawn->GetClass()->ImplementsInterface(UFlyingAIInterface::StaticClass()))
	{
		FlyingTime = 0.f;

		IFlyingAIInterface::Execute_OnLocomotionBegin(MyPawn);

		return EBTNodeResult::InProgress;
	}
	else
	{
		UE_VLOG(MyController, LogBehaviorTree, Log, TEXT("This bot doesn't inherit from the IFlyingAIInterface. It doesn't use FlyTo"));

		return EBTNodeResult::Failed;
	}
}

EBTNodeResult::Type UBTTask_FlyTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	AAIController* MyController = OwnerComp.GetAIOwner();
	APawn* MyPawn = MyController ? MyController->GetPawn() : nullptr;
	if (MyPawn != nullptr)
	{
		IFlyingAIInterface::Execute_OnLocomotionAbort(MyPawn);
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_FlyTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	AAIController* MyController = OwnerComp.GetAIOwner();
	APawn* MyPawn = MyController ? MyController->GetPawn() : nullptr;
	if (MyPawn != nullptr)
	{
		IFlyingAIInterface::Execute_OnLocomotionEnd(MyPawn, TaskResult == EBTNodeResult::Succeeded);
	}
}

void UBTTask_FlyTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (TimeOut > 0.f && FlyingTime >= TimeOut)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	FlyingTime += DeltaSeconds;

	AAIController* MyController = OwnerComp.GetAIOwner();
	APawn* MyPawn = MyController ? MyController->GetPawn() : nullptr;
	if (MyPawn == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	if (ensure(MyBlackboard))
	{
		FVector TargetLocation = MyPawn->GetActorLocation();
		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			AActor* TargetActor = Cast<AActor>(KeyValue);
			if (TargetActor)
			{
				TargetLocation = TargetActor->GetActorLocation();
			}
			else
			{
				UE_VLOG(MyController, LogBehaviorTree, Warning, TEXT("UBTTask_FlyTo::ExecuteTask tried to go to actor while BB %s entry was empty"), *BlackboardKey.SelectedKeyName.ToString());
			}
			
		}
		else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		}

		if (FVector::DistSquared(MyPawn->GetActorLocation(), TargetLocation) <= AcceptableRadius * AcceptableRadius)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}

		FVector TargetDirection = (TargetLocation - MyPawn->GetActorLocation()).GetSafeNormal();

		IFlyingAIInterface::Execute_AddMovementInputFlying(MyPawn, TargetDirection, 1.f);
	}
}
