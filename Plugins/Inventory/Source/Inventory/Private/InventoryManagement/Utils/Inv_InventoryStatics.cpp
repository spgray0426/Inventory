// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Utils/Inv_InventoryStatics.h"

#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"

UInv_InventoryComponent* UInv_InventoryStatics::GetInventoryComponent(const APlayerController* PlayerController)
{

	if (!IsValid(PlayerController)) return nullptr;
	
	return PlayerController->FindComponentByClass<UInv_InventoryComponent>();
}

EInv_ItemCategory UInv_InventoryStatics::GetItemCategoryFromItemComp(UInv_ItemComponent* ItemComp)
{
	if (!ItemComp) return EInv_ItemCategory::None;

	return ItemComp->GetItemManifest().GetItemCategory();
}

void UInv_InventoryStatics::ItemHovered(APlayerController* PC, UInv_InventoryItem* Item)
{
	// 플레이어 컨트롤러에서 인벤토리 컴포넌트를 가져옵니다
	UInv_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return;

	// 인벤토리 메뉴 위젯을 가져옵니다
	UInv_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return;

	// 현재 드래그 중인 아이템(HoverItem)이 있으면 설명을 표시하지 않습니다
	if (InventoryBase->HasHoverItem()) return;

	// 인벤토리 UI에 아이템 호버 이벤트를 전달하여 설명을 표시합니다
	InventoryBase->OnItemHovered(Item);
}

void UInv_InventoryStatics::ItemUnhovered(APlayerController* PC)
{
	// 플레이어 컨트롤러에서 인벤토리 컴포넌트를 가져옵니다
	UInv_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return;

	// 인벤토리 메뉴 위젯을 가져옵니다
	UInv_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return;

	// 인벤토리 UI에 마우스 떼기 이벤트를 전달하여 설명을 숨깁니다
	InventoryBase->OnItemUnHovered();
}

UInv_HoverItem* UInv_InventoryStatics::GetHoverItem(APlayerController* PC)
{
	UInv_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;

	UInv_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return nullptr;

	return InventoryBase->GetHoverItem();
	
}
