// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Blueprint/UserWidgetEx.h"
#include "Components/Button.h"


#define LOCTEXT_NAMESPACE "UMG"

UUserWidgetEx::UUserWidgetEx(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, RawIndex(0)
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
void UUserWidgetEx::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	const FName MemberPropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	const FName WidgetLayersName = GET_MEMBER_NAME_CHECKED(UUserWidgetEx, WidgetLayers);
	if (MemberPropertyName == TEXT("Widgets") || MemberPropertyName == TEXT("WidgetLayers"))
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

		const FName MaxColumnName = GET_MEMBER_NAME_CHECKED(UUserWidgetEx, MaxColumn);
		const FName WidgetNamesName = GET_MEMBER_NAME_CHECKED(UUserWidgetEx, WidgetNames);
		for (TFieldIterator<FProperty> It(this->GetClass()); It; ++It)
		{
			if (!It->GetName().Equals(WidgetNamesName.ToString()) &&
				!It->GetName().Equals(MaxColumnName.ToString()))
			{
				continue;
			}

			FEditPropertyChain Chain;
			Chain.AddHead(*It);
			
			TArrayView<const UObject* const> TopLevelObjects = { this };
			FPropertyChangedEvent Event(*It, EPropertyChangeType::ValueSet, TopLevelObjects);

			FPropertyChangedChainEvent ChangedEvent(Chain, Event);
			ChangedEvent.ObjectIteratorIndex = 0;

			PostEditChangeChainProperty(ChangedEvent);
		}

		Modify();
	}

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
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

void UUserWidgetEx::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	SetKeyboardFocus();
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

	UE_LOG(LogTemp, Log, TEXT("RawIndex: %d ColumnIndex: %d FocusWidget: %s"), RawIndex, ColumnIndex, *GetNameSafe(FocusWidget));

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UUserWidgetEx::SetFocusWidget(int32 DeltaRaw, int32 DeltaColumn)
{
	UWidget* OldWidget = FocusWidget;

	int32 RawCount = WidgetLayers.Num();
	RawIndex = FMath::Clamp(RawIndex + DeltaRaw, 0, RawCount);

	if (RawCount > 0 && WidgetLayers.IsValidIndex(RawIndex))
	{
		const FWidgetLayer& WidgetLayer = WidgetLayers[RawIndex];

		int32 ColumnCount = WidgetLayer.Widgets.Num();
		ColumnIndex = FMath::Clamp(ColumnIndex + DeltaColumn, 0, ColumnCount);

		if (WidgetLayer.Widgets.IsValidIndex(ColumnIndex))
		{
			FocusWidget = WidgetLayer.Widgets[ColumnIndex];
		}
	}

	OnFocusWidgetChanged(OldWidget, FocusWidget);
}

void UUserWidgetEx::OnFocusWidgetChanged(UWidget* OldWidget, UWidget* NewWidget)
{
	if (OldWidget)
	{

	}

	if (NewWidget)
	{

	}
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE