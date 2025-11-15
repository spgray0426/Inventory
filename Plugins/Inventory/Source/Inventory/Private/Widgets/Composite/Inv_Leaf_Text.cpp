// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Composite/Inv_Leaf_Text.h"

#include "Components/TextBlock.h"

void UInv_Leaf_Text::SetText(const FText& Text) const
{
	// 텍스트 블록에 텍스트를 설정합니다.
	Text_LeafText->SetText(Text);
}

void UInv_Leaf_Text::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 현재 폰트 정보를 가져옵니다.
	FSlateFontInfo FontInfo = Text_LeafText->GetFont();

	// 에디터에서 설정한 폰트 크기를 적용합니다.
	FontInfo.Size = FontSize;

	// 수정된 폰트 정보를 텍스트 블록에 적용합니다.
	Text_LeafText->SetFont(FontInfo);
}
