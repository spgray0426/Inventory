// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/SlottedItems/Inv_EquippedSlottedItem.h"

/**
 * 마우스 버튼 다운 이벤트 처리
 *
 * 장착된 아이템이 클릭되었을 때 호출됩니다.
 * OnEquippedSlottedItemClicked 델리게이트를 브로드캐스트하여
 * 상위 위젯(UInv_SpatialInventory)에서 장비 해제 로직을 처리하도록 합니다.
 *
 * 델리게이트 리스너 (UInv_SpatialInventory::EquippedSlottedItemClicked):
 * - 클릭된 아이템을 호버 아이템으로 변환
 * - 장비 슬롯에서 아이템 참조 제거
 * - 이 위젯을 UI에서 제거
 *
 * @param InGeometry 위젯의 지오메트리 정보
 * @param InMouseEvent 마우스 이벤트 정보
 * @return FReply::Handled() - 이벤트가 처리되었음을 알림
 */
FReply UInv_EquippedSlottedItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 자기 자신(this)을 파라미터로 전달하여 델리게이트 브로드캐스트
    // 리스너는 어떤 장착 아이템이 클릭되었는지 알 수 있습니다
    OnEquippedSlottedItemClicked.Broadcast(this);
    return FReply::Handled();
}
