// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemDescription/Inv_ItemDescription.h"

#include "Components/SizeBox.h"

FVector2D UInv_ItemDescription::GetBoxSize() const
{
    // SizeBox의 desired size를 반환하여 콘텐츠에 맞는 실제 크기를 가져옵니다
    // 이 크기는 화면 경계 내에 위젯을 배치하기 위한 계산에 사용됩니다
    return SizeBox->GetDesiredSize();
}

void UInv_ItemDescription::SetVisibility(ESlateVisibility InVisibility)
{
    // 가시성 변경 전 모든 자식 위젯을 축소(Collapse)하여 초기화
    // 이전에 표시된 아이템 정보를 지우고 새로운 정보만 표시하기 위함
    for (auto Child : GetChildren())
    {
        Child->Collapse();
    }
    Super::SetVisibility(InVisibility);
}
