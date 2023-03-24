// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "StudioGameTypes.h"
#include "Widgets/SWidget.h"
#include "Animation/InputScaleBias.h"
#include "Components/Widget.h"
#include "DoubleSlider.generated.h"

class SDoubleSlider;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMouseCaptureBeginEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMouseCaptureEndEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnControllerCaptureBeginEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnControllerCaptureEndEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputRangeValueChangedEvent, FInputRange, Value);

DECLARE_DYNAMIC_DELEGATE_RetVal(FInputRange, FGetInputRange);

/**
 * A simple widget that shows a sliding bar with a handle that allows you to control the value between 0..1.
 *
 * * No Children
 */
UCLASS()
class STUDIOGAME_API UDoubleSlider : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	/** A bindable delegate to allow logic to drive the value of the widget */
	UPROPERTY()
	FGetInputRange ValueDelegate;

	/** Sets new value if mouse position is greater/less than half the step size. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, AdvancedDisplay)
	bool MouseUsesStep;

	/** Sets whether we have to lock input to change the slider value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, AdvancedDisplay)
	bool RequiresControllerLock;

	/** Should the slider be focusable? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool IsFocusable;

protected:

	/** The volume value to display. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintGetter = "GetValue", BlueprintSetter = "SetValue", Category = "Appearance")
	FInputRange Value;

	/** The D-value the slider can be set to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = "SetDValue", Category = "Appearance")
	float DValue;

	/** The minimum value the slider can be set to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = "SetMinValue", Category = "Appearance")
	float MinValue;

	/** The maximum value the slider can be set to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = "SetMaxValue", Category = "Appearance")
	float MaxValue;

	/** The progress bar style */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style", meta=( DisplayName="Style" ))
	FDoubleSliderStyle WidgetStyle;

	/** The slider's orientation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	TEnumAsByte<EOrientation> Orientation;

	/** The color to draw the slider bar in. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter="SetSliderBarColor", Category="Appearance")
	FLinearColor SliderBarColor;

	/** The color to draw the slider handle in. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter="SetSliderHandleColor", Category="Appearance")
	FLinearColor SliderHandleColor;

	/** Whether the slidable area should be indented to fit the handle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintGetter="HasIndentHandle", BlueprintSetter="SetIndentHandle", Category = "Appearance", AdvancedDisplay)
	bool IndentHandle;

	/** Whether the handle is interactive or fixed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintGetter="IsLocked", BlueprintSetter="SetLocked", Category="Appearance", AdvancedDisplay)
	bool Locked;

	/** The amount to adjust the value by, when using a controller or keyboard */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, BlueprintGetter = "GetStepSize", BlueprintSetter="SetStepSize", Category="Appearance", meta=(UIMin="0", UIMax="1"))
	float StepSize;

public:

	/** Invoked when the mouse is pressed and a capture begins. */
	UPROPERTY(BlueprintAssignable, Category="Widget Event")
	FOnMouseCaptureBeginEvent OnMouseCaptureBegin;

	/** Invoked when the mouse is released and a capture ends. */
	UPROPERTY(BlueprintAssignable, Category="Widget Event")
	FOnMouseCaptureEndEvent OnMouseCaptureEnd;

	/** Invoked when the controller capture begins. */
	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FOnControllerCaptureBeginEvent OnControllerCaptureBegin;

	/** Invoked when the controller capture ends. */
	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FOnControllerCaptureEndEvent OnControllerCaptureEnd;

	/** Called when the value is changed by slider or typing. */
	UPROPERTY(BlueprintAssignable, Category="Widget Event")
	FOnInputRangeValueChangedEvent OnValueChanged;

	/** Gets the current value of the slider. */
	UFUNCTION(BlueprintCallable, Category="Behavior")
	FInputRange GetValue() const;

	/** Get the current value scaled from 0 to 1 */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	FInputRange GetNormalizedValue() const;

	/** Sets the current value of the slider. */
	UFUNCTION(BlueprintCallable, Category="Behavior")
	void SetValue(FInputRange InValue);

	/** Gets the D-value of the slider. */
	float GetDValue() const;

	/** Sets the current D-value of the slider. */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void SetDValue(float InDValue);

	/** Gets the minimum value of the slider. */
	float GetMinValue() const;

	/** Sets the minimum value of the slider. */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void SetMinValue(float InValue);

	/** Gets the maximum value of the slider. */
	float GetMaxValue() const;

	/** Sets the maximum value of the slider. */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void SetMaxValue(float InValue);

	/** Get the style used by the widget. */
	const FDoubleSliderStyle& GetWidgetStyle() const;

	/** Set the style used by the widget. */
	void SetWidgetStyle(const FDoubleSliderStyle& InStyle);

	/** Getg the slider's orientation. */
	EOrientation GetOrientation() const;

	/** Sets the slider's orientation. */
	void SetOrientation(EOrientation InOrientation);

	/** Gets if the slidable area should be indented to fit the handle. */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	bool HasIndentHandle() const;

	/** Sets if the slidable area should be indented to fit the handle. */
	UFUNCTION(BlueprintCallable, Category="Behavior")
	void SetIndentHandle(bool InValue);

	/** Returns true when the handle is fixed. */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	bool IsLocked() const;

	/** Sets the handle to be interactive or fixed. */
	UFUNCTION(BlueprintCallable, Category="Behavior")
	void SetLocked(bool InValue);

	/** Gets the amount to adjust the value by. */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	float GetStepSize() const;

	/** Sets the amount to adjust the value by, when using a controller or keyboard. */
	UFUNCTION(BlueprintCallable, Category="Behavior")
	void SetStepSize(float InValue);

	/** Gets the color of the slider bar. */
	FLinearColor GetSliderBarColor() const;

	/** Sets the color of the slider bar. */
	UFUNCTION(BlueprintCallable, Category="Appearance")
	void SetSliderBarColor(FLinearColor InValue);

	/** Gets the color of the handle bar */
	FLinearColor GetSliderHandleColor() const;

	/** Sets the color of the handle bar */
	UFUNCTION(BlueprintCallable, Category="Appearance")
	void SetSliderHandleColor(FLinearColor InValue);
	
	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UVisual interface

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	/** Native Slate Widget */
	TSharedPtr<SDoubleSlider> MyDoubleSlider;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	void HandleOnValueChanged(FInputRange InValue);
	void HandleOnMouseCaptureBegin();
	void HandleOnMouseCaptureEnd();
	void HandleOnControllerCaptureBegin();
	void HandleOnControllerCaptureEnd();

#if WITH_ACCESSIBILITY
	virtual TSharedPtr<SWidget> GetAccessibleWidget() const override;
#endif

	PROPERTY_BINDING_IMPLEMENTATION(FInputRange, Value);
};
