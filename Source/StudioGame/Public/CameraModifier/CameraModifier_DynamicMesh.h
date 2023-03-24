// Copyright 2019 CQUnreal, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "CameraModifier_DynamicMesh.generated.h"

UENUM(BlueprintType)
enum class EModifierType : uint8
{
	/* 无类型 */
	None,

	/* 死亡遮罩 */
	DeathMask,

	/* 死亡提示 */
	DeathTips,

	/* 复活遮罩 */
	ResuscitateMask,

	/* 复活提示 */
	ResuscitateTips
};

USTRUCT(BlueprintType)
struct FMaterialModifierParameter
{
	GENERATED_USTRUCT_BODY()

	FMaterialModifierParameter()
		: bEnableChange(true)
		, StartTime(0.f)
		, EndTime(0.f)
		, ParameterName("None")
		, ParameterInitialValue(0.f)
		, ParameterTargetValue(0.f)
	{

	}

	//是否启用动态材质变化
	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bEnableChange;

	//开始变化时间(Modifier启用后开始计时)
	UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "bEnableChange", ClampMin = 0.f))
	float StartTime;

	//停止变化时间(Modifier启用后开始计时)
	UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "bEnableChange", ClampMin = 0.f))
	float EndTime;

	//动态材质参数名称
	UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "bEnableChange"))
	FName ParameterName;

	//动态材质参数初始值
	UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "bEnableChange"))
	float ParameterInitialValue;

	//动态材质参数目标值
	UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "bEnableChange"))
	float ParameterTargetValue;
};

USTRUCT(BlueprintType)
struct FSoundModifierParameter
{
	GENERATED_USTRUCT_BODY()

	FSoundModifierParameter()
		: StartPlayTime(0.f)
		, StopPlayTime(-1.f)
		, SoundResources(nullptr)
		, AudioComponent(nullptr)
		, StopSoundTimerHandle()
	{

	}

	//开始播放音效时间(Modifier启用后开始计时)
	UPROPERTY(EditAnywhere, Category = Sound, meta = (ClampMin = 0.f))
	float StartPlayTime;

	//停止播放音效的时间(在音效开始播放后计时,若时间小于0则不启用)
	UPROPERTY(EditAnywhere, Category = Sound)
	float StopPlayTime;

	//播放音效资源
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* SoundResources;

	//记录播放音效的组件
	UPROPERTY()
	class UAudioComponent* AudioComponent;

	//停止声音计时器
	FTimerHandle StopSoundTimerHandle;
};

/**
 *
 */
UCLASS()
class STUDIOGAME_API UCameraModifier_DynamicMesh : public UCameraModifier
{
	GENERATED_UCLASS_BODY()

	virtual void EnableModifier() override;
	virtual void DisableModifier(bool bImmediate = false) override;
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

protected:
	//启用存在时间
	UPROPERTY(EditAnywhere, Category = Config)
	bool bEnableKeepTime;

	//最大存在时间
	UPROPERTY(EditAnywhere, Category = Config, meta = (EditCondition = "bEnableKeepTime", ClampMin = 0.f))
	float KeepTime;

	//当前存在时间
	UPROPERTY()
	float ContinuedKeepTime;

	//遮罩类型
	UPROPERTY(EditAnywhere, Category = Config)
	EModifierType CurrentModifierType;

	//Mesh偏移位置
	UPROPERTY(EditAnywhere, Category = Config)
	FTransform ShowTransform;

	//显示的Mesh
	UPROPERTY(EditAnywhere, Category = Config)
	UStaticMesh* ShowMesh;

	//变化动态材质参数列表
	UPROPERTY(EditAnywhere, Category = Modifier)
	TArray<FMaterialModifierParameter> MaterialModifierParameterList;

	//播放的声音列表
	UPROPERTY(EditAnywhere, Category = Modifier)
	TArray<FSoundModifierParameter> SoundModifierParameterList;

private:
	UPROPERTY()
	class UStaticMeshComponent* MaskMesh;
};
