// Copyright Epic Games, Inc. All Rights Reserved.

#include "Manager/CatonFrameManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UCatonFrameManager* UCatonFrameManager::Instance = nullptr;

UCatonFrameManager::UCatonFrameManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UCatonFrameManager::OnWorldCleanup);
}

UCatonFrameManager& UCatonFrameManager::Get()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UCatonFrameManager>();
		Instance->AddToRoot();
	}

	return *Instance;
}

void UCatonFrameManager::AddActor(AActor* Actor, float CatonTime, ECatonFrame CatonFrame, ECatonShake CatonShake, FVector ShakeLocation)
{
	ACharacter* Character = Cast<ACharacter>(Actor);
	if (Character != nullptr && CatonTime > 0.f)
	{
		UCatonFrameManager& CatonFrameManager = UCatonFrameManager::Get();

		FCatonFrame* CatonFramePtr = CatonFrameManager.CatonFrameActors.Find(Character);
		if (CatonFramePtr == nullptr)
		{
			FCatonFrame NewCatonFrame;
			NewCatonFrame.CatonTime = CatonTime;
			NewCatonFrame.MeshRelativeLocation = Character->GetMesh()->GetRelativeLocation();
			NewCatonFrame.ShakeLocation = ShakeLocation;

			CatonFramePtr = &(CatonFrameManager.CatonFrameActors.Add(Character, NewCatonFrame));

			Character->GetMesh()->bPauseAnims = true;
			Character->GetMesh()->SetComponentTickEnabled(false);
			Character->GetCharacterMovement()->SetComponentTickEnabled(false);
		}
		else
		{
			if (CatonFrame == ECatonFrame::Append)
			{
				CatonFramePtr->CatonTime += CatonTime;
			}
			else if (CatonFrame == ECatonFrame::Override)
			{
				CatonFramePtr->CatonTime = CatonTime;
			}

			if (CatonShake == ECatonShake::Append)
			{
				CatonFramePtr->CatonTime += CatonTime;
				CatonFramePtr->ShakeLocation += ShakeLocation;
			}
			else if (CatonShake == ECatonShake::Override)
			{
				CatonFramePtr->CatonTime = CatonTime;
				CatonFramePtr->ShakeLocation = ShakeLocation;
			}
		}
	}
}

bool UCatonFrameManager::Tick(float DeltaTime)
{
	TMap<ACharacter*, FCatonFrame> Temp = CatonFrameActors;
	for (TPair<ACharacter*, FCatonFrame>& Pair : Temp)
	{
		ACharacter* Character = Pair.Key;
		if (IsValid(Character))
		{
			FCatonFrame* CatonFramePtr = CatonFrameActors.Find(Character);
			check(CatonFramePtr);
			CatonFramePtr->CatonTime -= DeltaTime;
			CatonFramePtr->ShakeLocation *= -1.f;

			Character->GetMesh()->AddLocalOffset(CatonFramePtr->ShakeLocation);

			if (CatonFramePtr->CatonTime <= 0.f)
			{
				Character->GetMesh()->bPauseAnims = false;
				Character->GetMesh()->SetComponentTickEnabled(true);
				Character->GetMesh()->SetRelativeLocation(CatonFramePtr->MeshRelativeLocation);
				Character->GetCharacterMovement()->SetComponentTickEnabled(true);

				CatonFrameActors.Remove(Character);
			}
		}
		else
		{
			CatonFrameActors.Remove(Character);
		}
	}

	return true;
}

void UCatonFrameManager::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	CatonFrameActors.Empty();
}