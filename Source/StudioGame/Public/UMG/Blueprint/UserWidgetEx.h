// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "UserWidgetEx.generated.h"


USTRUCT(BlueprintType)
struct FWidgetLayer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Instanced, Category = "WidgetLayer")
	TArray<UWidget*> Widgets;
};

/**
 * UUserWidgetEx
 */
UCLASS()
class STUDIOGAME_API UUserWidgetEx : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	// End of UObject interface
#endif

	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void SetVisibility(ESlateVisibility InVisibility) override;

	UFUNCTION(BlueprintCallable, Category = "User Interface|Ex")
	virtual void SetFocusWidget(int32 DeltaRaw = 0, int32 DeltaColumn = 0);

	UFUNCTION(BlueprintNativeEvent, Category = "User Interface|Ex")
	void OnFocusWidgetChanged(UWidget* OldWidget, UWidget* NewWidget);

protected:
	UPROPERTY(EditAnywhere, Category = "User Interface|Ex")
	TArray<FKey> UpKeys;

	UPROPERTY(EditAnywhere, Category = "User Interface|Ex")
	TArray<FKey> DownKeys;

	UPROPERTY(EditAnywhere, Category = "User Interface|Ex")
	TArray<FKey> LeftKeys;

	UPROPERTY(EditAnywhere, Category = "User Interface|Ex")
	TArray<FKey> RightKeys;

	UPROPERTY(EditAnywhere, Category = "User Interface|Ex")
	TArray<FKey> ConfirmKeys;

	UPROPERTY(EditAnywhere, Transient, Category = "User Interface|Ex")
	TArray<FWidgetLayer> WidgetLayers;

	UPROPERTY(EditAnywhere, Category = "User Interface|Ex")
	int32 MaxColumn = 0;

	UPROPERTY(EditAnywhere, Category = "User Interface|Ex")
	TArray<FName> WidgetNames;

private:
	int32 RowIndex;

	int32 ColumnIndex;

	FSlateBrush ButtonStyleHoveredCache;

	UPROPERTY()
	UWidget* FocusWidget;

	static TArray<TWeakObjectPtr<UUserWidgetEx>> UserWidgets;
};
