// Copyright 2019 CQUnreal, Inc. All Rights Reserved.


#include "CameraModifier/CameraModifier_HitParticle.h"
#include "Camera/PlayerCameraManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UCameraModifier_HitParticle::UCameraModifier_HitParticle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AlphaInTime = 0.5f;
	AlphaOutTime = 0.5f;
	bDisabled = true;
}

void UCameraModifier_HitParticle::EnableModifier()
{
	Super::EnableModifier();

	bEnableModifier = true;
	CurrentTime = KeepTime;

	MaskParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ModifieShowParticle, CameraOwner->GetActorTransform());
	if (CameraOwner != nullptr && MaskParticle != nullptr)
	{
		MaskParticle->CastShadow = false;
		MaskParticle->bReceivesDecals = false;
		MaskParticle->bAutoDestroy = true;
		MaskParticle->SetOwnerNoSee(false);
		MaskParticle->SetMobility(EComponentMobility::Movable);
		MaskParticle->AttachToComponent(CameraOwner->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
}

void UCameraModifier_HitParticle::DisableModifier(bool bImmediate)
{
	Super::DisableModifier(bImmediate);

	if (IsValid(MaskParticle))
	{
		MaskParticle->SetHiddenInGame(true);
		MaskParticle->DestroyComponent(true);
		MaskParticle = nullptr;
	}

	bEnableModifier = false;
}

bool UCameraModifier_HitParticle::ModifyCamera(float DeltaTime, FMinimalViewInfo & InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	if (IsValid(MaskParticle))
	{
		const FTransform CameraTransform = FTransform(CameraOwner->GetCameraRotation(), CameraOwner->GetCameraLocation());
		const FTransform NewTransform = FTransform(CameraTransform.TransformRotation(ModifieShowTransform.GetRotation()), CameraTransform.TransformPosition(ModifieShowTransform.GetLocation()), ModifieShowTransform.GetScale3D());
		MaskParticle->SetRelativeTransform(NewTransform);
	}

	CurrentTime -= DeltaTime;
	if ((CurrentTime < 0.f || !IsValid(MaskParticle)) && (!bDisabled && !bPendingDisable))
	{
		DisableModifier();
	}

	return false;
}