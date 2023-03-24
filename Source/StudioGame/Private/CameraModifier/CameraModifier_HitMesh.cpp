// Copyright 2019 CQUnreal, Inc. All Rights Reserved.

#include "CameraModifier/CameraModifier_HitMesh.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Character.h"

UCameraModifier_HitMesh::UCameraModifier_HitMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AlphaInTime = 0.5f;
	AlphaOutTime = 0.5f;
	KeepTime = 1.f;
	EffectSize = FVector2D(1024, 1024);

	TargetContrast = FVector4(1.f, 1.f, 1.f, 1.f);
	bDisabled = true;
}

void UCameraModifier_HitMesh::SetOtherMasks(const TArray<FDamagedMask>& Masks)
{
	bEnableOtherMask = true;
	OtherMasks = Masks;
}

void UCameraModifier_HitMesh::SetLastHitCauser(AActor* Causer)
{
	LastHitCauser = Causer;
}

void UCameraModifier_HitMesh::PlayCameraTakeDamage()
{
	EnableModifier();
}

void UCameraModifier_HitMesh::EnableModifier()
{
	Super::EnableModifier();

	RemainingKeepTime = KeepTime;

	APlayerController* PlayerController = CameraOwner ? CameraOwner->GetOwningPlayerController() : nullptr;
	ACharacter* Pawn = PlayerController ? Cast<ACharacter>(PlayerController->GetPawn()) : nullptr;
	if (CameraOwner != nullptr && Pawn != nullptr)
	{
		if (LastHitCauser != nullptr)
		{
			const FTransform CameraTransform = FTransform(CameraOwner->GetCameraRotation(), CameraOwner->GetCameraLocation());
			const FTransform CauserTransform = LastHitCauser->GetActorTransform();
			const FTransform RelativeTransform = CameraTransform.GetRelativeTransform(CauserTransform);

			const float HitAngle = RelativeTransform.Rotator().Yaw;

			//判断是否有其他的Masks,若拥有则替换掉默认的Masks
			TArray<FDamagedMask> CarmeraMasks = DamagedMasks;

			if (bEnableOtherMask)
			{
				CarmeraMasks = OtherMasks;
			}

			for (const FDamagedMask& DamagedMask : CarmeraMasks)
			{
				if (HitAngle >= DamagedMask.LowerLimitAngle && HitAngle < DamagedMask.UpperLimitAngle)
				{
					MaskMesh = NewObject<UStaticMeshComponent>(Pawn, TEXT("MaskMesh"));
					if (MaskMesh != nullptr)
					{
						Pawn->AddOwnedComponent(MaskMesh);

						MaskMesh->CastShadow = false;
						MaskMesh->bReceivesDecals = false;
						MaskMesh->SetOnlyOwnerSee(true);
						MaskMesh->SetOwnerNoSee(false);
						MaskMesh->RegisterComponentWithWorld(GetWorld());
						MaskMesh->SetMobility(EComponentMobility::Movable);
						MaskMesh->AttachToComponent(Pawn->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
						MaskMesh->SetCollisionProfileName("NoCollision");
						MaskMesh->SetStaticMesh(DamagedMask.Mesh);
						for (int32 Index = 0; Index < DamagedMask.Materials.Num(); Index++)
						{
							MaskMesh->SetMaterial(Index, DamagedMask.Materials[Index]);
						}

						MaskTransform = DamagedMask.Transform;
					}
					break;
				}
			}

			//UE_LOG(LogTemp, Log, TEXT("HitAngle [%.2f]"), HitAngle);
		}
	}
}

void UCameraModifier_HitMesh::DisableModifier(bool bImmediate)
{
	Super::DisableModifier(bImmediate);

	if (MaskMesh != nullptr)
	{
		MaskMesh->SetHiddenInGame(true);
		MaskMesh->DestroyComponent(true);
		MaskMesh = nullptr;

		bEnableOtherMask = false;
		OtherMasks.Empty();
	}
}

bool UCameraModifier_HitMesh::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	if (CameraOwner)
	{
		FPostProcessSettings PPSettings;
		PPSettings.bOverride_ColorContrast = true;
		PPSettings.ColorContrast = FMath::Lerp<FVector4>(FVector4(1.f, 1.f, 1.f, 1.f), TargetContrast, Alpha);
		CameraOwner->AddCachedPPBlend(PPSettings, 1.f);
	}

	if (IsValid(MaskMesh))
	{
		const FTransform CameraTransform = FTransform(CameraOwner->GetCameraRotation(), CameraOwner->GetCameraLocation());
		const FTransform NewTransform = FTransform(CameraTransform.TransformRotation(MaskTransform.GetRotation()), CameraTransform.TransformPosition(MaskTransform.GetLocation()), MaskTransform.GetScale3D());

		MaskMesh->SetWorldTransform(NewTransform);
	}

	RemainingKeepTime -= DeltaTime;
	if (RemainingKeepTime < 0.f && (!bDisabled && !bPendingDisable))
	{
		DisableModifier();
	}

	return false;
}