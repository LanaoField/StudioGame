// Copyright 2022 CQUnreal. All Rights Reserved.

#include "GameplayTargetType.h"
#include "GameFramework/Character.h"


void UGameplayTargetType::GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

void UGameplayTargetType_UseOwner::GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingCharacter);
}

void UGameplayTargetType_UseEventData::GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	if (FoundHitResult)
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if (EventData.Target)
	{
		#if ENGINE_MAJOR_VERSION >= 5
		OutActors.Add(const_cast<AActor*>(EventData.Target.Get()));
		#else
		OutActors.Add(const_cast<AActor*>(EventData.Target));
		#endif
	}
}

void UGameplayTargetType_SphereTrace::GetTargets_Implementation(ACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	if (TargetingCharacter == nullptr)
	{
		return;
	}

	UWorld* World = TargetingCharacter->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	FCollisionQueryParams Params;
	Params.bTraceComplex = bTraceComplex;
	Params.bReturnPhysicalMaterial = bReturnPhysicalMaterial;
	Params.AddIgnoredActor(TargetingCharacter);

	FCollisionObjectQueryParams ObjectParams;
	for (int32 Index = 0; Index < CollisionChannels.Num(); Index++)
	{
		ObjectParams.AddObjectTypesToQuery(CollisionChannels[Index]);
	}

	FVector Start = TargetingCharacter->GetActorTransform().TransformPosition(OffsetFromActor);
	FVector End = Start + TargetingCharacter->GetActorForwardVector() * TraceLength;

	World->SweepMultiByObjectType(OutHitResults, Start, End, FQuat::Identity, ObjectParams, FCollisionShape::MakeSphere(SphereRadius), Params);
}