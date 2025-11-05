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

	Button_Equippables->OnClicked.AddDynamic(this, &ThisClass::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);

	ShowEquippables();
}

/**
 * 주어진 아이템 컴포넌트가 인벤토리에 들어갈 공간이 있는지 확인합니다.
 *
 * @param ItemComponent 공간을 확인할 대상 아이템 컴포넌트입니다.
 * @return 인벤토리에 아이템을 넣을 수 있는지에 대한 결과를 반환합니다. 결과에는 사용 가능한 슬롯 정보, 남은 아이템 수량 등이 포함됩니다.
 */
FInv_SlotAvailabilityResult UInv_SpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComponent) const
{
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
	SetActiveGrid(Grid_Equippables, Button_Equippables);
}

void UInv_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables, Button_Consumables);
}

void UInv_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables, Button_Craftables);
}

void UInv_SpatialInventory::DisableButtons(UButton* Button)
{
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button)
{
	DisableButtons(Button);

	WidgetSwitcher->SetActiveWidget(Grid);
}
