// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "Components/ContentWidget.h"
#include "Widgets/Input/SButton.h"
#include "ActionButton.generated.h"

/**
 * UActionButton
 */
UCLASS()
class STUDIOGAME_API UActionButton : public UContentWidget
{
	GENERATED_UCLASS_BODY()

public:
	/** The template style asset, used to seed the mutable instance of the style. */
	UPROPERTY()
	USlateWidgetStyleAsset* Style_DEPRECATED;

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface

public:
	/** The button style used at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance", meta=( DisplayName="Style" ))
	FButtonStyle WidgetStyle;

	/** The color multiplier for the button content */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance", meta=( sRGB="true" ))
	FLinearColor ColorAndOpacity;
	
	/** The color multiplier for the button background */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance", meta=( sRGB="true" ))
	FLinearColor BackgroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FKey ActionKey;

public:
	/** Sets the color multiplier for the button background */
	UFUNCTION(BlueprintCallable, Category="Button|Appearance")
	void SetStyle(const FButtonStyle& InStyle);

	/** Sets the color multiplier for the button content */
	UFUNCTION(BlueprintCallable, Category="Button|Appearance")
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	/** Sets the color multiplier for the button background */
	UFUNCTION(BlueprintCallable, Category="Button|Appearance")
	void SetBackgroundColor(FLinearColor InBackgroundColor);

protected:
	/** Handle the actual click event from slate and forward it on */
	void SlateHandlePressed();
	void SlateHandleReleased();

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;

	// UPanelWidget
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// End UPanelWidget

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
	virtual TSharedRef<SWidget> RebuildDesignWidget(TSharedRef<SWidget> Content) override { return Content; }
#endif
	//~ End UWidget Interface

protected:
	/** Cached pointer to the underlying slate button owned by this UWidget */
	TSharedPtr<SButton> MyButton;
};
