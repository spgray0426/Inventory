// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemPopUp.generated.h"

class USizeBox;
class UTextBlock;
class USlider;
class UButton;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPopUpMenuSplit, int32, SplitAmount, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuDrop, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuConsume, int32, Index);

/**
 * 인벤토리 그리드에서 마우스 오른쪽 클릭을하면 아이템 팝업이 뜨는 위젯
 */
UCLASS()
class INVENTORY_API UInv_ItemPopUp : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeOnInitialized() override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    
    int32 GetSplitAmount() const;
    void CollapseSplitButton() const;
    void CollapseConsumeButton() const;
    void SetSliderParams(const float Max, const float Value) const;
    FVector2D GetBoxSize() const;
    void SetGridIndex(int32 Index) { GridIndex = Index; }
    int32 GetGridIndex() const { return GridIndex; }
    
    FPopUpMenuSplit OnSplit;
    FPopUpMenuDrop OnDrop;
    FPopUpMenuConsume OnConsume;
private:

    UFUNCTION()
    void SplitButtonClicked();

    UFUNCTION()
    void DropButtonClicked();

    UFUNCTION()
    void ConsumeButtonClicked();

    UFUNCTION()
    void SliderValueChanged(float Value);
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Split;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Drop;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Consume;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_Split;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_SplitAmount;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USizeBox> SizeBox_Root;

    int32 GridIndex{INDEX_NONE};
};
