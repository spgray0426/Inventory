// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inv_GridSlot.h"
#include "Inv_EquippedGridSlot.generated.h"

class UOverlay;
class UInv_EquippedSlottedItem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClicked, UInv_EquippedGridSlot*, GridSlot, const FGameplayTag&, EquipmentTypeTag);

/**
 * 장비 전용 그리드 슬롯 위젯 클래스
 *
 * 특정 장비 타입(무기, 방어구 등)을 장착할 수 있는 전용 슬롯입니다.
 * 일반 그리드 슬롯과 달리 특정 EquipmentTypeTag를 가지며,
 * 해당 타입의 장비만 장착할 수 있습니다.
 *
 * 주요 기능:
 * - 마우스 호버 시 시각적 피드백 제공
 * - 클릭 시 호버 아이템을 해당 슬롯에 장착
 * - 장착된 아이템을 시각적으로 표시 (EquippedSlottedItem 위젯 사용)
 */
UCLASS()
class INVENTORY_API UInv_EquippedGridSlot : public UInv_GridSlot
{
    GENERATED_BODY()
    
public:
	/**
	 * 마우스가 슬롯 위에 올라갔을 때 호출됩니다
	 * 시각적 하이라이트 효과를 표시하여 장착 가능 여부를 사용자에게 알립니다
	 * @param InGeometry 위젯의 지오메트리 정보
	 * @param InMouseEvent 마우스 이벤트 정보
	 */
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/**
	 * 마우스가 슬롯에서 벗어났을 때 호출됩니다
	 * 하이라이트 효과를 제거하고 슬롯을 원래 상태로 되돌립니다
	 * @param InMouseEvent 마우스 이벤트 정보
	 */
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	/**
	 * 슬롯이 클릭되었을 때 호출됩니다
	 * EquippedGridSlotClicked 델리게이트를 브로드캐스트하여
	 * 상위 위젯에서 장착 로직을 처리할 수 있도록 합니다
	 * @param InGeometry 위젯의 지오메트리 정보
	 * @param InMouseEvent 마우스 이벤트 정보
	 * @return 이벤트 처리 결과
	 */
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/**
	 * 아이템이 이 슬롯에 장착되었을 때 호출됩니다
	 * 장착된 아이템을 시각적으로 표시하는 EquippedSlottedItem 위젯을 생성하고
	 * 오버레이에 추가합니다
	 * @param Item 장착할 인벤토리 아이템
	 * @param EquipmentTag 장비 타입 태그 (예: GameItems.Equipment.Weapon)
	 * @param TileSize 타일 크기 (아이템 위젯의 크기 계산에 사용)
	 * @return 생성된 장착 아이템 위젯
	 */
    UInv_EquippedSlottedItem* OnItemEquipped(UInv_InventoryItem* Item, const FGameplayTag& EquipmentTag, float TileSize);

	/**
	 * 장착된 아이템 위젯을 설정합니다
	 * 슬롯이 현재 어떤 아이템 위젯을 표시하고 있는지 추적하기 위해 사용됩니다
	 * @param Item 설정할 장착 아이템 위젯
	 */
    void SetEquippedSlottedItem(UInv_EquippedSlottedItem* Item) { EquippedSlottedItem = Item; }

	/**
	 * 장비 슬롯이 클릭되었을 때 발동되는 델리게이트
	 * 파라미터:
	 * - UInv_EquippedGridSlot*: 클릭된 슬롯
	 * - FGameplayTag: 장비 타입 태그
	 */
    FEquippedGridSlotClicked EquippedGridSlotClicked;

private:
	/** 슬롯이 비어있을 때 표시되는 회색 아이콘 이미지 (블루프린트에서 바인딩됨) */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> Image_GrayedOutIcon;

	/** 장착 아이템 위젯을 포함하는 루트 오버레이 (블루프린트에서 바인딩됨) */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UOverlay> Overlay_Root;

	/**
	 * 이 슬롯에 장착할 수 있는 장비 타입 태그
	 * 예: GameItems.Equipment.Weapon, GameItems.Equipment.Helmet 등
	 * 호버 아이템의 타입이 이 태그와 일치해야만 장착 가능합니다
	 */
    UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "GameItems.Equipment"))
    FGameplayTag EquipmentTypeTag;

	/** 장착된 아이템을 표시할 위젯의 클래스 */
    UPROPERTY(EditAnywhere, Category = "Inventory")
    TSubclassOf<UInv_EquippedSlottedItem> EquippedSlottedItemClass;

	/** 현재 이 슬롯에 장착된 아이템 위젯 인스턴스 */
    UPROPERTY()
    TObjectPtr<UInv_EquippedSlottedItem> EquippedSlottedItem;
    
  
};
