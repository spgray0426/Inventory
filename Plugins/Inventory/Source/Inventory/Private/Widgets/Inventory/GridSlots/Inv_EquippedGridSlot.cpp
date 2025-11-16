// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/Inv_EquippedGridSlot.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_EquippedSlottedItem.h"

void UInv_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!IsAvailable()) return;
    UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
    if (!IsValid(HoverItem)) return;

    if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
    {
        SetOccupiedTexture();
        Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UInv_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    if (!IsAvailable()) return;
    UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
    if (!IsValid(HoverItem)) return;

    if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
    {
        SetUnoccupiedTexture();
        Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Visible);
    }
}

FReply UInv_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
    return FReply::Handled();
}

UInv_EquippedSlottedItem* UInv_EquippedGridSlot::OnItemEquipped(UInv_InventoryItem* Item, const FGameplayTag& EquipmentTag, float TileSize)
{
    
    // 장비 타입 태그 확인
    if (!EquipmentTag.MatchesTagExact(EquipmentTypeTag)) return nullptr;
    // 그리드 차원 가져오기
    const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
    if (!GridFragment) return nullptr;
    const FIntPoint GridDimensions = GridFragment->GetGridSize();

    // 장착된 슬롯 아이템의 그리기 크기 계산
    const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
    const FVector2D DrawSize = GridDimensions * IconTileWidth;
    // 장착된 슬롯 아이템 위젯 생성
    EquippedSlottedItem = CreateWidget<UInv_EquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);
    // 슬롯 아이템의 인벤토리 아이템 설정
    EquippedSlottedItem->SetInventoryItem(Item);
    // 슬롯 아이템의 장비 타입 태그 설정
    EquippedSlottedItem->SetEquipmentTypeTag(EquipmentTag);
    // 슬롯 아이템의 스택 개수 위젯 숨기기
    EquippedSlottedItem->UpdateStackCount(0);
    // 이 클래스(장착된 그리드 슬롯)에 인벤토리 아이템 설정
    SetInventoryItem(Item);
    // 장착된 슬롯 아이템의 이미지 브러시 설정
    const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item, FragmentTags::IconFragment);
    if (!ImageFragment) return nullptr;

    FSlateBrush Brush;
    Brush.SetResourceObject(ImageFragment->GetIcon());
    Brush.DrawAs = ESlateBrushDrawType::Image;
    Brush.ImageSize = DrawSize;
	
    EquippedSlottedItem->SetImageBrush(Brush);
    
    // 슬롯 아이템을 이 위젯의 오버레이에 자식으로 추가
    Overlay_Root->AddChildToOverlay(EquippedSlottedItem);
    FGeometry OverlayGeometry = Overlay_Root->GetCachedGeometry();
    auto OverlayPos = OverlayGeometry.Position;
    auto OverlaySize = OverlayGeometry.Size;

    const float LeftPadding = OverlaySize.X / 2.f - DrawSize.X / 2.f;
    const float TopPadding = OverlaySize.Y / 2.f - DrawSize.Y / 2.f;

    UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquippedSlottedItem);
    OverlaySlot->SetPadding(FMargin(LeftPadding, TopPadding));
	
    // 장착된 슬롯 아이템 위젯 반환
    return EquippedSlottedItem;
}
