// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_SpatialInventory.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
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

	Grid_Equippables->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);
	
	// 기본적으로 장비 그리드를 표시합니다
	ShowEquippables();
}

FReply UInv_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}

void UInv_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 아이템 설명 위젯이 생성되어 있으면 매 프레임 위치를 업데이트합니다
	// 마우스 커서를 따라다니도록 하기 위함입니다
	if (!IsValid(ItemDescription)) return;
	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);
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

void UInv_SpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	// 아이템 설명 위젯을 가져옵니다 (없으면 생성)
	UInv_ItemDescription* DescriptionWidget = GetItemDescription();
	// 일단 숨김 상태로 설정합니다
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);

	// 이전 타이머가 있다면 정리합니다
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);

	// 지연 후에 설명을 표시하는 람다를 타이머 델리게이트에 바인딩합니다
	// 이를 통해 마우스를 빠르게 움직일 때는 설명이 표시되지 않습니다
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this]()
	{
		// HitTestInvisible로 설정하여 마우스 이벤트를 방해하지 않으면서 보이도록 합니다
		GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
	});

	// 설정된 지연 시간 후에 타이머를 실행합니다 (한 번만)
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimer, DescriptionTimerDelegate, DescriptionTimerDelay, false);
}

void UInv_SpatialInventory::OnItemUnHovered()
{
	// 아이템 설명을 즉시 숨깁니다
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	// 실행 대기 중인 타이머를 취소합니다
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
}

bool UInv_SpatialInventory::HasHoverItem() const
{
	// 세 개의 그리드 중 하나라도 호버 아이템(드래그 중인 아이템)이 있으면 true를 반환합니다
	if (Grid_Equippables->HasHoverItem()) return true;
	if (Grid_Consumables->HasHoverItem()) return true;
	if (Grid_Craftables->HasHoverItem()) return true;
	return false;
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
