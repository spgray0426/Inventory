// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemPopUp.generated.h"

class USizeBox;
class UTextBlock;
class USlider;
class UButton;

/** 분리 버튼 클릭 시 호출되는 델리게이트 (분리 수량, 그리드 인덱스) */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPopUpMenuSplit, int32, SplitAmount, int32, Index);

/** 버리기 버튼 클릭 시 호출되는 델리게이트 (그리드 인덱스) */
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuDrop, int32, Index);

/** 사용 버튼 클릭 시 호출되는 델리게이트 (그리드 인덱스) */
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

    /** 슬라이더로 설정된 분리 수량을 반환 (소수점 버림) */
    int32 GetSplitAmount() const;

    /** 분리 버튼과 슬라이더를 숨김 처리 */
    void CollapseSplitButton() const;

    /** 사용 버튼을 숨김 처리 */
    void CollapseConsumeButton() const;

    /** 슬라이더의 최대값과 현재값을 설정 */
    void SetSliderParams(const float Max, const float Value) const;

    /** 위젯의 크기를 반환 */
    FVector2D GetBoxSize() const;

    /** 그리드 인덱스를 설정 */
    void SetGridIndex(int32 Index) { GridIndex = Index; }

    /** 그리드 인덱스를 반환 */
    int32 GetGridIndex() const { return GridIndex; }

    /** 분리 버튼 클릭 시 실행될 델리게이트 */
    FPopUpMenuSplit OnSplit;

    /** 버리기 버튼 클릭 시 실행될 델리게이트 */
    FPopUpMenuDrop OnDrop;

    /** 사용 버튼 클릭 시 실행될 델리게이트 */
    FPopUpMenuConsume OnConsume;
private:
    /** 분리 버튼 클릭 시 호출되는 콜백 함수 */
    UFUNCTION()
    void SplitButtonClicked();

    /** 버리기 버튼 클릭 시 호출되는 콜백 함수 */
    UFUNCTION()
    void DropButtonClicked();

    /** 사용 버튼 클릭 시 호출되는 콜백 함수 */
    UFUNCTION()
    void ConsumeButtonClicked();

    /** 슬라이더 값 변경 시 호출되는 콜백 함수 */
    UFUNCTION()
    void SliderValueChanged(float Value);

    /** 분리 버튼 위젯 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Split;

    /** 버리기 버튼 위젯 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Drop;

    /** 사용 버튼 위젯 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Consume;

    /** 분리 수량을 조절하는 슬라이더 위젯 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_Split;

    /** 분리 수량을 표시하는 텍스트 위젯 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_SplitAmount;

    /** 루트 사이즈박스 위젯 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USizeBox> SizeBox_Root;

    /** 이 팝업이 참조하는 그리드 슬롯의 인덱스 */
    int32 GridIndex{INDEX_NONE};
};
