// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "CatonFrameManager.generated.h"


USTRUCT()
struct FCatonFrame
{
	GENERATED_USTRUCT_BODY()

public:
	FCatonFrame()
		: CatonTime(0.f)
		, ShakeLocation(FVector::ZeroVector)
		, MeshRelativeLocation(FVector::ZeroVector)
	{

	}

	UPROPERTY()
	float CatonTime;

	UPROPERTY()
	FVector ShakeLocation;

	UPROPERTY()
	FVector MeshRelativeLocation;
};

UENUM(BlueprintType)
enum class ECatonFrame : uint8
{
	Normal,
	Override,
	Append
};

UENUM(BlueprintType)
enum class ECatonShake : uint8
{
	Normal,
	Override,
	Append
};

UCLASS()
class STUDIOGAME_API UCatonFrameManager : public UObject
#if ENGINE_MAJOR_VERSION >= 5
	, public FTSTickerObjectBase
#else
	, public FTickerObjectBase
#endif
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = CatonFrame, meta = (DisplayName = "AddCatonFrameActor"))
	static void AddActor(AActor* Actor, float CatonTime, ECatonFrame CatonFrame = ECatonFrame::Normal, ECatonShake CatonShake = ECatonShake::Normal, FVector ShakeLocation = FVector::ZeroVector);
	
	static UCatonFrameManager& Get();

	virtual bool Tick(float DeltaTime) override;
	virtual void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

protected:
	UPROPERTY(Transient)
	TMap<ACharacter*, FCatonFrame> CatonFrameActors;

private:
	static UCatonFrameManager* Instance;
};

