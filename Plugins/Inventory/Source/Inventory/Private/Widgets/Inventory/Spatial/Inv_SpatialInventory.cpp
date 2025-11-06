// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_SpatialInventory.h"

#include "Inventory.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"

void UInv_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 각 카테고리 버튼에 클릭 이벤트를 바인딩합니다
	Button_Equippables->OnClicked.AddDynamic(this, &ThisClass::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);

	// 기본적으로 장비 그리드를 표시합니다
	ShowEquippables();
}

FInv_SlotAvailabilityResult UInv_SpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComponent) const
{
	// 아이템 카테고리에 따라 해당 그리드에서 공간을 확인합니다
	switch (UInv_InventoryStatics::GetItemCategoryFromItemComp(ItemComponent)) {
	case EInv_ItemCategory::Equippable:
		return Grid_Equippables->HasRoomForItem(ItemComponent);
	case EInv_ItemCategory::Consumable:
		return Grid_Consumables->HasRoomForItem(ItemComponent);
	case EInv_ItemCategory::Craftable:
		return Grid_Craftables->HasRoomForItem(ItemComponent);
	default:
		UE_LOG(LogInventory, Error, TEXT("ItemComponent doesn't have a valid Item Category."));
		return FInv_SlotAvailabilityResult();
	}

}

void UInv_SpatialInventory::ShowEquippables()
{
	// 장비 그리드를 활성화합니다
	SetActiveGrid(Grid_Equippables, Button_Equippables);
}

void UInv_SpatialInventory::ShowConsumables()
{
	// 소비품 그리드를 활성화합니다
	SetActiveGrid(Grid_Consumables, Button_Consumables);
}

void UInv_SpatialInventory::ShowCraftables()
{
	// 제작 재료 그리드를 활성화합니다
	SetActiveGrid(Grid_Craftables, Button_Craftables);
}

void UInv_SpatialInventory::DisableButtons(UButton* Button)
{
	// 모든 버튼을 활성화합니다
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	// 현재 선택된 버튼만 비활성화합니다
	Button->SetIsEnabled(false);
}

void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button)
{
	// 이전 그리드의 커서를 숨깁니다
	if (ActiveGrid.IsValid()) ActiveGrid->HideCursor();
	// 새로운 그리드를 활성화합니다
	ActiveGrid = Grid;
	// 새 그리드의 커서를 표시합니다
	if (ActiveGrid.IsValid()) ActiveGrid->ShowCursor();

	// 버튼 상태를 업데이트하고 위젯 스위처로 그리드를 전환합니다
	DisableButtons(Button);
	WidgetSwitcher->SetActiveWidget(Grid);
}
