// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inv_SlottedItem.h"
#include "Inv_EquippedSlottedItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedSlottedItemClicked, class UInv_EquippedSlottedItem*, SlottedItem);

/**
 * 장착된 아이템을 시각적으로 표시하는 슬롯 아이템 위젯 클래스
 *
 * UInv_SlottedItem을 상속받아 장비 슬롯에 장착된 아이템을 표시합니다.
 * 일반 슬롯 아이템과 달리 장비 타입 태그를 가지며,
 * 클릭 시 장비 해제 등의 특수한 동작을 수행할 수 있습니다.
 *
 * 주요 차이점:
 * - EquipmentTypeTag를 가지고 있음 (무기, 방어구 등)
 * - 전용 클릭 델리게이트 제공 (OnEquippedSlottedItemClicked)
 * - 장비 슬롯의 오버레이에 배치됨 (인벤토리 그리드가 아님)
 *
 * 사용 위치:
 * - UInv_EquippedGridSlot의 Overlay_Root에 자식으로 추가됨
 * - UInv_SpatialInventory에서 장비 장착/해제 시 생성/제거됨
 */
UCLASS()
class INVENTORY_API UInv_EquippedSlottedItem : public UInv_SlottedItem
{
    GENERATED_BODY()

public:
	/**
	 * 마우스 버튼이 눌렸을 때 호출됩니다
	 * OnEquippedSlottedItemClicked 델리게이트를 브로드캐스트하여
	 * 장비 해제 등의 상위 레벨 로직을 트리거합니다
	 * @param InGeometry 위젯의 지오메트리 정보
	 * @param InMouseEvent 마우스 이벤트 정보
	 * @return 이벤트 처리 결과
	 */
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/**
	 * 장비 타입 태그를 설정합니다
	 * 이 아이템이 어떤 종류의 장비인지를 나타냅니다 (무기, 투구, 갑옷 등)
	 * @param Tag 설정할 장비 타입 태그
	 */
    void SetEquipmentTypeTag(const FGameplayTag& Tag) { EquipmentTypeTag = Tag; }

	/**
	 * 장비 타입 태그를 가져옵니다
	 * @return 이 아이템의 장비 타입 태그
	 */
    FGameplayTag GetEquipmentTypeTag() const { return EquipmentTypeTag; }

	/**
	 * 장착된 아이템이 클릭되었을 때 발동되는 델리게이트
	 * 파라미터: UInv_EquippedSlottedItem* - 클릭된 장착 아이템 위젯
	 */
    FEquippedSlottedItemClicked OnEquippedSlottedItemClicked;

private:
	/**
	 * 이 장착 아이템의 장비 타입 태그
	 * 예: GameItems.Equipment.Weapon, GameItems.Equipment.Helmet 등
	 * 장비 해제 시 어느 슬롯에서 제거해야 하는지 판단하는 데 사용됩니다
	 */
    UPROPERTY()
    FGameplayTag EquipmentTypeTag;
};
