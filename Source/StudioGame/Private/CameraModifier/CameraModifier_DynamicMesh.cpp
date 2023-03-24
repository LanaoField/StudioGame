// Copyright 2019 CQUnreal, Inc. All Rights Reserved.


#include "CameraModifier/CameraModifier_DynamicMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"

UCameraModifier_DynamicMesh::UCameraModifier_DynamicMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AlphaInTime = 0.5f;
	AlphaOutTime = 0.5f;
	KeepTime = 1.f;
	bDisabled = true;
	bEnableKeepTime = true;
}

void UCameraModifier_DynamicMesh::EnableModifier()
{
	Super::EnableModifier();

	ContinuedKeepTime = 0.f;

	APlayerController* PlayerController = CameraOwner ? CameraOwner->GetOwningPlayerController() : nullptr;
	ACharacter* Pawn = PlayerController ? Cast<ACharacter>(PlayerController->GetPawn()) : nullptr;
	if (Pawn != nullptr)
	{
		MaskMesh = Cast<UStaticMeshComponent>(Pawn->AddComponentByClass(UStaticMeshComponent::StaticClass(), true, Pawn->GetActorTransform(), true));
		if (MaskMesh != nullptr)
		{
			MaskMesh->CastShadow = false;
			MaskMesh->bReceivesDecals = false;
			//MaskMesh->SetOnlyOwnerSee(true);
			MaskMesh->SetOwnerNoSee(false);
			MaskMesh->SetMobility(EComponentMobility::Movable);
			MaskMesh->SetCollisionProfileName(TEXT("NoCollision"));
			MaskMesh->SetStaticMesh(ShowMesh);
		}
	}
}

void UCameraModifier_DynamicMesh::DisableModifier(bool bImmediate)
{
	Super::DisableModifier(bImmediate);

	if (MaskMesh != nullptr)
	{
		MaskMesh->SetHiddenInGame(true);
		MaskMesh->DestroyComponent(true);
		MaskMesh = nullptr;
	}
}

bool UCameraModifier_DynamicMesh::ModifyCamera(float DeltaTime, FMinimalViewInfo & InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	ContinuedKeepTime += DeltaTime;

	if (IsValid(MaskMesh))
	{
		/* 设置物体的位置 */
		const FTransform CameraTransform = FTransform(CameraOwner->GetCameraRotation(), CameraOwner->GetCameraLocation());
		const FTransform NewTransform = FTransform(CameraTransform.TransformRotation(ShowTransform.GetRotation()), CameraTransform.TransformPosition(ShowTransform.GetLocation()), ShowTransform.GetScale3D());
		MaskMesh->SetWorldTransform(NewTransform);

		/* 修改物体材质动态参数 */
		for (FMaterialModifierParameter MaterialModifier : MaterialModifierParameterList)
		{
			if (MaterialModifier.bEnableChange && ContinuedKeepTime >= MaterialModifier.StartTime && ContinuedKeepTime <= MaterialModifier.EndTime)
			{
				for (int32 Index = 0; Index < MaskMesh->GetMaterials().Num(); Index++)
				{
					UMaterialInterface* MaterialInstance = MaskMesh->GetMaterial(Index);
					UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MaterialInstance);
					if (MaterialInstance && !MID)
					{
						// Create and set the dynamic material instance.
						MID = UMaterialInstanceDynamic::Create(MaterialInstance, this);
						MaskMesh->SetMaterial(Index, MID);
					}

					if (MID != nullptr)
					{
						const float UpdateValueCountTime = MaterialModifier.EndTime - MaterialModifier.StartTime;
						const float UpdateValueCurrentTime = ContinuedKeepTime - MaterialModifier.StartTime;

						float NewDynamicMaterValue = FMath::Lerp<float>(MaterialModifier.ParameterInitialValue, MaterialModifier.ParameterTargetValue, UpdateValueCurrentTime / UpdateValueCountTime);

						MID->SetScalarParameterValue(MaterialModifier.ParameterName, NewDynamicMaterValue);
					}
				}
			}
		}
	}

	for (FSoundModifierParameter& SoundModifier : SoundModifierParameterList)
	{
		if (SoundModifier.SoundResources && !SoundModifier.AudioComponent && ContinuedKeepTime >= SoundModifier.StartPlayTime)
		{
			SoundModifier.AudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), SoundModifier.SoundResources);
		}

		if (SoundModifier.AudioComponent && SoundModifier.StopPlayTime >= 0.f && !SoundModifier.StopSoundTimerHandle.IsValid())
		{
			FTimerDelegate StopSoundTimerDeleget = FTimerDelegate::CreateLambda([=]() {
				if (SoundModifier.AudioComponent)
				{
					SoundModifier.AudioComponent->Stop();
				}
			});
			GetWorld()->GetTimerManager().SetTimer(SoundModifier.StopSoundTimerHandle, StopSoundTimerDeleget, SoundModifier.StopPlayTime, false);
		}
	}


	if (bEnableKeepTime && ContinuedKeepTime >= KeepTime && (!bDisabled && !bPendingDisable))
	{
		DisableModifier();
	}

	return false;
}