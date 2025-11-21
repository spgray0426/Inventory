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

/**
 * 마우스가 슬롯 위에 올라갔을 때 호출됩니다
 *
 * 호버 아이템이 이 슬롯에 장착 가능한지 확인하고,
 * 가능한 경우 시각적 피드백(하이라이트)을 제공합니다.
 *
 * 동작 순서:
 * 1. 슬롯이 사용 가능한 상태인지 확인 (비어있는지)
 * 2. 현재 호버 아이템이 있는지 확인
 * 3. 호버 아이템의 타입이 이 슬롯의 장비 타입과 일치하는지 확인
 * 4. 일치하면 점유 텍스처로 변경하고 회색 아이콘 숨김
 *
 * 시각적 피드백:
 * - SetOccupiedTexture(): 슬롯을 하이라이트 처리
 * - Image_GrayedOutIcon 숨김: 장착 가능함을 명확히 표시
 *
 * @param InGeometry 위젯의 지오메트리 정보
 * @param InMouseEvent 마우스 이벤트 정보
 */
void UInv_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 슬롯이 사용 가능한지 확인 (비어있는지)
    if (!IsAvailable()) return;
    // 현재 드래그 중인 호버 아이템을 가져옵니다
    UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
    if (!IsValid(HoverItem)) return;

    // 호버 아이템의 타입이 이 슬롯의 장비 타입과 일치하는지 확인
    // 예: Weapon 슬롯에는 Weapon 타입 아이템만 장착 가능
    if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
    {
        // 점유 텍스처로 변경하여 하이라이트 표시
        SetOccupiedTexture();
        // 회색 아이콘을 숨겨 장착 가능함을 명확히 표시
        Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
    }
}

/**
 * 마우스가 슬롯에서 벗어났을 때 호출됩니다
 *
 * NativeOnMouseEnter에서 적용한 시각적 피드백을 제거하고
 * 슬롯을 원래 상태로 되돌립니다.
 *
 * 동작 순서:
 * 1. 슬롯이 사용 가능한 상태인지 확인
 * 2. 호버 아이템이 있는지 확인
 * 3. 호버 아이템의 타입이 이 슬롯의 장비 타입과 일치하는지 확인
 * 4. 일치하면 비점유 텍스처로 변경하고 회색 아이콘 표시
 *
 * 시각적 피드백 제거:
 * - SetUnoccupiedTexture(): 하이라이트 제거
 * - Image_GrayedOutIcon 표시: 슬롯이 비어있음을 나타냄
 *
 * @param InMouseEvent 마우스 이벤트 정보
 */
void UInv_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    // 슬롯이 사용 가능한지 확인
    if (!IsAvailable()) return;
    // 호버 아이템을 가져옵니다
    UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
    if (!IsValid(HoverItem)) return;

    if (IsValid(EquippedSlottedItem)) return;
    
    // 호버 아이템의 타입이 이 슬롯의 장비 타입과 일치하는지 확인
    if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
    {
        // 비점유 텍스처로 변경하여 하이라이트 제거
        SetUnoccupiedTexture();
        // 회색 아이콘을 다시 표시하여 슬롯이 비어있음을 나타냄
        Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Visible);
    }
}

/**
 * 슬롯이 클릭되었을 때 호출됩니다
 *
 * EquippedGridSlotClicked 델리게이트를 브로드캐스트하여
 * 상위 위젯(UInv_SpatialInventory)에서 장착 로직을 처리하도록 합니다.
 *
 * 델리게이트 파라미터:
 * - this: 클릭된 슬롯 (어느 슬롯이 클릭되었는지)
 * - EquipmentTypeTag: 슬롯의 장비 타입 (어떤 종류의 장비를 장착해야 하는지)
 *
 * 델리게이트 리스너 (UInv_SpatialInventory::EquippedGridSlotClicked):
 * - 호버 아이템이 이 슬롯에 장착 가능한지 검증
 * - 장착 가능하면 OnItemEquipped() 호출하여 위젯 생성
 * - 서버에 장착 RPC 전송
 *
 * @param InGeometry 위젯의 지오메트리 정보
 * @param InMouseEvent 마우스 이벤트 정보
 * @return FReply::Handled() - 이벤트가 처리되었음을 알림
 */
FReply UInv_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 슬롯 클릭 이벤트를 브로드캐스트하여 상위 위젯에서 장착 로직 처리
    EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
    return FReply::Handled();
}

/**
 * 아이템이 이 슬롯에 장착되었을 때 호출됩니다
 *
 * 장착된 아이템을 시각적으로 표시하는 UInv_EquippedSlottedItem 위젯을 생성하고
 * 이 슬롯의 오버레이에 배치합니다.
 *
 * 동작 순서:
 * 1. 장비 타입 태그 검증 (일치하지 않으면 실패)
 * 2. 아이템의 그리드 크기 정보 가져오기
 * 3. 위젯의 그리기 크기 계산
 * 4. 장착 아이템 위젯 생성 및 설정
 * 5. 아이템 이미지 브러시 설정
 * 6. 오버레이에 위젯 추가 및 중앙 정렬
 *
 * 크기 계산:
 * - IconTileWidth = TileSize - GridPadding * 2
 * - DrawSize = GridDimensions * IconTileWidth
 * - 중앙 정렬을 위한 패딩 계산
 *
 * @param Item 장착할 인벤토리 아이템
 * @param EquipmentTag 장비 타입 태그 (검증용)
 * @param TileSize 타일 크기 (위젯 크기 계산에 사용)
 * @return 생성된 장착 아이템 위젯 (실패 시 nullptr)
 */
UInv_EquippedSlottedItem* UInv_EquippedGridSlot::OnItemEquipped(UInv_InventoryItem* Item, const FGameplayTag& EquipmentTag, float TileSize)
{

    // 장비 타입 태그 검증: 요청된 태그가 이 슬롯의 타입과 정확히 일치하는지 확인
    // 예: Weapon 슬롯에 Helmet을 장착하려고 하면 실패
    if (!EquipmentTag.MatchesTagExact(EquipmentTypeTag)) return nullptr;

    // 아이템의 그리드 프래그먼트를 가져와서 크기 정보 획득
    const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
    if (!GridFragment) return nullptr;
    const FIntPoint GridDimensions = GridFragment->GetGridSize();

    // 장착 아이템 위젯의 실제 그리기 크기 계산
    // TileSize에서 양쪽 패딩을 빼서 아이콘이 슬롯 안에 깔끔하게 들어가도록 함
    const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
    const FVector2D DrawSize = GridDimensions * IconTileWidth;

    // 장착 아이템 위젯 생성
    EquippedSlottedItem = CreateWidget<UInv_EquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);

    // 위젯에 인벤토리 아이템 데이터 설정
    EquippedSlottedItem->SetInventoryItem(Item);

    // 장비 타입 태그 설정 (장비 해제 시 필요)
    EquippedSlottedItem->SetEquipmentTypeTag(EquipmentTag);

    // 스택 개수 위젯 숨김 (장비는 스택 불가하므로 표시할 필요 없음)
    EquippedSlottedItem->UpdateStackCount(0);

    // 이 슬롯에 아이템 참조 저장 (슬롯이 점유되었음을 표시)
    SetInventoryItem(Item);

    // 아이템의 이미지 프래그먼트를 가져와서 아이콘 설정
    const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item, FragmentTags::IconFragment);
    if (!ImageFragment) return nullptr;

    // Slate 브러시 생성 및 설정 (아이템 이미지 표시용)
    FSlateBrush Brush;
    Brush.SetResourceObject(ImageFragment->GetIcon());
    Brush.DrawAs = ESlateBrushDrawType::Image;
    Brush.ImageSize = DrawSize;

    // 위젯에 이미지 브러시 적용
    EquippedSlottedItem->SetImageBrush(Brush);

    // 생성된 장착 아이템 위젯을 이 슬롯의 오버레이에 추가
    Overlay_Root->AddChildToOverlay(EquippedSlottedItem);

    // 오버레이의 지오메트리 정보를 가져와서 중앙 정렬을 위한 패딩 계산
    FGeometry OverlayGeometry = Overlay_Root->GetCachedGeometry();
    auto OverlayPos = OverlayGeometry.Position;
    auto OverlaySize = OverlayGeometry.Size;

    // 장착 아이템 위젯을 슬롯 중앙에 배치하기 위한 패딩 계산
    // 오버레이 크기의 절반에서 아이템 크기의 절반을 빼면 중앙 정렬됨
    const float LeftPadding = OverlaySize.X / 2.f - DrawSize.X / 2.f;
    const float TopPadding = OverlaySize.Y / 2.f - DrawSize.Y / 2.f;

    // 계산된 패딩을 오버레이 슬롯에 적용
    UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquippedSlottedItem);
    OverlaySlot->SetPadding(FMargin(LeftPadding, TopPadding));

    // 생성되고 설정된 장착 아이템 위젯 반환
    return EquippedSlottedItem;
}
