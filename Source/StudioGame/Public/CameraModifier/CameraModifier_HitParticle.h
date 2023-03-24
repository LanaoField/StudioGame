// Copyright 2019 CQUnreal, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "CameraModifier_HitParticle.generated.h"

/**
 * 
 */
UCLASS()
class STUDIOGAME_API UCameraModifier_HitParticle : public UCameraModifier
{
	GENERATED_UCLASS_BODY()
	

	virtual void EnableModifier() override;
	virtual void DisableModifier(bool bImmediate = false) override;
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

protected:
	/* ������ʾλ�� */
	UPROPERTY(EditAnywhere, Category = Config)
	FTransform ModifieShowTransform;

	/* ����������Ч */
	UPROPERTY(EditAnywhere, Category = Config)
	UParticleSystem* ModifieShowParticle;

	/* �Ƿ��������� */
	uint8 bEnableModifier : 1;

	/* ����ʱ�� */
	UPROPERTY(EditAnywhere, Category = Config)
	float KeepTime;

	float CurrentTime;

private:
	UPROPERTY()
	class UParticleSystemComponent* MaskParticle;
};
