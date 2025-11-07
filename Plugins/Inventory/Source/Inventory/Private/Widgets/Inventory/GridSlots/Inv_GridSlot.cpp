// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"
#include "Items/Inv_InventoryItem.h"

#include "Components/Image.h"
#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"

void UInv_GridSlot::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    Super::NativeOnMouseEnter(MyGeometry, MouseEvent);
    // 마우스가 슬롯에 들어올 때 GridSlotHovered 이벤트를 브로드캐스트하여 외부 시스템에 알립니다
    GridSlotHovered.Broadcast(TileIndex, MouseEvent);
}

void UInv_GridSlot::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
    Super::NativeOnMouseLeave(MouseEvent);
    // 마우스가 슬롯에서 나갈 때 GridSlotUnhovered 이벤트를 브로드캐스트하여 외부 시스템에 알립니다
    GridSlotUnhovered.Broadcast(TileIndex, MouseEvent);
}

FReply UInv_GridSlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    // 마우스 버튼이 눌렸을 때 GridSlotClicked 이벤트를 브로드캐스트합니다
    GridSlotClicked.Broadcast(TileIndex, MouseEvent);
	return FReply::Handled();
}

void UInv_GridSlot::SetInventoryItem(UInv_InventoryItem* Item)
{
    // 이 슬롯에 인벤토리 아이템을 할당합니다
    InventoryItem = Item;
}


void UInv_GridSlot::SetOccupiedTexture()
{
    // 슬롯 상태를 점유됨으로 변경하고 해당 브러시를 적용합니다
    GridSlotState = EInv_GridSlotState::Occupied;
    Image_GridSlot->SetBrush(Brush_Occupied);
}

void UInv_GridSlot::SetUnoccupiedTexture()
{
    // 슬롯 상태를 비어있음으로 변경하고 해당 브러시를 적용합니다
    GridSlotState = EInv_GridSlotState::Unoccupied;
    Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UInv_GridSlot::SetSelectedTexture()
{
    // 슬롯 상태를 선택됨으로 변경하고 해당 브러시를 적용합니다
    GridSlotState = EInv_GridSlotState::Selected;
    Image_GridSlot->SetBrush(Brush_Selected);
}

void UInv_GridSlot::SetGrayedOutTexture()
{
    // 슬롯 상태를 비활성화로 변경하고 해당 브러시를 적용합니다
    GridSlotState = EInv_GridSlotState::GrayedOut;
    Image_GridSlot->SetBrush(Brush_GrayedOut);
}

void UInv_GridSlot::SetItemPopUp(UInv_ItemPopUp* PopUp)
{
    ItemPopUp = PopUp;
    ItemPopUp->SetGridIndex(GetIndex());
    ItemPopUp->OnNativeDestruct.AddUObject(this, &ThisClass::OnItemPopUpDestruct);
}

UInv_ItemPopUp* UInv_GridSlot::GetItemPopUp() const
{
    return ItemPopUp.Get();
}

void UInv_GridSlot::OnItemPopUpDestruct(UUserWidget* Menu)
{
    ItemPopUp.Reset();
}
