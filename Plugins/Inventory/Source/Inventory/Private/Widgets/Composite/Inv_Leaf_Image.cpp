// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Composite/Inv_Leaf_Image.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"

void UInv_Leaf_Image::SetImage(UTexture2D* Texture) const
{
	// 이미지 위젯에 텍스처를 설정합니다.
	Image_Icon->SetBrushFromTexture(Texture);
}

void UInv_Leaf_Image::SetBoxSize(const FVector2D& Size) const
{
	// SizeBox의 너비와 높이를 개별적으로 오버라이드합니다.
	SizeBox_Icon->SetWidthOverride(Size.X);
	SizeBox_Icon->SetHeightOverride(Size.Y);
}

void UInv_Leaf_Image::SetImageSize(const FVector2D& Size) const
{
	// 이미지의 원하는 크기를 설정합니다.
	Image_Icon->SetDesiredSizeOverride(Size);
}

FVector2D UInv_Leaf_Image::GetImageSize() const
{
	// 현재 이미지의 원하는 크기를 반환합니다.
	return Image_Icon->GetDesiredSize();
}