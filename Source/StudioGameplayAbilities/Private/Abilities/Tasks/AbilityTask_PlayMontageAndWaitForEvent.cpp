// Copyright Epic Games, Inc. All Rights Reserved.

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWaitForEvent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Animation/AnimInstance.h"


UAbilityTask_PlayMontageAndWaitForEvent::UAbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
	bStopWhenAbilityCancelled = true;
	bEndTaskWhenMontageEnded = true;
	bAlwaysBroadcastAbilityTaskDelegates = false;
}

void UAbilityTask_PlayMontageAndWaitForEvent::Activation()
{
	if (UGameplayTasksComponent* TasksPtr = TasksComponent.Get())
	{
		if (RequiresPriorityOrResourceManagement() == false)
		{
			TaskState = EGameplayTaskState::Active;

			Activate();
		}
		else
		{
			TasksPtr->AddTaskReadyForActivation(*this);
		}
	}
	else
	{
		EndTask();
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			// Reset AnimRootMotionTranslationScale
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority || (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}
		}
	}

	if (bInterrupted)
	{
		OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	else
	{
		OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	// TODO: Merge this fix back to engine, it was calling the wrong callback
	if (bStopWhenAbilityCancelled)
	{
		StopPlayingMontage();
	}

	// Let the BP handle the interrupt as well
	if (bAlwaysBroadcastAbilityTaskDelegates || ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (bAlwaysBroadcastAbilityTaskDelegates || ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	if (bEndTaskWhenMontageEnded)
	{
		EndTask();
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (bAlwaysBroadcastAbilityTaskDelegates || ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

UAbilityTask_PlayMontageAndWaitForEvent* UAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility,
	FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds, bool bStopWhenAbilityCancelled, bool bEndTaskWhenMontageEnded, bool bAlwaysBroadcastAbilityTaskDelegates, float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UAbilityTask_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UAbilityTask_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->EventTags = EventTags;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->bStopWhenAbilityCancelled = bStopWhenAbilityCancelled;
	MyObj->bEndTaskWhenMontageEnded = bEndTaskWhenMontageEnded;
	MyObj->bAlwaysBroadcastAbilityTaskDelegates = bAlwaysBroadcastAbilityTaskDelegates;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;

	return MyObj;
}

void UAbilityTask_PlayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;

#if ENGINE_MAJOR_VERSION > 4
	if (AbilitySystemComponent.IsValid())
#else
	if (AbilitySystemComponent != nullptr)
#endif
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			// Bind to event callback
			if (!EventHandle.IsValid())
			{
				EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent));
			}			

			if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (bAlwaysBroadcastAbilityTaskDelegates == false && ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				if (!CancelledHandle.IsValid())
				{
					CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled);
				}

				BlendingOutDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
					(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (bAlwaysBroadcastAbilityTaskDelegates || ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UAbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
#if ENGINE_MAJOR_VERSION > 4
	check(AbilitySystemComponent.IsValid());
#else
	check(AbilitySystemComponent);
#endif

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

#if ENGINE_MAJOR_VERSION > 4
	if (AbilitySystemComponent.IsValid())
#else
	if (AbilitySystemComponent != nullptr)
#endif
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);

}

bool UAbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
#if ENGINE_MAJOR_VERSION > 4
	if (AbilitySystemComponent.IsValid() && Ability)
#else
	if (AbilitySystemComponent && Ability)
#endif
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

FString UAbilityTask_PlayMontageAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}
