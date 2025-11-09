// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"
#include "Items/Inv_InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"

FReply UInv_SlottedItem::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// 슬롯 아이템 클릭 이벤트를 브로드캐스트합니다
	OnSlottedItemClicked.Broadcast(GridIndex, MouseEvent);
	return FReply::Handled();
}

void UInv_SlottedItem::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// 아이템에 마우스를 올렸을 때 정적 함수를 통해 아이템 설명 표시를 요청합니다
	UInv_InventoryStatics::ItemHovered(GetOwningPlayer(), InventoryItem.Get());
}

void UInv_SlottedItem::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	// 아이템에서 마우스를 뗐을 때 정적 함수를 통해 아이템 설명 숨김을 요청합니다
	UInv_InventoryStatics::ItemUnhovered(GetOwningPlayer());
}

void UInv_SlottedItem::SetInventoryItem(UInv_InventoryItem* Item)
{
	// 인벤토리 아이템에 대한 약한 참조를 설정합니다
	InventoryItem = Item;
}

void UInv_SlottedItem::SetImageBrush(const FSlateBrush& Brush) const
{
	// 아이콘 이미지의 브러시를 설정합니다
	Image_Icon->SetBrush(Brush);
}

void UInv_SlottedItem::UpdateStackCount(int32 StackCount)
{
	// 스택 개수가 0보다 크면 텍스트를 표시합니다
	if (StackCount > 0)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(StackCount));
	}
	// 스택 개수가 0이면 텍스트를 숨깁니다
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}
