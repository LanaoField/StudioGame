// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractiveInterface.generated.h"


UINTERFACE(MinimalAPI)
class UInteractiveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * �����ӿ���
 */
class STUDIOGAME_API IInteractiveInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = Interactive)
	void BeginFocus(class AActor* InteractiveActor, class UPrimitiveComponent* InteractiveComponent, class UPrimitiveComponent* TriggerComponent);

	UFUNCTION(BlueprintNativeEvent, Category = Interactive)
	void Focus(class AActor* InteractiveActor, class UPrimitiveComponent* InteractiveComponent, class UPrimitiveComponent* TriggerComponent);

	UFUNCTION(BlueprintNativeEvent, Category = Interactive)
	void EndFocus(class AActor* InteractiveActor, class UPrimitiveComponent* InteractiveComponent, class UPrimitiveComponent* TriggerComponent);

	UFUNCTION(BlueprintNativeEvent, Category = Interactive)
	void PresseInteraction(class AActor* InteractiveActor, class UPrimitiveComponent* InteractiveComponent, class UPrimitiveComponent* TriggerComponent, FKey Key);

	UFUNCTION(BlueprintNativeEvent, Category = Interactive)
	void ReleaseInteraction(class AActor* InteractiveActor, class UPrimitiveComponent* InteractiveComponent, class UPrimitiveComponent* TriggerComponent, FKey Key);
};
