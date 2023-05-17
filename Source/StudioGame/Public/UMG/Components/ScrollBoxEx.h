// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "Components/ScrollBox.h"
#include "Slate/Widgets/SScrollBoxEx.h"
#include "ScrollBoxEx.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScrollEndEvent, float, CurrentOffset);

UCLASS()
class STUDIOGAME_API UScrollBoxEx : public UScrollBox
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
	FOnScrollEndEvent OnScrollEnd;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	float GetScrollOffsetOfEndEx() const;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	float GetScrollOffsetEx() const;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void ScrollWidgetIntoViewEx(UWidget* WidgetToFind, bool AnimateScroll = true, EDescendantScrollDestination ScrollDestination = EDescendantScrollDestination::IntoView, float Padding = 0);

#if ENGINE_MAJOR_VERSION >= 5

	virtual void SynchronizeProperties() override;

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interfaces

protected:

	TSharedPtr<class SScrollBoxEx> MyScrollBox;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	void SlateHandleScrollEnd(float CurrentOffset);

	// UPanelWidget
	virtual UClass* GetSlotClass() const override;

	virtual void OnSlotAdded(UPanelSlot* Slot) override;

	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// UPanelWidget End
#endif
};