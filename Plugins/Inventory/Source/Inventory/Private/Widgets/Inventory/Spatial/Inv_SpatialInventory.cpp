// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_SpatialInventory.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Inventory/GridSlots/Inv_EquippedGridSlot.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_EquippedSlottedItem.h"
#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"

/**
 * 위젯 초기화 함수
 *
 * 실행 순서:
 * 1. 부모 클래스의 초기화 로직 실행
 * 2. 각 카테고리 버튼에 클릭 이벤트 핸들러 바인딩
 *    - 장비 버튼 -> ShowEquippables()
 *    - 소비품 버튼 -> ShowConsumables()
 *    - 제작 재료 버튼 -> ShowCraftables()
 * 3. 각 그리드에 캔버스 패널 설정 (호버 아이템 표시용)
 * 4. 기본 그리드로 장비 그리드를 표시
 */
void UInv_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 각 카테고리 버튼에 클릭 이벤트를 바인딩합니다
	Button_Equippables->OnClicked.AddDynamic(this, &ThisClass::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);

	// 각 그리드에 소유 캔버스를 설정합니다 (호버 아이템을 표시하기 위함)
	Grid_Equippables->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);

	// 기본적으로 장비 그리드를 표시합니다
	ShowEquippables();
	
	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		UInv_EquippedGridSlot* EquippedGridSlot = Cast<UInv_EquippedGridSlot>(Widget);
		if (IsValid(EquippedGridSlot))
		{
			EquippedGridSlots.Add(EquippedGridSlot);
			EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this, &ThisClass::EquippedGridSlotClicked);
		}
	});
}

/**
 * 마우스 버튼 다운 이벤트 처리
 *
 * 그리드 외부 영역을 클릭했을 때 드래그 중인 아이템을 드롭합니다
 * 이를 통해 사용자가 인벤토리 외부를 클릭하여 아이템을 버릴 수 있습니다
 *
 * @return FReply::Handled()를 반환하여 이벤트가 처리되었음을 알립니다
 */
FReply UInv_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// 현재 활성화된 그리드에서 호버 아이템을 드롭합니다
	ActiveGrid->DropItem();
	return FReply::Handled();
}

/**
 * 매 프레임마다 호출되는 틱 함수
 *
 * 아이템 설명 위젯이 표시 중일 때 마우스 커서를 따라다니도록
 * 위치를 지속적으로 업데이트합니다
 *
 * 이를 통해 마우스를 움직여도 설명 위젯이 항상 커서 근처에 표시됩니다
 */
void UInv_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 아이템 설명 위젯이 생성되어 있으면 매 프레임 위치를 업데이트합니다
	// 마우스 커서를 따라다니도록 하기 위함입니다
	if (!IsValid(ItemDescription)) return;
	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);
	SetEquippedItemDescriptionSizeAndPosition(ItemDescription, EquippedItemDescription, CanvasPanel);
}

/**
 * 아이템을 배치할 공간이 있는지 확인합니다
 *
 * 아이템의 카테고리에 따라 해당하는 그리드에서 공간을 체크합니다:
 * - Equippable: 장비 그리드
 * - Consumable: 소비품 그리드
 * - Craftable: 제작 재료 그리드
 *
 * 카테고리가 유효하지 않으면 에러를 로그에 출력하고 빈 결과를 반환합니다
 *
 * @param ItemComponent 공간을 확인할 아이템 컴포넌트
 * @return 슬롯 사용 가능 여부와 상세 정보를 담은 결과 구조체
 */
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

/**
 * 아이템에 마우스를 올렸을 때 호출되는 함수
 *
 * 동작 순서:
 * 1. 아이템의 매니페스트를 가져옵니다
 * 2. 아이템 설명 위젯을 가져오거나 생성합니다 (지연 초기화)
 * 3. 위젯을 일단 숨김 상태로 설정합니다
 * 4. 이전에 설정된 타이머가 있으면 취소합니다
 * 5. 지연 타이머를 설정하여 일정 시간 후에 설명을 표시합니다
 *
 * 지연 타이머를 사용하는 이유:
 * - 마우스를 빠르게 움직일 때는 설명이 표시되지 않도록 함
 * - UX 개선: 사용자가 특정 아이템에 잠시 머물렀을 때만 표시
 *
 * @param Item 마우스를 올린 아이템
 */
void UInv_SpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	// 아이템 설명 위젯을 가져옵니다 (없으면 생성)
	UInv_ItemDescription* DescriptionWidget = GetItemDescription();
	// 일단 숨김 상태로 설정합니다
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);

	// 이전 타이머가 있다면 정리합니다
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);

	// 지연 후에 설명을 표시하는 람다를 타이머 델리게이트에 바인딩합니다
	// 이를 통해 마우스를 빠르게 움직일 때는 설명이 표시되지 않습니다
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this, Item ,&Manifest, DescriptionWidget]()
	{
		// HitTestInvisible로 설정하여 마우스 이벤트를 방해하지 않으면서 보이도록 합니다
		GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
		// 매니페스트의 모든 인벤토리 프래그먼트를 위젯에 동화시킵니다
		Manifest.AssimilateInventoryFragments(DescriptionWidget);
		
		FTimerDelegate EquippedDescriptionTimerDelegate;
		EquippedDescriptionTimerDelegate.BindUObject(this, &ThisClass::ShowEquippedItemDescription, Item);
		GetOwningPlayer()->GetWorldTimerManager().SetTimer(EquippedDescriptionTimer, EquippedDescriptionTimerDelegate, EquippedDescriptionTimerDelay, false);
	});

	// 설정된 지연 시간 후에 타이머를 실행합니다 (한 번만)
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimer, DescriptionTimerDelegate, DescriptionTimerDelay, false);
}

/**
 * 아이템에서 마우스를 뗐을 때 호출되는 함수
 *
 * 동작:
 * 1. 아이템 설명 위젯을 즉시 숨깁니다
 * 2. 대기 중인 타이머를 취소합니다 (아직 표시되지 않았을 경우)
 *
 * 이를 통해 아이템 설명이 더 이상 필요 없을 때 즉시 사라지고,
 * 표시 예정이었던 타이머도 정리됩니다
 */
void UInv_SpatialInventory::OnItemUnHovered()
{
	// 아이템 설명을 즉시 숨깁니다
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	// 실행 대기 중인 타이머를 취소합니다
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
	
	GetEquippedItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(EquippedDescriptionTimer);
}

/**
 * 현재 드래그 중인 호버 아이템이 있는지 확인합니다
 *
 * 세 개의 그리드(장비, 소비품, 제작 재료)를 모두 검사하여
 * 하나라도 호버 아이템이 있으면 true를 반환합니다
 *
 * 호버 아이템은 사용자가 드래그 중인 아이템을 의미하며,
 * 이 정보는 인벤토리 닫기 방지 등에 사용됩니다
 *
 * @return 호버 아이템이 있으면 true, 없으면 false
 */
bool UInv_SpatialInventory::HasHoverItem() const
{
	// 세 개의 그리드 중 하나라도 호버 아이템(드래그 중인 아이템)이 있으면 true를 반환합니다
	if (Grid_Equippables->HasHoverItem()) return true;
	if (Grid_Consumables->HasHoverItem()) return true;
	if (Grid_Craftables->HasHoverItem()) return true;
	return false;
}

/**
 * 현재 활성화된 그리드의 호버 아이템 위젯을 가져옵니다
 *
 * 활성 그리드가 유효하지 않으면 nullptr을 반환합니다.
 *
 * @return 호버 아이템 위젯 (활성 그리드가 없으면 nullptr)
 */
UInv_HoverItem* UInv_SpatialInventory::GetHoverItem() const
{
	// 활성 그리드가 유효한지 확인합니다
	if (!ActiveGrid.IsValid()) return nullptr;

	// 활성 그리드의 호버 아이템을 반환합니다
	return ActiveGrid->GetHoverItem();
}

/**
 * 현재 활성화된 그리드의 타일 크기를 가져옵니다
 *
 * 타일 크기는 그리드의 각 셀이 차지하는 픽셀 단위 크기입니다.
 * 장착 아이템의 크기를 계산하는 데 사용됩니다.
 *
 * @return 타일 크기 (픽셀 단위, 활성 그리드가 없으면 0)
 */
float UInv_SpatialInventory::GetTileSize() const
{
	// 활성 그리드가 유효한지 확인합니다
	if (!ActiveGrid.IsValid()) return 0.f;

	// 활성 그리드의 타일 크기를 반환합니다
	return ActiveGrid->GetTileSize();
}

/**
 * 장비 그리드를 표시합니다
 *
 * 버튼 클릭 이벤트에 바인딩되어 있으며,
 * SetActiveGrid를 호출하여 장비 그리드로 전환합니다
 */
void UInv_SpatialInventory::ShowEquippables()
{
	// 장비 그리드를 활성화합니다
	SetActiveGrid(Grid_Equippables, Button_Equippables);
}

/**
 * 소비품 그리드를 표시합니다
 *
 * 버튼 클릭 이벤트에 바인딩되어 있으며,
 * SetActiveGrid를 호출하여 소비품 그리드로 전환합니다
 */
void UInv_SpatialInventory::ShowConsumables()
{
	// 소비품 그리드를 활성화합니다
	SetActiveGrid(Grid_Consumables, Button_Consumables);
}

/**
 * 제작 재료 그리드를 표시합니다
 *
 * 버튼 클릭 이벤트에 바인딩되어 있으며,
 * SetActiveGrid를 호출하여 제작 재료 그리드로 전환합니다
 */
void UInv_SpatialInventory::ShowCraftables()
{
	// 제작 재료 그리드를 활성화합니다
	SetActiveGrid(Grid_Craftables, Button_Craftables);
}

/**
 * 장착 그리드 슬롯이 클릭되었을 때 호출됩니다
 *
 * 호버 아이템을 장착 슬롯에 장착하는 전체 프로세스를 처리합니다:
 * 1. 호버 아이템이 해당 슬롯에 장착 가능한지 확인
 * 2. 타일 크기를 가져와서 장착 아이템 위젯 생성
 * 3. 장착 아이템 클릭 이벤트 바인딩
 * 4. 그리드에서 호버 아이템 제거
 * 5. 서버에 아이템 장착 RPC 요청 전송
 * 6. 로컬 클라이언트에서 장착 이벤트 브로드캐스트 (UI 업데이트)
 *
 * @param EquippedGridSlot 클릭된 장착 슬롯
 * @param EquipmentTypeTag 장비 타입 태그 (예: Weapon, Armor 등)
 */
void UInv_SpatialInventory::EquippedGridSlotClicked(UInv_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag)
{
	// 호버 아이템을 장착할 수 있는지 확인합니다
	if (!CanEquipHoverItem(EquippedGridSlot, EquipmentTypeTag)) return;

	// 호버 아이템을 가져옵니다
	UInv_HoverItem* HoverItem = GetHoverItem();

	// 타일 크기를 가져옵니다 (장착 아이템 크기 계산에 사용)
	const float TileSize = UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize();

	// 장착 슬롯에 아이템을 장착하고 장착 아이템 위젯을 생성합니다
	UInv_EquippedSlottedItem* EquippedSlottedItem = EquippedGridSlot->OnItemEquipped(
		HoverItem->GetInventoryItem(),
		EquipmentTypeTag,
		TileSize
	);

	// 장착 아이템 클릭 이벤트를 바인딩합니다
	EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	

	// 인벤토리 컴포넌트를 가져옵니다
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	// 서버에 아이템 장착 요청을 보냅니다
	// nullptr은 이전 장착 아이템이 없음을 의미합니다 (교체가 아닌 신규 장착)
	InventoryComponent->Server_EquipSlotClicked(HoverItem->GetInventoryItem(), nullptr);

	// 데디케이티드 서버가 아닌 경우 (로컬 클라이언트 또는 리슨 서버)
	// 아이템 장착 이벤트를 브로드캐스트하여 UI 업데이트를 트리거합니다
	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(HoverItem->GetInventoryItem());
	}
	
	// 호버 아이템을 그리드에서 제거합니다 (아이템이 장착되었으므로)
	Grid_Equippables->ClearHoverItem();
}

/**
 * 장착된 아이템이 클릭되었을 때 호출됩니다
 *
 * 장착된 아이템을 해제하거나, 호버 아이템이 있는 경우 교체합니다.
 *
 * 동작 흐름:
 * 1. 아이템 설명 위젯 숨김
 * 2. 호버 아이템이 스택 가능하면 중단 (장비는 스택 불가)
 * 3. 장착할 아이템과 해제할 아이템 식별
 * 4. 기존 장착 아이템이 있던 슬롯 찾기
 * 5. 슬롯을 비우고 기존 아이템을 호버 아이템으로 변환
 * 6. 기존 장착 위젯 제거
 * 7. 새 아이템이 있으면 장착 (교체)
 * 8. 서버에 알림 및 델리게이트 브로드캐스트
 *
 * 사용 시나리오:
 * - 장착된 무기 클릭 (호버 없음): 무기 해제하여 인벤토리로
 * - 새 무기를 들고 장착된 무기 클릭: 기존 무기 해제 후 새 무기 장착
 *
 * @param EquippedSlottedItem 클릭된 장착 아이템 위젯
 */
void UInv_SpatialInventory::EquippedSlottedItemClicked(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	// 아이템 설명 위젯을 숨깁니다
	UInv_InventoryStatics::ItemUnhovered(GetOwningPlayer());

	// 호버 아이템이 스택 가능하면 중단 (장비 슬롯에는 스택 가능 아이템 장착 불가)
	if (IsValid(GetHoverItem()) && GetHoverItem()->IsStackable()) return;

	// 장착할 아이템 가져오기 (호버 아이템이 있으면 그것을, 없으면 nullptr)
	UInv_InventoryItem* ItemToEquip = IsValid(GetHoverItem()) ? GetHoverItem()->GetInventoryItem() : nullptr;

	// 해제할 아이템 가져오기 (현재 클릭된 장착 아이템)
	UInv_InventoryItem* ItemToUnequip = EquippedSlottedItem->GetInventoryItem();

	// 해제할 아이템이 장착되어 있던 슬롯 찾기
	UInv_EquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);

	// 슬롯을 비웁니다 (인벤토리 아이템 참조를 nullptr로 설정)
	ClearSlotOfItem(EquippedGridSlot);

	// 해제된 아이템을 호버 아이템으로 만듭니다 (마우스 커서를 따라다니도록)
	Grid_Equippables->AssignHoverItem(ItemToUnequip);

	// 기존 장착 아이템 위젯을 UI에서 제거합니다
	RemoveEquippedSlottedItem(EquippedSlottedItem);

	// 새로운 아이템을 장착합니다 (ItemToEquip이 nullptr이면 장착만 해제됨)
	MakeEquippedSlottedItem(EquippedSlottedItem, EquippedGridSlot, ItemToEquip);

	// 서버에 알리고 델리게이트를 브로드캐스트합니다
	BroadcastSlotClickedDelegates(ItemToEquip, ItemToUnequip);
}

/**
 * 버튼 상태를 업데이트합니다
 *
 * 모든 카테고리 버튼을 활성화한 후,
 * 현재 선택된 버튼만 비활성화합니다
 *
 * 이를 통해 사용자가 현재 어떤 탭이 선택되었는지 시각적으로 확인할 수 있습니다
 *
 * @param Button 비활성화할 버튼 (현재 선택된 탭의 버튼)
 */
void UInv_SpatialInventory::DisableButtons(UButton* Button)
{
	// 모든 버튼을 활성화합니다
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	// 현재 선택된 버튼만 비활성화합니다
	Button->SetIsEnabled(false);
}

/**
 * 활성 그리드를 설정하고 UI를 전환합니다
 *
 * 실행 순서:
 * 1. 이전 활성 그리드의 커서를 숨깁니다
 * 2. 새 그리드를 활성 그리드로 설정합니다
 * 3. 새 그리드의 커서를 표시합니다
 * 4. 버튼 상태를 업데이트합니다 (선택된 버튼 비활성화)
 * 5. 위젯 스위처를 통해 새 그리드를 표시합니다
 *
 * @param Grid 활성화할 인벤토리 그리드
 * @param Button 관련된 탭 버튼
 */
void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button)
{
	// 이전 그리드의 커서를 숨깁니다
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->HideCursor();
		ActiveGrid->OnHide();
	}
	// 새로운 그리드를 활성화합니다
	ActiveGrid = Grid;
	// 새 그리드의 커서를 표시합니다
	if (ActiveGrid.IsValid()) ActiveGrid->ShowCursor();

	// 버튼 상태를 업데이트하고 위젯 스위처로 그리드를 전환합니다
	DisableButtons(Button);
	WidgetSwitcher->SetActiveWidget(Grid);
}

/**
 * 아이템 설명 위젯의 크기와 위치를 설정합니다
 *
 * 동작 순서:
 * 1. 캔버스 패널 슬롯을 가져옵니다
 * 2. 위젯의 동적 크기를 계산하여 설정합니다
 * 3. 마우스 커서 위치를 가져옵니다
 * 4. 화면 경계 내에서 위치를 보정합니다 (화면 밖으로 나가지 않도록)
 * 5. 계산된 위치를 캔버스 슬롯에 적용합니다
 *
 * 이 함수는 NativeTick에서 매 프레임 호출되어
 * 마우스 커서를 따라다니는 툴팁 효과를 구현합니다
 *
 * @param Description 위치를 설정할 아이템 설명 위젯
 * @param Canvas 위젯을 포함하는 캔버스 패널
 */
void UInv_SpatialInventory::SetItemDescriptionSizeAndPosition(UInv_ItemDescription* Description, UCanvasPanel* Canvas) const
{
	// 캔버스 패널 슬롯을 가져옵니다
	UCanvasPanelSlot* ItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCPS)) return;

	// 설명 위젯의 크기를 가져와서 설정합니다
	const FVector2D ItemDescriptionSize = Description->GetBoxSize();
	ItemDescriptionCPS->SetSize(ItemDescriptionSize);

	// 마우스 커서 위치를 기준으로 화면 경계 내에서 위치를 보정합니다
	// 이렇게 하면 설명 위젯이 화면 밖으로 나가지 않습니다
	FVector2D ClampedPosition = UInv_WidgetUtils::GetClampedWidgetPosition(
		UInv_WidgetUtils::GetWidgetSize(Canvas),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer()));

	// 계산된 위치를 설정합니다
	ItemDescriptionCPS->SetPosition(ClampedPosition);
}

/**
 * 호버 아이템을 지정된 장비 슬롯에 장착할 수 있는지 검증합니다
 *
 * 장착 가능 조건 (모두 만족해야 함):
 * 1. 슬롯이 유효하고 비어있어야 함 (이미 장착된 아이템이 없어야 함)
 * 2. 호버 아이템이 존재해야 함
 * 3. 들고 있는 아이템이 유효해야 함
 * 4. 호버 아이템이 스택 가능한 아이템이 아니어야 함 (장비는 스택 불가)
 * 5. 아이템의 카테고리가 Equippable이어야 함
 * 6. 아이템의 타입 태그가 슬롯의 장비 타입 태그와 일치해야 함
 *    (예: Weapon 슬롯에는 Weapon 태그를 가진 아이템만 장착 가능)
 *
 * @param EquippedGridSlot 확인할 장비 슬롯
 * @param EquipmentTypeTag 슬롯이 요구하는 장비 타입 태그
 * @return 모든 조건을 만족하면 true, 하나라도 만족하지 않으면 false
 */
bool UInv_SpatialInventory::CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const
{
	// 슬롯이 유효하지 않거나 이미 아이템이 장착되어 있으면 장착 불가
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid()) return false;

	// 호버 아이템을 가져옵니다
	UInv_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;

	// 호버 아이템의 실제 인벤토리 아이템을 가져옵니다
	UInv_InventoryItem* HeldItem = HoverItem->GetInventoryItem();

	// 모든 장착 조건을 확인합니다
	return HasHoverItem() && IsValid(HeldItem) &&
		!HoverItem->IsStackable() &&
			HeldItem->GetItemManifest().GetItemCategory() == EInv_ItemCategory::Equippable &&
				HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag);
}

/**
 * 특정 아이템이 장착된 슬롯을 찾습니다
 *
 * EquippedGridSlots 배열을 순회하며 지정된 아이템을 포함하는 슬롯을 찾습니다.
 * FindByPredicate를 사용하여 효율적으로 검색합니다.
 *
 * 사용 시나리오:
 * - 장비 해제: 어느 슬롯에서 아이템을 제거해야 하는지 확인
 * - 장비 교체: 같은 타입의 다른 장비로 교체할 때 기존 슬롯 찾기
 *
 * @param EquippedItem 찾을 장착된 인벤토리 아이템
 * @return 아이템이 장착된 슬롯 포인터 (찾지 못하면 nullptr)
 */
UInv_EquippedGridSlot* UInv_SpatialInventory::FindSlotWithEquippedItem(UInv_InventoryItem* EquippedItem) const
{
	// 람다 함수를 사용하여 EquippedItem과 일치하는 아이템을 가진 슬롯을 찾습니다
	auto* FoundEquippedGridSlot = EquippedGridSlots.FindByPredicate([EquippedItem](const UInv_EquippedGridSlot* GridSlot)
	{
		return GridSlot->GetInventoryItem() == EquippedItem;
	});
	// 찾은 경우 해당 슬롯을 반환하고, 찾지 못한 경우 nullptr을 반환합니다
	return FoundEquippedGridSlot ? *FoundEquippedGridSlot : nullptr;
}

/**
 * 장비 슬롯을 비웁니다 (아이템 해제)
 *
 * 슬롯의 모든 아이템 정보를 nullptr로 설정하여 초기화합니다:
 * - 장착 아이템 위젯 참조 제거
 * - 인벤토리 아이템 참조 제거
 *
 * 이 함수는 슬롯을 비우기만 하며, 시각적인 위젯 제거는 하지 않습니다.
 * 위젯 제거는 RemoveEquippedSlottedItem()에서 처리합니다.
 *
 * 장비 해제 흐름에서의 역할:
 * 1. FindSlotWithEquippedItem() - 슬롯 찾기
 * 2. ClearSlotOfItem() - 슬롯 비우기 (이 함수)
 * 3. RemoveEquippedSlottedItem() - 위젯 제거
 *
 * @param EquippedGridSlot 비울 장비 슬롯
 */
void UInv_SpatialInventory::ClearSlotOfItem(UInv_EquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		// 장착 아이템 위젯 참조를 제거합니다
		EquippedGridSlot->SetEquippedSlottedItem(nullptr);
		// 인벤토리 아이템 참조를 제거합니다
		EquippedGridSlot->SetInventoryItem(nullptr);
	}
}

/**
 * 장착 아이템 위젯을 UI에서 제거하고 정리합니다
 *
 * 동작 순서:
 * 1. 위젯의 유효성을 확인합니다
 * 2. 클릭 이벤트 델리게이트가 바인딩되어 있으면 해제합니다
 * 3. 위젯을 부모 위젯(오버레이)에서 제거하여 화면에서 사라지게 합니다
 *
 * 델리게이트 바인딩 해제의 중요성:
 * - 메모리 누수 방지
 * - 삭제된 위젯을 통한 이벤트 호출 방지
 * - Unreal의 델리게이트 시스템에서 권장하는 정리 패턴
 *
 * 장비 해제 흐름에서의 역할:
 * 1. FindSlotWithEquippedItem() - 슬롯 찾기
 * 2. ClearSlotOfItem() - 슬롯 비우기
 * 3. RemoveEquippedSlottedItem() - 위젯 제거 (이 함수)
 *
 * @param EquippedSlottedItem 제거할 장착 아이템 위젯
 */
void UInv_SpatialInventory::RemoveEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	if (!IsValid(EquippedSlottedItem)) return;

	// 클릭 델리게이트가 이미 바인딩되어 있는지 확인하고 해제합니다
	// 메모리 누수와 댕글링 포인터 문제를 방지하기 위함입니다
	if (EquippedSlottedItem->OnEquippedSlottedItemClicked.IsAlreadyBound(this, &ThisClass::EquippedSlottedItemClicked))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.RemoveDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}
	// 부모 위젯(오버레이)에서 제거하여 화면에서 사라지게 합니다
	EquippedSlottedItem->RemoveFromParent();
}

/**
 * 장착 슬롯에 새로운 장착 아이템 위젯을 생성합니다
 *
 * 장비 교체 프로세스의 일부로, 기존 장착 아이템 위젯이 제거된 후
 * 새로운 아이템을 같은 슬롯에 장착하기 위해 호출됩니다.
 *
 * 동작 순서:
 * 1. 슬롯 유효성 검사
 * 2. 슬롯의 OnItemEquipped() 호출하여 새 위젯 생성
 *    - 기존 장착 아이템의 장비 타입 태그 재사용 (같은 슬롯이므로)
 *    - 타일 크기는 인벤토리 위젯에서 가져옴
 * 3. 새 위젯에 클릭 이벤트 바인딩
 * 4. 슬롯에 새 위젯 참조 저장
 *
 * 중요:
 * - ItemToEquip이 nullptr이면 슬롯은 비어있는 상태로 유지됨
 * - 기존 장착 아이템의 장비 타입 태그를 사용하여 같은 타입 슬롯에 배치
 *
 * @param EquippedSlottedItem 기존 장착 아이템 위젯 (장비 타입 태그 참조용)
 * @param EquippedGridSlot 아이템을 장착할 슬롯
 * @param ItemToEquip 장착할 새 인벤토리 아이템 (nullptr 가능)
 */
void UInv_SpatialInventory::MakeEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem, UInv_EquippedGridSlot* EquippedGridSlot, UInv_InventoryItem* ItemToEquip)
{
	// 슬롯이 유효한지 확인
	if (!IsValid(EquippedGridSlot)) return;

	// 슬롯에 새 아이템을 장착하고 위젯 생성
	// 기존 장착 아이템의 장비 타입 태그를 재사용합니다 (같은 슬롯이므로)
	UInv_EquippedSlottedItem* SlottedItem = EquippedGridSlot->OnItemEquipped(
		ItemToEquip,
		EquippedSlottedItem->GetEquipmentTypeTag(), // 기존 아이템의 타입 태그 (Weapon, Helmet 등)
		UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize()
		);

	// 새 위젯에 클릭 이벤트를 바인딩합니다
	// 이를 통해 사용자가 장착된 아이템을 다시 클릭하여 해제할 수 있습니다
	if (IsValid(SlottedItem)) SlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);

	// 슬롯에 새 위젯 참조를 저장합니다
	 EquippedGridSlot->SetEquippedSlottedItem(SlottedItem);
}

/**
 * 장비 장착/해제 델리게이트를 브로드캐스트합니다
 *
 * 장비 상태 변경을 서버와 다른 시스템에 알리기 위한 함수입니다.
 * 네트워크 멀티플레이어를 고려하여 서버 RPC와 로컬 델리게이트를
 * 적절히 처리합니다.
 *
 * 동작:
 * 1. 인벤토리 컴포넌트 가져오기
 * 2. 서버 RPC 호출 (모든 네트워크 모드에서)
 *    - Server_EquipSlotClicked: 서버에 장비 변경 알림
 *    - 서버는 이를 처리하고 모든 클라이언트에 복제
 * 3. UI 델리게이트 브로드캐스트 (데디케이티드 서버 제외)
 *    - OnItemEquipped: 새 아이템 장착 알림 (UI 업데이트용)
 *    - OnItemUnequipped: 기존 아이템 해제 알림 (UI 업데이트용)
 *
 * 네트워크 고려사항:
 * - 데디케이티드 서버는 UI가 없으므로 UI 델리게이트 불필요
 * - 리슨 서버와 클라이언트는 로컬 UI 업데이트를 위해 델리게이트 필요
 * - Server_EquipSlotClicked는 항상 호출되어야 서버가 상태를 동기화할 수 있음
 *
 * @param ItemToEquip 장착할 아이템 (nullptr이면 장착만 해제)
 * @param ItemToUnequip 해제할 아이템 (nullptr이면 단순 장착)
 */
void UInv_SpatialInventory::BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip) const
{
	// 인벤토리 컴포넌트를 가져옵니다
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	// 서버에 장비 변경을 알립니다 (모든 네트워크 모드에서 호출)
	// 서버는 이 정보를 받아 모든 클라이언트에 복제합니다
	InventoryComponent->Server_EquipSlotClicked(ItemToEquip, ItemToUnequip);

	// 데디케이티드 서버가 아닌 경우 (리슨 서버 또는 클라이언트)
	// 로컬 UI 업데이트를 위해 델리게이트를 브로드캐스트합니다
	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		// 새 아이템 장착 이벤트 브로드캐스트 (UI에서 장착 효과 표시 등)
		InventoryComponent->OnItemEquipped.Broadcast(ItemToEquip);
		// 기존 아이템 해제 이벤트 브로드캐스트 (UI에서 해제 효과 표시 등)
		InventoryComponent->OnItemUnequipped.Broadcast(ItemToUnequip);
	}
}

void UInv_SpatialInventory::SetEquippedItemDescriptionSizeAndPosition(UInv_ItemDescription* Description, UInv_ItemDescription* EquippedDescription, UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	UCanvasPanelSlot* EquippedItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(EquippedDescription);
	if (!IsValid(ItemDescriptionCPS) || !IsValid(EquippedItemDescriptionCPS)) return;

	const FVector2D ItemDescriptionSize = Description->GetBoxSize();
	const FVector2D EquippedItemDescriptionSize = EquippedDescription->GetBoxSize();

	FVector2D ClampedPosition = UInv_WidgetUtils::GetClampedWidgetPosition(
		UInv_WidgetUtils::GetWidgetSize(Canvas),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer()));
	ClampedPosition.X -= EquippedItemDescriptionSize.X;

	EquippedItemDescriptionCPS->SetSize(EquippedItemDescriptionSize);
	EquippedItemDescriptionCPS->SetPosition(ClampedPosition);
}

void UInv_SpatialInventory::ShowEquippedItemDescription(UInv_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	const FInv_EquipmentFragment* EquipmentFragment = Manifest.GetFragmentOfType<FInv_EquipmentFragment>();
	if (!EquipmentFragment) return;

	const FGameplayTag HoveredEquipmentType = EquipmentFragment->GetEquipmentType();
	
	auto EquippedGridSlot = EquippedGridSlots.FindByPredicate([Item](const UInv_EquippedGridSlot* GridSlot)
	{
		return GridSlot->GetInventoryItem() == Item;
	});
	if (EquippedGridSlot != nullptr) return; // 호버 아이템은 이미 장착되어 있고, 아이템 설명도 이미 보여줘요

	// 장착된 아이템이 아니니 같은 장비 유형으로 장착된 아이템을 찾아보세요
	auto FoundEquippedSlot = EquippedGridSlots.FindByPredicate([HoveredEquipmentType](const UInv_EquippedGridSlot* GridSlot)
	{
		return GridSlot->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FInv_EquipmentFragment>()->GetEquipmentType() == HoveredEquipmentType;
	});
	UInv_EquippedGridSlot* EquippedSlot = FoundEquippedSlot ? *FoundEquippedSlot : nullptr;
	if (!IsValid(EquippedSlot)) return; // No equipped item with the same equipment type

	UInv_InventoryItem* EquippedItem = EquippedSlot->GetInventoryItem().Get();
	if (!IsValid(EquippedItem)) return;

	const auto& EquippedItemManifest = EquippedItem->GetItemManifest();
	UInv_ItemDescription* DescriptionWidget = GetEquippedItemDescription();

	auto EquippedDescriptionWidget = GetEquippedItemDescription();
	
	EquippedDescriptionWidget->Collapse();
	DescriptionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);	
	EquippedItemManifest.AssimilateInventoryFragments(EquippedDescriptionWidget);
}

UInv_ItemDescription* UInv_SpatialInventory::GetEquippedItemDescription()
{
	if (!IsValid(EquippedItemDescription))
	{
		EquippedItemDescription = CreateWidget<UInv_ItemDescription>(GetOwningPlayer(), EquippedItemDescriptionClass);
		CanvasPanel->AddChild(EquippedItemDescription);
	}
	return EquippedItemDescription;
}

/**
 * 아이템 설명 위젯을 가져옵니다 (지연 초기화 패턴)
 *
 * 처음 호출될 때만 위젯을 생성하고, 이후에는 캐시된 인스턴스를 반환합니다.
 * 이는 메모리 효율성을 위한 최적화 기법입니다.
 *
 * 동작:
 * 1. ItemDescription이 nullptr인지 확인
 * 2. nullptr이면 새 위젯 생성
 * 3. 생성된 위젯을 캔버스 패널에 추가
 * 4. 위젯 반환
 *
 * 지연 초기화를 사용하는 이유:
 * - 위젯이 실제로 필요할 때만 생성하여 초기 로딩 시간 단축
 * - 사용하지 않는 경우 메모리 절약
 * - Unreal의 위젯 생성이 비용이 높은 작업이므로 필요할 때만 수행
 *
 * @return 아이템 설명 위젯 인스턴스
 */
UInv_ItemDescription* UInv_SpatialInventory::GetItemDescription()
{
	// 아직 생성되지 않은 경우에만 위젯을 생성합니다 (지연 초기화 패턴)
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UInv_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}
