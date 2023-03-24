// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/DoubleSlider.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SDoubleSlider.h"
#include "Styling/StudioGameCoreStyle.h"

#if ENGINE_MAJOR_VERSION >= 5
#include UE_INLINE_GENERATED_CPP_BY_NAME(DoubleSlider)
#endif

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UDoubleSlider

static FDoubleSliderStyle* DefaultSliderStyle = nullptr;

#if WITH_EDITOR
static FDoubleSliderStyle* EditorSliderStyle = nullptr;
#endif 

UDoubleSlider::UDoubleSlider(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	DValue = 0.0f;
	MinValue = 0.0f;
	MaxValue = 1.0f;
	Orientation = EOrientation::Orient_Horizontal;
	SliderBarColor = FLinearColor::White;
	SliderHandleColor = FLinearColor::White;
	StepSize = 0.01f;
	IsFocusable = true;
	MouseUsesStep = false;
	RequiresControllerLock = true;

	if (DefaultSliderStyle == nullptr)
	{
		DefaultSliderStyle = new FDoubleSliderStyle(FStudioGameCoreStyle::Get().GetWidgetStyle<FDoubleSliderStyle>("DoubleSlider"));

		// Unlink UMG default colors.
		DefaultSliderStyle->UnlinkColors();
	}

	WidgetStyle = *DefaultSliderStyle;


#if WITH_EDITOR 
	if (EditorSliderStyle == nullptr)
	{
		EditorSliderStyle = new FDoubleSliderStyle(FStudioGameCoreStyle::Get().GetWidgetStyle<FDoubleSliderStyle>("DoubleSlider"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorSliderStyle->UnlinkColors();
	}

#if ENGINE_MAJOR_VERSION >= 5
	if (IsEditorWidget())
	{
		WidgetStyle = *EditorSliderStyle;

		// The CDO isn't an editor widget and thus won't use the editor style, call post edit change to mark difference from CDO
		PostEditChange();
	}
#endif
#endif // WITH_EDITOR

PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_EDITORONLY_DATA
	AccessibleBehavior = ESlateAccessibleBehavior::Summary;
	bCanChildrenBeAccessible = false;
#endif
}

TSharedRef<SWidget> UDoubleSlider::RebuildWidget()
{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	MyDoubleSlider = SNew(SDoubleSlider)
		.Style(&WidgetStyle)
		.IsFocusable(IsFocusable)
		.OnMouseCaptureBegin(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnMouseCaptureBegin))
		.OnMouseCaptureEnd(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnMouseCaptureEnd))
		.OnControllerCaptureBegin(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnControllerCaptureBegin))
		.OnControllerCaptureEnd(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnControllerCaptureEnd))
		.OnValueChanged(BIND_UOBJECT_DELEGATE(FOnInputRangeValueChanged, HandleOnValueChanged));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	return MyDoubleSlider.ToSharedRef();
}

void UDoubleSlider::SynchronizeProperties()
{
	Super::SynchronizeProperties();

PRAGMA_DISABLE_DEPRECATION_WARNINGS
	TAttribute<FInputRange> ValueBinding = PROPERTY_BINDING(FInputRange, Value);
	
	MyDoubleSlider->SetOrientation(Orientation);
	MyDoubleSlider->SetMouseUsesStep(MouseUsesStep);
	MyDoubleSlider->SetRequiresControllerLock(RequiresControllerLock);
	MyDoubleSlider->SetSliderBarColor(SliderBarColor);
	MyDoubleSlider->SetSliderHandleColor(SliderHandleColor);
	MyDoubleSlider->SetValue(ValueBinding);
	MyDoubleSlider->SetDValue(DValue);
	MyDoubleSlider->SetMinAndMaxValues(MinValue, MaxValue);
	MyDoubleSlider->SetLocked(Locked);
	MyDoubleSlider->SetIndentHandle(IndentHandle);
	MyDoubleSlider->SetStepSize(StepSize);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UDoubleSlider::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyDoubleSlider.Reset();
}

void UDoubleSlider::HandleOnValueChanged(FInputRange InValue)
{
	OnValueChanged.Broadcast(InValue);
}

void UDoubleSlider::HandleOnMouseCaptureBegin()
{
	OnMouseCaptureBegin.Broadcast();
}

void UDoubleSlider::HandleOnMouseCaptureEnd()
{
	OnMouseCaptureEnd.Broadcast();
}

void UDoubleSlider::HandleOnControllerCaptureBegin()
{
	OnControllerCaptureBegin.Broadcast();
}

void UDoubleSlider::HandleOnControllerCaptureEnd()
{
	OnControllerCaptureEnd.Broadcast();
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
FInputRange UDoubleSlider::GetValue() const
{
	if ( MyDoubleSlider.IsValid() )
	{
		return MyDoubleSlider->GetValue();
	}

	return Value;
}

FInputRange UDoubleSlider::GetNormalizedValue() const
{
	if (MyDoubleSlider.IsValid())
	{
		return MyDoubleSlider->GetNormalizedValue();
	}

	if (MinValue == MaxValue)
	{
		return FInputRange(0.f, 1.f);
	}
	else
	{
		const float MDValue = MaxValue - MinValue;
		return FInputRange((Value.Min - MinValue) / MDValue, (Value.Max - MinValue) / MDValue);
	}
}

void UDoubleSlider::SetValue(FInputRange InValue)
{
	Value = InValue;
	if ( MyDoubleSlider.IsValid() )
	{
		MyDoubleSlider->SetValue(InValue);
	}
}

float UDoubleSlider::GetDValue() const
{
	if (MyDoubleSlider.IsValid())
	{
		return MyDoubleSlider->GetDValue();
	}
	return DValue;
}

void UDoubleSlider::SetDValue(float InDValue)
{
	DValue = InDValue;
	if (MyDoubleSlider.IsValid())
	{
		MyDoubleSlider->SetDValue(InDValue);
	}
}

float UDoubleSlider::GetMinValue() const
{
	if (MyDoubleSlider.IsValid())
	{
		return MyDoubleSlider->GetMinValue();
	}
	return MinValue;
}

void UDoubleSlider::SetMinValue(float InValue)
{
	MinValue = InValue;
	if (MyDoubleSlider.IsValid())
	{
		// Because SSlider clamps min/max values upon setting them,
		// we have to send both values together to ensure that they
		// don't get out of sync.
		MyDoubleSlider->SetMinAndMaxValues(MinValue, MaxValue);
	}
}

float UDoubleSlider::GetMaxValue() const
{
	if (MyDoubleSlider.IsValid())
	{
		return MyDoubleSlider->GetMaxValue();
	}
	return MaxValue;
}

void UDoubleSlider::SetMaxValue(float InValue)
{
	MaxValue = InValue;
	if (MyDoubleSlider.IsValid())
	{
		MyDoubleSlider->SetMinAndMaxValues(MinValue, MaxValue);
	}
}

const FDoubleSliderStyle& UDoubleSlider::GetWidgetStyle() const
{
	return WidgetStyle;
}

void UDoubleSlider::SetWidgetStyle(const FDoubleSliderStyle& InStyle)
{
	WidgetStyle = InStyle;
	if (MyDoubleSlider.IsValid())
	{
		MyDoubleSlider->SetStyle(&WidgetStyle);
	}
}

EOrientation UDoubleSlider::GetOrientation() const
{
	return Orientation;
}

void UDoubleSlider::SetOrientation(EOrientation InOrientation)
{
	Orientation = InOrientation;
	if (MyDoubleSlider.IsValid())
	{
		MyDoubleSlider->SetOrientation(Orientation);
	}
}

bool UDoubleSlider::HasIndentHandle() const
{
	return IndentHandle;
}

void UDoubleSlider::SetIndentHandle(bool InIndentHandle)
{
	IndentHandle = InIndentHandle;
	if ( MyDoubleSlider.IsValid() )
	{
		MyDoubleSlider->SetIndentHandle(InIndentHandle);
	}
}

bool UDoubleSlider::IsLocked() const
{
	return Locked;
}

void UDoubleSlider::SetLocked(bool InLocked)
{
	Locked = InLocked;
	if ( MyDoubleSlider.IsValid() )
	{
		MyDoubleSlider->SetLocked(InLocked);
	}
}

float UDoubleSlider::GetStepSize() const
{
	if (MyDoubleSlider.IsValid())
	{
		return MyDoubleSlider->GetStepSize();
	}
	return StepSize;
}

void UDoubleSlider::SetStepSize(float InValue)
{
	StepSize = InValue;
	if (MyDoubleSlider.IsValid())
	{
		MyDoubleSlider->SetStepSize(InValue);
	}
}

FLinearColor UDoubleSlider::GetSliderHandleColor() const
{
	return SliderHandleColor;
}

void UDoubleSlider::SetSliderHandleColor(FLinearColor InValue)
{
	SliderHandleColor = InValue;
	if (MyDoubleSlider.IsValid())
	{
		MyDoubleSlider->SetSliderHandleColor(InValue);
	}
}

FLinearColor UDoubleSlider::GetSliderBarColor() const
{
	return SliderBarColor;
}

void UDoubleSlider::SetSliderBarColor(FLinearColor InValue)
{
	SliderBarColor = InValue;
	if (MyDoubleSlider.IsValid())
	{
		MyDoubleSlider->SetSliderBarColor(InValue);
	}
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_ACCESSIBILITY
TSharedPtr<SWidget> UDoubleSlider::GetAccessibleWidget() const
{
	return MyDoubleSlider;
}
#endif

#if WITH_EDITOR

const FText UDoubleSlider::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

