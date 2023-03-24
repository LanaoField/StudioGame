// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Components/ActionButton.h"
#include "Components/ButtonSlot.h"
#include "GameFramework/PlayerController.h"

#define LOCTEXT_NAMESPACE "UMG"

static FButtonStyle* DefaultButtonStyle = nullptr;

UActionButton::UActionButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (DefaultButtonStyle == nullptr)
	{
		// HACK: THIS SHOULD NOT COME FROM CORESTYLE AND SHOULD INSTEAD BE DEFINED BY ENGINE TEXTURES/PROJECT SETTINGS
		DefaultButtonStyle = new FButtonStyle(FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"));

		// Unlink UMG default colors from the editor settings colors.
		DefaultButtonStyle->UnlinkColors();
	}

	WidgetStyle = *DefaultButtonStyle;

	ColorAndOpacity = FLinearColor::White;
	BackgroundColor = FLinearColor::White;
}

void UActionButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyButton->SetColorAndOpacity(ColorAndOpacity);
	MyButton->SetBorderBackgroundColor(BackgroundColor);
}

void UActionButton::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyButton.Reset();
}

void UActionButton::PostLoad()
{
	Super::PostLoad();

	if (GetChildrenCount() > 0)
	{
		//TODO UMG Pre-Release Upgrade, now buttons have slots of their own.  Convert existing slot to new slot.
		if (UPanelSlot* PanelSlot = GetContentSlot())
		{
			UButtonSlot* ButtonSlot = Cast<UButtonSlot>(PanelSlot);
			if (ButtonSlot == NULL)
			{
				ButtonSlot = NewObject<UButtonSlot>(this);
				ButtonSlot->Content = GetContentSlot()->Content;
				ButtonSlot->Content->Slot = ButtonSlot;
				Slots[0] = ButtonSlot;
			}
		}
	}

#if ENGINE_MAJOR_VERSION < 5
	if (GetLinkerUE4Version() < VER_UE4_DEPRECATE_UMG_STYLE_ASSETS && Style_DEPRECATED != nullptr)
	{
		const FButtonStyle* StylePtr = Style_DEPRECATED->GetStyle<FButtonStyle>();
		if (StylePtr != nullptr)
		{
			WidgetStyle = *StylePtr;
		}

		Style_DEPRECATED = nullptr;
	}
#endif
}

void UActionButton::SetStyle(const FButtonStyle& InStyle)
{
	WidgetStyle = InStyle;
	if (MyButton.IsValid())
	{
		MyButton->SetButtonStyle(&WidgetStyle);
	}
}

void UActionButton::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;
	if (MyButton.IsValid())
	{
		MyButton->SetColorAndOpacity(InColorAndOpacity);
	}
}

void UActionButton::SetBackgroundColor(FLinearColor InBackgroundColor)
{
	BackgroundColor = InBackgroundColor;
	if (MyButton.IsValid())
	{
		MyButton->SetBorderBackgroundColor(InBackgroundColor);
	}
}

void UActionButton::SlateHandlePressed()
{
	if (ActionKey.IsValid())
	{
		APlayerController* PlayerController = GetOwningPlayer();
		if (PlayerController != nullptr)
		{
			#if ENGINE_MAJOR_VERSION >= 5
			FInputKeyParams InputKeyParams;
			InputKeyParams.Key = ActionKey;
			InputKeyParams.Event = EInputEvent::IE_Pressed;
			InputKeyParams.bIsGamepadOverride = true;
			PlayerController->InputKey(InputKeyParams);
			#else
			PlayerController->InputKey(ActionKey, EInputEvent::IE_Pressed, 0.f, true);
			#endif
		}
	}
}

void UActionButton::SlateHandleReleased()
{
	if (ActionKey.IsValid())
	{
		APlayerController* PlayerController = GetOwningPlayer();
		if (PlayerController != nullptr)
		{
			#if ENGINE_MAJOR_VERSION >= 5
			FInputKeyParams InputKeyParams;
			InputKeyParams.Key = ActionKey;
			InputKeyParams.Event = EInputEvent::IE_Released;
			InputKeyParams.bIsGamepadOverride = true;
			PlayerController->InputKey(InputKeyParams);
			#else
			PlayerController->InputKey(ActionKey, EInputEvent::IE_Released, 0.f, true);
			#endif
		}
	}
}

TSharedRef<SWidget> UActionButton::RebuildWidget()
{
	MyButton = SNew(SButton)
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(EButtonClickMethod::PreciseClick)
		.TouchMethod(EButtonTouchMethod::PreciseTap)
		.IsFocusable(false)
		;

	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}

	return MyButton.ToSharedRef();
}

UClass* UActionButton::GetSlotClass() const
{
	return UButtonSlot::StaticClass();
}

void UActionButton::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live slot if it already exists
	if (MyButton.IsValid())
	{
		CastChecked<UButtonSlot>(InSlot)->BuildSlot(MyButton.ToSharedRef());
	}
}

void UActionButton::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyButton.IsValid())
	{
		MyButton->SetContent(SNullWidget::NullWidget);
	}
}

#if WITH_EDITOR

const FText UActionButton::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE