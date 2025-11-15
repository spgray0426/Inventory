// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"

#include "Components/TextBlock.h"

void UInv_Leaf_LabeledValue::SetText_Label(const FText& Text, bool bCollapse) const
{
	// bCollapse가 true이면 라벨을 숨깁니다
	if (bCollapse)
	{
		Text_Label->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// 라벨 텍스트를 설정합니다
	Text_Label->SetText(Text);
}

void UInv_Leaf_LabeledValue::SetText_Value(const FText& Text, bool bCollapse) const
{
	// bCollapse가 true이면 값을 숨깁니다
	if (bCollapse)
	{
		Text_Value->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// 값 텍스트를 설정합니다
	Text_Value->SetText(Text);
}

void UInv_Leaf_LabeledValue::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 라벨의 폰트 정보를 가져옵니다
	FSlateFontInfo FontInfo_Label = Text_Label->GetFont();

	// 에디터에서 설정한 폰트 크기를 적용합니다
	FontInfo_Label.Size = FontSize_Label;

	// 수정된 폰트 정보를 라벨에 적용합니다
	Text_Label->SetFont(FontInfo_Label);

	// 값의 폰트 정보를 가져옵니다
	FSlateFontInfo FontInfo_Value = Text_Value->GetFont();

	// 에디터에서 설정한 폰트 크기를 적용합니다
	FontInfo_Value.Size = FontSize_Value;

	// 수정된 폰트 정보를 값에 적용합니다
	Text_Value->SetFont(FontInfo_Value);
}
