// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"

#include "Components/Image.h"

// 아래 함수들 스위치로 바꾸고 싶네
void UInv_GridSlot::SetOccupiedTexture()
{
    GridSlotState = EInv_GridSlotState::Occupied;
    Image_GridSlot->SetBrush(Brush_Occupied);
}

void UInv_GridSlot::SetUnoccupiedTexture()
{
    GridSlotState = EInv_GridSlotState::Unoccupied;
    Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UInv_GridSlot::SetSelectedTexture()
{
    GridSlotState = EInv_GridSlotState::Selected;
    Image_GridSlot->SetBrush(Brush_Selected);
}

void UInv_GridSlot::SetGrayedOutTexture()
{
    GridSlotState = EInv_GridSlotState::GrayedOut;
    Image_GridSlot->SetBrush(Brush_GrayedOut);
}
