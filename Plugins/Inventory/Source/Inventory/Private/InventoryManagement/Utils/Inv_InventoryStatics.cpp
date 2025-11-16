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

/**
 * 플레이어 컨트롤러의 호버 아이템 위젯을 가져옵니다
 *
 * 인벤토리 컴포넌트 → 인벤토리 메뉴 → 호버 아이템 순서로 접근하여
 * 현재 마우스 커서를 따라다니는 호버 아이템 위젯을 반환합니다.
 *
 * @param PC 플레이어 컨트롤러
 * @return 호버 아이템 위젯 (유효하지 않으면 nullptr)
 */
UInv_HoverItem* UInv_InventoryStatics::GetHoverItem(APlayerController* PC)
{
	// 인벤토리 컴포넌트를 가져옵니다
	UInv_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;

	// 인벤토리 메뉴 위젯을 가져옵니다
	UInv_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return nullptr;

	// 호버 아이템 위젯을 반환합니다
	return InventoryBase->GetHoverItem();

}

/**
 * 플레이어 컨트롤러의 인벤토리 위젯을 가져옵니다
 *
 * 인벤토리 컴포넌트에서 메인 인벤토리 UI 위젯을 반환합니다.
 * 이 위젯은 그리드, 호버 아이템, 장착 슬롯 등을 포함하는 최상위 인벤토리 UI입니다.
 *
 * @param PC 플레이어 컨트롤러
 * @return 인벤토리 베이스 위젯 (유효하지 않으면 nullptr)
 */
UInv_InventoryBase* UInv_InventoryStatics::GetInventoryWidget(APlayerController* PC)
{
	// 인벤토리 컴포넌트를 가져옵니다
	UInv_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;

	// 인벤토리 메뉴 위젯을 반환합니다
	return IC->GetInventoryMenu();
}
