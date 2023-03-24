// Copyright 2018 CQUnreal, Inc. All Rights Reserved.

#include "Components/VirtualKeyboard.h"
#include "Components/TextWidgetTypes.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/MultiLineEditableText.h"
#include "Components/MultiLineEditableTextBox.h"


UVirtualKey::UVirtualKey(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OnInputedEvent(true)
{
	
}

void UVirtualKey::SetIsShift(bool NewIsShift)
{
	bIsShift = NewIsShift;
}

void UVirtualKey::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	OnClicked.AddUniqueDynamic(this, &UVirtualKey::HandleInputed);
}

void UVirtualKey::HandleInputed()
{
	TAttribute<bool> bPressedShift = BITFIELD_PROPERTY_BINDING(bIsShift);
	bIsShift = bPressedShift.Get(bIsShift);
	if (bIsShift)
	{
		OnInputed.Broadcast(Shift_Key);

		if (OnInputedEventDelegate.IsBound())
		{
			OnInputedEventDelegate.Execute(Shift_Key);
		}
	}
	else
	{
		OnInputed.Broadcast(Key);

		if (OnInputedEventDelegate.IsBound())
		{
			OnInputedEventDelegate.Execute(Key);
		}
	}
}

UVirtualKeyboard::UVirtualKeyboard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UVirtualKeyboard::BeginDestroy()
{
	Super::BeginDestroy();
}

bool UVirtualKeyboard::IsShift() const
{
	return bIsShift;
}

bool UVirtualKeyboard::OnInputed(const FString& Key)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Key);

	UEditableText* EditableText = Cast<UEditableText>(TextWidget);
	if (EditableText != nullptr)
	{
		FString Text = EditableText->GetText().ToString();
		Text.Append(Key);

		EditableText->SetText(FText::FromString(Text));
	}

	UEditableTextBox* EditableTextBox = Cast<UEditableTextBox>(TextWidget);
	if (EditableTextBox != nullptr)
	{
		FString Text = EditableTextBox->GetText().ToString();
		Text.Append(Key);

		EditableTextBox->SetText(FText::FromString(Text));
	}

	UMultiLineEditableText* MultiLineEditableText = Cast<UMultiLineEditableText>(TextWidget);
	if (MultiLineEditableText != nullptr)
	{
		FString Text = MultiLineEditableText->GetText().ToString();
		Text.Append(Key);

		MultiLineEditableText->SetText(FText::FromString(Text));
	}

	UMultiLineEditableTextBox* MultiLineEditableTextBox = Cast<UMultiLineEditableTextBox>(TextWidget);
	if (MultiLineEditableTextBox != nullptr)
	{
		FString Text = MultiLineEditableTextBox->GetText().ToString();
		Text.Append(Key);

		MultiLineEditableTextBox->SetText(FText::FromString(Text));
	}

	return true;
}

bool UVirtualKeyboard::OnDelete(const FString& Key)
{
	UEditableText* EditableText = Cast<UEditableText>(TextWidget);
	if (EditableText != nullptr)
	{
		FString Text = EditableText->GetText().ToString();
		if (Text.Len() > 0)
		{
			Text.RemoveAt(Text.Len() - 1);
		}

		EditableText->SetText(FText::FromString(Text));
	}

	UEditableTextBox* EditableTextBox = Cast<UEditableTextBox>(TextWidget);
	if (EditableTextBox != nullptr)
	{
		FString Text = EditableTextBox->GetText().ToString();
		if (Text.Len() > 0)
		{
			Text.RemoveAt(Text.Len() - 1);
		}

		EditableTextBox->SetText(FText::FromString(Text));
	}

	UMultiLineEditableText* MultiLineEditableText = Cast<UMultiLineEditableText>(TextWidget);
	if (MultiLineEditableText != nullptr)
	{
		FString Text = MultiLineEditableText->GetText().ToString();
		if (Text.Len() > 0)
		{
			Text.RemoveAt(Text.Len() - 1);
		}

		MultiLineEditableText->SetText(FText::FromString(Text));
	}

	UMultiLineEditableTextBox* MultiLineEditableTextBox = Cast<UMultiLineEditableTextBox>(TextWidget);
	if (MultiLineEditableTextBox != nullptr)
	{
		FString Text = MultiLineEditableTextBox->GetText().ToString();
		if (Text.Len() > 0)
		{
			Text.RemoveAt(Text.Len() - 1);
		}

		MultiLineEditableTextBox->SetText(FText::FromString(Text));
	}

	return true;
}

bool UVirtualKeyboard::OnShift(const FString& Key)
{
	bIsShift = !bIsShift;

	return true;
}

bool UVirtualKeyboard::OnDetermine(const FString& Key)
{
	Unregister();

	return true;
}

void UVirtualKeyboard::Register(UWidget* InTextWidget)
{
	TextWidget = InTextWidget;

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UVirtualKeyboard::Unregister()
{
	SetVisibility(ESlateVisibility::Collapsed);
}