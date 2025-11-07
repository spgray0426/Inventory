// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UInv_ItemPopUp::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // 각 버튼의 클릭 이벤트와 슬라이더의 값 변경 이벤트를 바인딩
    Button_Split->OnClicked.AddDynamic(this, &ThisClass::SplitButtonClicked);
    Button_Drop->OnClicked.AddDynamic(this, &ThisClass::DropButtonClicked);
    Button_Consume->OnClicked.AddDynamic(this, &ThisClass::ConsumeButtonClicked);
    Slider_Split->OnValueChanged.AddDynamic(this, &ThisClass::SliderValueChanged);
}

void UInv_ItemPopUp::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    // 마우스가 팝업 영역을 벗어나면 팝업을 화면에서 제거
    RemoveFromParent();
}

int32 UInv_ItemPopUp::GetSplitAmount() const
{
    // 슬라이더 값을 정수로 변환하여 반환
    return FMath::Floor(Slider_Split->GetValue());
}

void UInv_ItemPopUp::CollapseSplitButton() const
{
    // 분리 관련 UI 요소들을 모두 숨김 처리
    Button_Split->SetVisibility(ESlateVisibility::Collapsed);
    Slider_Split->SetVisibility(ESlateVisibility::Collapsed);
    Text_SplitAmount->SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_ItemPopUp::CollapseConsumeButton() const
{
    // 사용 버튼을 숨김 처리
    Button_Consume->SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_ItemPopUp::SetSliderParams(const float Max, const float Value) const
{
    // 슬라이더의 최소값, 최대값, 현재값 설정
    Slider_Split->SetMaxValue(Max);
    Slider_Split->SetMinValue(1);
    Slider_Split->SetValue(Value);

    // Slider_Split의 값이 변경될때 실행되는 함수에서도 변경을 해줘서 실제로는 필요없는 코드
    Text_SplitAmount->SetText(FText::AsNumber(FMath::Floor(Value)));
}

FVector2D UInv_ItemPopUp::GetBoxSize() const
{
    // 루트 사이즈박스의 너비와 높이를 반환
    return FVector2D(SizeBox_Root->GetWidthOverride(), SizeBox_Root->GetHeightOverride());
}

void UInv_ItemPopUp::SplitButtonClicked()
{
    // 분리 델리게이트를 실행하고 성공 시 팝업 제거
    if (OnSplit.ExecuteIfBound(GetSplitAmount(), GridIndex))
    {
        RemoveFromParent();
    }
}

void UInv_ItemPopUp::DropButtonClicked()
{
    // 버리기 델리게이트를 실행하고 성공 시 팝업 제거
    if (OnDrop.ExecuteIfBound(GridIndex))
    {
        RemoveFromParent();
    }
}

void UInv_ItemPopUp::ConsumeButtonClicked()
{
    // 사용 델리게이트를 실행하고 성공 시 팝업 제거
    if (OnConsume.ExecuteIfBound(GridIndex))
    {
        RemoveFromParent();
    }
}

void UInv_ItemPopUp::SliderValueChanged(float Value)
{
    // 슬라이더 값이 변경되면 텍스트를 정수 형태로 업데이트
    Text_SplitAmount->SetText(FText::AsNumber(FMath::Floor(Value)));
}
