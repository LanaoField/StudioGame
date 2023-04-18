// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Blueprint/UserWidgetEx.h"
#include "Components/Button.h"
#include "Styling/SlateWidgetStyle.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "UMG"

TArray<TWeakObjectPtr<UUserWidgetEx>> UUserWidgetEx::UserWidgets;

UUserWidgetEx::UUserWidgetEx(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, RowIndex(0)
	, ColumnIndex(0)
{
	bIsFocusable = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UpKeys.Add(EKeys::W);
		DownKeys.Add(EKeys::S);
		LeftKeys.Add(EKeys::A);
		RightKeys.Add(EKeys::D);
		ConfirmKeys.Add(EKeys::SpaceBar);
	}
}

#if WITH_EDITOR
void UUserWidgetEx::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	const FName MemberPropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	const FName WidgetNamesName = GET_MEMBER_NAME_CHECKED(UUserWidgetEx, WidgetNames);
	const FName WidgetLayersName = GET_MEMBER_NAME_CHECKED(UUserWidgetEx, WidgetLayers);
	const FName MaxColumnName = GET_MEMBER_NAME_CHECKED(UUserWidgetEx, MaxColumn);
	if (MemberPropertyName == WidgetLayersName)
	{
		int32 RawCount = WidgetLayers.Num();

		WidgetNames.Empty();

		MaxColumn = 0;
		for (const FWidgetLayer& WidgetLayer : WidgetLayers)
		{
			int32 Num = WidgetLayer.Widgets.Num();
			if (Num > MaxColumn)
			{
				MaxColumn = Num;
			}
		}

		for (int32 Index = 0; Index < RawCount * MaxColumn; Index++)
		{
			UWidget* Widget = nullptr;

			int32 Raw = Index / MaxColumn;
			int32 Column = Index % MaxColumn;

			if (WidgetLayers.IsValidIndex(Raw))
			{
				const FWidgetLayer& WidgetLayer = WidgetLayers[Raw];
				if (WidgetLayer.Widgets.IsValidIndex(Column))
				{
					Widget = WidgetLayer.Widgets[Column];
				}
			}

			WidgetNames.Add(*GetNameSafe(Widget));
		}

		for (TFieldIterator<FProperty> It(this->GetClass()); It; ++It)
		{
			if (It->GetName().Equals(WidgetNamesName.ToString()))
			{
				FPropertyChangedEvent ChangedEvent(*It, EPropertyChangeType::ArrayAdd);
				PostEditChangeProperty(ChangedEvent);
			}
			else if (It->GetName().Equals(MaxColumnName.ToString()))
			{
				FPropertyChangedEvent ChangedEvent(*It, EPropertyChangeType::ArrayAdd);
				PostEditChangeProperty(ChangedEvent);
			}
		}
	}
}
#endif

bool UUserWidgetEx::Initialize()
{
	bool bInitialize = Super::Initialize();

	for (int32 Index = 0; Index < WidgetNames.Num() && MaxColumn > 0; Index++)
	{
		int32 Raw = Index / MaxColumn;

		const FName WidgetName = WidgetNames[Index];
		if (WidgetName == NAME_None)
		{
			continue;
		}

		UWidget* Widget = GetWidgetFromName(WidgetName);
		if (WidgetLayers.Num() < Raw + 1)
		{
			FWidgetLayer WidgetLayer;
			WidgetLayer.Widgets.Add(Widget);

			WidgetLayers.Add(WidgetLayer);
		}
		else
		{
			FWidgetLayer& WidgetLayer = WidgetLayers[Raw];
			WidgetLayer.Widgets.Add(Widget);
		}
	}

	return bInitialize;
}

void UUserWidgetEx::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
	SetKeyboardFocus();
	SetFocusWidget();

	UserWidgets.Add(this);
}

void UUserWidgetEx::NativeDestruct()
{
	UserWidgets.Remove(this);

	Super::NativeDestruct();
}

FReply UUserWidgetEx::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey& Key = InKeyEvent.GetKey();
	if (UpKeys.Contains(Key))
	{
		SetFocusWidget(-1, 0);
	}
	else if (DownKeys.Contains(Key))
	{
		SetFocusWidget(1, 0);
	}
	else if (LeftKeys.Contains(Key))
	{
		SetFocusWidget(0, -1);
	}
	else if (RightKeys.Contains(Key))
	{
		SetFocusWidget(0, 1);
	}
	else if (ConfirmKeys.Contains(Key))
	{
		if (FocusWidget != nullptr)
		{
			UButton* Button = Cast<UButton>(FocusWidget);
			if (Button != nullptr)
			{
				Button->OnClicked.Broadcast();
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("RowIndex: %d ColumnIndex: %d FocusWidget: %s"), RowIndex, ColumnIndex, *GetNameSafe(FocusWidget));

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UUserWidgetEx::SetFocusWidget(int32 DeltaRaw, int32 DeltaColumn)
{
	UWidget* OldWidget = FocusWidget;

	int32 RawCount = WidgetLayers.Num();
	RowIndex = FMath::Clamp(RowIndex + DeltaRaw, 0, RawCount - 1);

	if (RawCount > 0 && WidgetLayers.IsValidIndex(RowIndex))
	{
		const FWidgetLayer& WidgetLayer = WidgetLayers[RowIndex];

		int32 ColumnCount = WidgetLayer.Widgets.Num();
		ColumnIndex = FMath::Clamp(ColumnIndex + DeltaColumn, 0, ColumnCount - 1);

		if (WidgetLayer.Widgets.IsValidIndex(ColumnIndex))
		{
			FocusWidget = WidgetLayer.Widgets[ColumnIndex];
		}
	}

	OnFocusWidgetChanged(OldWidget, FocusWidget);
}

void UUserWidgetEx::OnFocusWidgetChanged_Implementation(UWidget* OldWidget, UWidget* NewWidget)
{
	if (OldWidget)
	{
		UButton* Button = Cast<UButton>(OldWidget);
		if (Button != nullptr)
		{
			Button->WidgetStyle.Normal = ButtonStyleHoveredCache;
		}
	}

	if (NewWidget)
	{
		UButton* Button = Cast<UButton>(NewWidget);
		if (Button != nullptr)
		{
			ButtonStyleHoveredCache = Button->WidgetStyle.Normal;

			Button->WidgetStyle.Normal = Button->WidgetStyle.Hovered;
		}
		NewWidget->SetFocus();
	}
}

void UUserWidgetEx::SetVisibility(ESlateVisibility InVisibility)
{
	ESlateVisibility OldVisibility = GetVisibility();

	Super::SetVisibility(InVisibility);

	ESlateVisibility NewVisibility = GetVisibility();

	if (OldVisibility != NewVisibility)
	{
		if (NewVisibility == ESlateVisibility::Collapsed || NewVisibility == ESlateVisibility::Hidden)
		{
			bIsFocusable = false;

			UserWidgets.Remove(this);
		}
		else
		{
			bIsFocusable = true;

			UserWidgets.Remove(this);
			UserWidgets.Insert(this, 0);
		}
	}

	TWeakObjectPtr<UUserWidgetEx> UserWidget = nullptr;
	while (UserWidgets.Num() > 0 && !UserWidget.IsValid())
	{
		UserWidget = UserWidgets[0];
		if (!UserWidget.IsValid())
		{
			UserWidgets.Remove(UserWidget);
			continue;
		}

		UserWidget->SetFocusWidget();
	}

	UE_LOG(LogTemp, Log, TEXT("Name: %s VisibilityChangedEvent :%d"), *GetNameSafe(this), InVisibility);
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
