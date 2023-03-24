// Copyright 2019 CQUnreal, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "Camera/CameraTypes.h"
#include "Camera/CameraModifier.h"
#include "CameraModifier_HitMesh.generated.h"


USTRUCT()
struct FDamagedMask
{
	GENERATED_USTRUCT_BODY()

	FDamagedMask()
		: Explain(TEXT(""))
		, LowerLimitAngle(0.f)
		, UpperLimitAngle(0.f)
		, Transform(FTransform::Identity)
		, Mesh(nullptr)
	{
		
	}

	UPROPERTY(EditAnywhere, Category = Damaged)
	FString Explain;

	UPROPERTY(EditAnywhere, Category = Damaged)
	float LowerLimitAngle;

	UPROPERTY(EditAnywhere, Category = Damaged)
	float UpperLimitAngle;

	UPROPERTY(EditAnywhere, Category = Damaged)
	FTransform Transform;

	UPROPERTY(EditAnywhere, Category = Damaged)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, Category = Damaged)
	TArray<UMaterialInterface*> Materials;
};

UCLASS(config = Camera)
class STUDIOGAME_API UCameraModifier_HitMesh : public UCameraModifier
{
	GENERATED_UCLASS_BODY()

	virtual void EnableModifier() override;
	virtual void DisableModifier(bool bImmediate = false) override;
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

public:
	UFUNCTION()
	void SetOtherMasks(const TArray<FDamagedMask>& Masks);

	UFUNCTION()
	void SetLastHitCauser(AActor* Causer);
	
	UFUNCTION()
	void PlayCameraTakeDamage();

	UPROPERTY()
	AActor* LastHitCauser;

protected:
	UPROPERTY(EditAnywhere, Category = CameraModifier_TakeDamage)
	float KeepTime;

	UPROPERTY(EditAnywhere, Category = CameraModifier_TakeDamage)
	FVector4 TargetContrast;

	UPROPERTY(EditAnywhere, Category = CameraModifier_TakeDamage)
	FVector2D EffectSize;

	UPROPERTY(EditAnywhere, Category = CameraModifier_TakeDamage)
	TArray<FDamagedMask> DamagedMasks;

	UPROPERTY()
	bool bEnableOtherMask;

	UPROPERTY()
	TArray<FDamagedMask> OtherMasks;

	UPROPERTY()
	float RemainingKeepTime;

	UPROPERTY()
	bool bDisable;

private:
	UPROPERTY()
	FTransform MaskTransform;

	UPROPERTY()
	class UStaticMeshComponent* MaskMesh;
};