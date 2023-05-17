// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShowLoginUIProxy.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "OnlineSubsystem.h"

UShowLoginUIProxy::UShowLoginUIProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, WorldContextObject(nullptr)
	, SubsystemName(NAME_None)
{

}

UShowLoginUIProxy* UShowLoginUIProxy::ShowLoginUI(UObject* WorldContextObject, APlayerController* InPlayerController, FName InSubsystemName)
{
	UShowLoginUIProxy* Proxy = NewObject<UShowLoginUIProxy>();
	Proxy->PlayerControllerWeakPtr = InPlayerController;
	Proxy->WorldContextObject = WorldContextObject;
	Proxy->SubsystemName = InSubsystemName;
	return Proxy;
}

void UShowLoginUIProxy::Activate()
{
	APlayerController* MyPlayerController = PlayerControllerWeakPtr.Get();
	if (!MyPlayerController)
	{
		FFrame::KismetExecutionMessage(TEXT("A player controller must be provided in order to show the external login UI."), ELogVerbosity::Warning);
		OnFailure.Broadcast(MyPlayerController);
		return;
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(SubsystemName);
	if (OnlineSubsystem == nullptr)
	{
		OnFailure.Broadcast(MyPlayerController);
		return;
	}

	IOnlineExternalUIPtr OnlineExternalUI = OnlineSubsystem->GetExternalUIInterface();
	if (!OnlineExternalUI.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("External UI not supported by the current online subsystem"), ELogVerbosity::Warning);
		OnFailure.Broadcast(MyPlayerController);
		return;
	}

	const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(MyPlayerController->Player);
	if (LocalPlayer == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Can only show login UI for local players"), ELogVerbosity::Warning);
		OnFailure.Broadcast(MyPlayerController);
		return;
	}

	const bool bWaitForDelegate = OnlineExternalUI->ShowLoginUI(LocalPlayer->GetControllerId(), false, false,
		FOnLoginUIClosedDelegate::CreateUObject(this, &UShowLoginUIProxy::OnShowLoginUICompleted));

	if (!bWaitForDelegate)
	{
		FFrame::KismetExecutionMessage(TEXT("The online subsystem couldn't show its login UI"), ELogVerbosity::Log);
		OnFailure.Broadcast(MyPlayerController);
	}
}

void UShowLoginUIProxy::OnShowLoginUICompleted(FUniqueNetIdPtr UniqueId, int LocalPlayerNum, const FOnlineError& Error)
{
	// Update the cached unique ID for the local player and the player state.
	APlayerController* MyPlayerController = PlayerControllerWeakPtr.Get();

	if (MyPlayerController != nullptr)
	{
	#if ENGINE_MAJOR_VERSION >= 5
		const FUniqueNetIdRepl UniqueNetIdRepl = FUniqueNetIdRepl(UniqueId);
		ULocalPlayer* LocalPlayer = MyPlayerController->GetLocalPlayer();
		if (LocalPlayer != nullptr)
		{
			LocalPlayer->SetCachedUniqueNetId(UniqueNetIdRepl);
		}

		if (MyPlayerController->PlayerState != nullptr)
		{
			MyPlayerController->PlayerState->SetUniqueId(UniqueNetIdRepl);
		}
	#else
		ULocalPlayer* LocalPlayer = MyPlayerController->GetLocalPlayer();
		if (LocalPlayer != nullptr)
		{
			LocalPlayer->SetCachedUniqueNetId(UniqueId);
		}

		if (MyPlayerController->PlayerState != nullptr)
		{
			MyPlayerController->PlayerState->SetUniqueId(UniqueId);
		}
	#endif
	}

	if (UniqueId.IsValid())
	{
		OnSuccess.Broadcast(MyPlayerController);
	}
	else
	{
		OnFailure.Broadcast(MyPlayerController);
	}

	SetReadyToDestroy();
}
