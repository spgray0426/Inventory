// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 그리드 슬롯들을 생성하고 초기화합니다
	ConstructGrid();

	// 인벤토리 컴포넌트를 찾아 이벤트를 바인딩합니다
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
	InventoryComponent->OnStackChange.AddDynamic(this, &ThisClass::AddStacks);
}

void UInv_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 캔버스와 마우스 위치를 가져옵니다
	const FVector2D CanvasPosition = UInv_WidgetUtils::GetWidgetPosition(GridCanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	// 커서가 캔버스를 벗어났다면 조기 반환합니다
	if (CursorExitedCanvas(CanvasPosition, UInv_WidgetUtils::GetWidgetSize(GridCanvasPanel), MousePosition))
	{
		return;
	}

	// 타일 파라미터를 업데이트합니다 (마우스 위치 추적)
	UpdateTileParameters(CanvasPosition, MousePosition);
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemComponent)
{
	// 아이템 컴포넌트의 매니페스트를 사용하여 공간 확인
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_InventoryItem* Item)
{
	// 인벤토리 아이템의 매니페스트를 사용하여 공간 확인
	return HasRoomForItem(Item->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const FInv_ItemManifest& Manifest)
{
	FInv_SlotAvailabilityResult Result;

	//항목이 쌓을 수 있는지 확인합니다.
	const FInv_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FInv_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;

	//추가할 스택 수를 결정합니다.
	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;
	
	TSet<int32> CheckedIndices;
	
	for (const auto& GridSlot : GridSlots)
	{
		// 모든 수량이 채워졌다면 종료
		if (AmountToFill == 0 ) break;

		// 이미 검사한 슬롯이면 건너뜀
		if (IsIndexClaimed(CheckedIndices, GridSlot->GetIndex())) continue;

		// 그리드 범위를 벗어나면 건너뜀
		if (!IsInGridBounds(GridSlot->GetIndex(), GetItemDimensions(Manifest))) continue;
		
		// 해당 위치에 아이템을 놓을 수 있는지 확인
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, GetItemDimensions(Manifest), CheckedIndices, TentativelyClaimed, Manifest.GetItemType(), MaxStackSize))
		{
			continue;
		}

		// 슬롯에 채울 수 있는 수량을 계산
		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable, MaxStackSize, AmountToFill, GridSlot);
		if (AmountToFillInSlot == 0) continue;

		// 임시로 점유한 슬롯들을 실제로 점유
		CheckedIndices.Append(TentativelyClaimed);

		// 결과 정보 갱신
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			FInv_SlotAvailability{
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftGridIndex() : GridSlot->GetIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)
			}
		);

		// 남은 수량 갱신
		AmountToFill -= AmountToFillInSlot;
		Result.Remainder = AmountToFill;

		if (AmountToFill == 0) return Result;
	}
	return Result;
}

bool UInv_InventoryGrid::HasRoomAtIndex(const UInv_GridSlot* GridSlot,const FIntPoint& Dimensions,const TSet<int32>& CheckedIndices,TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize)
{
	bool bHasRoomAtIndex = true;

	UInv_InventoryStatics::ForEach2D(GridSlots, GridSlot->GetIndex(), Dimensions, Columns, [&](const UInv_GridSlot* SubGridSlot)
	{
		if (CheckSlotConstraints(GridSlot, SubGridSlot, CheckedIndices, OutTentativelyClaimed, ItemType, MaxStackSize))
		{
			OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		}
		else
		{
			bHasRoomAtIndex = false;
		}
	});

	return bHasRoomAtIndex;
}

bool UInv_InventoryGrid::CheckSlotConstraints(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot,const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize) const
{
	// 이미 점유된 인덱스면 실패
	if (IsIndexClaimed(CheckedIndices, SubGridSlot->GetIndex())) return false;

	// 아이템이 없는 슬롯이면 사용 가능
	if (!HasValidItem(SubGridSlot))
	{
		OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		return true;
	}

	// 좌상단 슬롯이 아니면 실패
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;

	// 스택 가능 여부 확인
	const UInv_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;

	// 아이템 타입이 일치하는지 확인
	if (!DoesItemTypeMatch(SubItem, ItemType)) return false;

	// 스택이 꽉 찼는지 확인
	if (GridSlot->GetStackCount() >= MaxStackSize) return false;

	return true ;
}

bool UInv_InventoryGrid::HasValidItem(const UInv_GridSlot* GridSlot) const
{
	// 슬롯에 유효한 아이템이 있는지 확인
	return GridSlot->GetInventoryItem().IsValid();
}

bool UInv_InventoryGrid::IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const
{
	// SubGridSlot의 좌상단 인덱스가 GridSlot의 인덱스와 같은지 확인
	return SubGridSlot->GetUpperLeftGridIndex() == GridSlot->GetIndex();
}

bool UInv_InventoryGrid::DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	// 아이템 타입 GameplayTag가 정확히 일치하는지 확인
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UInv_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const
{
	// 시작 인덱스가 유효한 범위 내에 있는지 확인
	if (StartIndex < 0 || StartIndex >= GridSlots.Num()) return false;

	// 아이템의 끝 위치가 그리드 경계를 넘지 않는지 확인
	const int32 EndColumn = (StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = (StartIndex / Columns) + ItemDimensions.Y;
	return EndColumn <= Columns && EndRow <= Rows;
}

int32 UInv_InventoryGrid::DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UInv_GridSlot* GridSlot) const
{
	// 슬롯에 채울 수 있는 여유 공간 계산
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);
	// 스택 가능하면 최소값 사용, 아니면 1개만
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;
}

int32 UInv_InventoryGrid::GetStackAmount(const UInv_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();
	// 다중 슬롯 아이템의 경우, 좌상단 슬롯에서 실제 스택 수를 가져옵니다
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftGridIndex(); UpperLeftIndex != INDEX_NONE)
	{
		UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();
	}
	return CurrentSlotStackCount;
}

FIntPoint UInv_InventoryGrid::GetItemDimensions(const FInv_ItemManifest& Manifest) const
{
	// 그리드 프래그먼트에서 아이템 크기를 가져옵니다 (없으면 기본 1x1)
	const FInv_GridFragment* GridFragment = Manifest.GetFragmentOfType<FInv_GridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
}

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* Item)
{
	// 카테고리가 일치하지 않으면 무시
	if (!MatchesCategory(Item)) return;

	// 공간이 있는지 확인하고 결과에 따라 아이템 추가
	FInv_SlotAvailabilityResult Result = HasRoomForItem(Item);
	AddItemToIndices(Result, Item);
}

void UInv_InventoryGrid::ShowCursor()
{
	// 플레이어 컨트롤러가 유효하지 않으면 무시
	if (!IsValid(GetOwningPlayer())) return;
	// 표시 가능한 커서 위젯을 마우스 커서로 설정합니다
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
}

void UInv_InventoryGrid::HideCursor()
{
	// 플레이어 컨트롤러가 유효하지 않으면 무시
	if (!IsValid(GetOwningPlayer())) return;
	// 숨겨진 커서 위젯을 마우스 커서로 설정하여 커서를 보이지 않게 합니다
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem)
{
	// 사용 가능한 모든 슬롯에 아이템을 추가하고 업데이트합니다
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
	}
}

UInv_SlottedItem* UInv_InventoryGrid::CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment, const int32 Index)
{
	// 슬롯 아이템 위젯을 생성하고 초기화합니다
	UInv_SlottedItem* SlottedItem = CreateWidget<UInv_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	const int32 StackUpdateAmount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);

	// 클릭 이벤트를 바인딩합니다
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &UInv_InventoryGrid::OnSlottedItemClicked);
	return SlottedItem;
}

void UInv_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment, UInv_SlottedItem* SlottedItem)
{
	// 슬롯 아이템을 캔버스에 추가하고 위치와 크기를 설정합니다
	GridCanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));

	// 그리드 인덱스로부터 위치를 계산하고 패딩을 적용합니다
	const FVector2D DrawPos = UInv_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawposWithPadding = DrawPos + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawposWithPadding);
}

void UInv_InventoryGrid::UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	// 스택 가능 아이템이면 스택 수량 설정
	if (bStackableItem)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}

	// 아이템이 차지하는 모든 슬롯을 업데이트합니다
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewItem, FragmentTags::GridFragment);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);

	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(NewItem);
		GridSlot->SetUpperLeftGridIndex(Index);
		GridSlot->SetOccupiedTexture();
		GridSlot->SetAvailable(false);
	});
}

bool UInv_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const
{
	// 인덱스가 이미 점유되었는지 확인
	return CheckedIndices.Contains(Index);
}

void UInv_InventoryGrid::AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount)
{
	// 아이템의 프래그먼트를 가져옵니다
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
    const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item, FragmentTags::IconFragment);
    if (!GridFragment || !ImageFragment) return;

	// 슬롯 아이템 위젯을 생성하고 캔버스에 추가합니다
    UInv_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);

	SlottedItems.Add(Index, SlottedItem);
}

void UInv_InventoryGrid::AddStacks(const FInv_SlotAvailabilityResult& Result)
{
	// 카테고리가 일치하지 않으면 무시
	if (!MatchesCategory(Result.Item.Get())) return;

	// 각 사용 가능한 슬롯에 스택을 추가합니다
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			// 이미 아이템이 있는 슬롯이면 스택 수량만 업데이트
			const auto& GridSlot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
		}
		else
		{
			// 비어있는 슬롯이면 새로 아이템 추가
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
}

void UInv_InventoryGrid::OnSlottedItemClicked(int32 GrideIndex, const FPointerEvent& MouseEvent)
{
	check(GridSlots.IsValidIndex(GrideIndex));
	UInv_InventoryItem* ClickedInventoryItem = GridSlots[GrideIndex]->GetInventoryItem().Get();

	// 호버 아이템이 없고 왼쪽 클릭이면 아이템을 집어듭니다
	if (!IsValid(HoverItem) && IsLeftClick(MouseEvent))
	{
		PickUp(ClickedInventoryItem, GrideIndex);
	}
	// 호버 아이템이 있고 오른쪽 클릭이면 (향후 구현 예정)
	else if (IsValid(HoverItem) && IsRightClick(MouseEvent))
	{

	}
}

void UInv_InventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	// 호버 아이템이 없거나 인덱스가 유효하지 않으면 무시
	if (!IsValid(HoverItem)) return;
	if (!GridSlots.IsValidIndex(ItemDropIndex)) return;

	// 클릭한 위치에 아이템이 있으면 해당 아이템을 클릭한 것으로 처리
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}

	// 빈 슬롯이면 아이템을 놓습니다
	auto GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		PutDownOnIndex(ItemDropIndex);
	}
}

void UInv_InventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	// 호버 아이템이 있으면 무시 (드래그 중이면 별도 처리)
	if (IsValid(HoverItem)) return;

	// 빈 슬롯에 마우스가 올라가면 하이라이트 처리
	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
}

void UInv_InventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	// 호버 아이템이 있으면 무시 (드래그 중이면 별도 처리)
	if (IsValid(HoverItem)) return;

	// 슬롯에서 마우스가 벗어나면 하이라이트 해제
	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
}

bool UInv_InventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	// 마우스 이벤트가 오른쪽 클릭인지 확인
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UInv_InventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	// 마우스 이벤트가 왼쪽 클릭인지 확인
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

void UInv_InventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	// 호버 아이템에 클릭한 아이템을 할당하고 그리드에서 제거합니다
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex)
{
	// 호버 아이템을 할당합니다
	AssignHoverItem(InventoryItem);

	// 이전 위치와 스택 수량을 설정합니다
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UInv_InventoryGrid::RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex)
{
	// 아이템의 그리드 프래그먼트를 가져옵니다
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	if (!GridFragment) return;

	// 아이템이 차지하는 모든 슬롯을 초기화합니다
	UInv_InventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetUpperLeftGridIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetAvailable(true);
		GridSlot->SetStackCount(0);
	});

	// 슬롯 아이템 위젯을 제거합니다
	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UInv_SlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UInv_InventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	// 마우스가 캔버스 밖에 있으면 무시
	if (!bMouseWithinCanvas) return;

	// 호버된 타일의 좌표와 사분면을 계산합니다
	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);
	LastTileParameters = TileParameters;
	TileParameters.TileCoordinats = HoveredTileCoordinates;
	TileParameters.TileIndex = UInv_WidgetUtils::GetIndexFromPosition(HoveredTileCoordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);

	// 타일 파라미터 업데이트 처리
	OnTileParametersUpdated(TileParameters);
}

FIntPoint UInv_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	// 마우스 위치로부터 타일 좌표를 계산합니다
	return FIntPoint{
		static_cast<int32>(FMath::FloorToInt((MousePosition.X - CanvasPosition.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt((MousePosition.Y - CanvasPosition.Y) / TileSize))
	};
}

EInv_TileQuadrant UInv_InventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	// 타일 내에서의 로컬 위치를 계산합니다
	const float TileLocalX = FMath::Fmod(MousePosition.X - CanvasPosition.X, TileSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - CanvasPosition.Y, TileSize);

	// 상단/하단, 좌측/우측을 판단합니다
	const bool bIsTop = TileLocalY < TileSize / 2;
	const bool bIsLeft = TileLocalX < TileSize / 2;

	EInv_TileQuadrant HoveredTileQuadrant {EInv_TileQuadrant::None};

	// 4개의 사분면 중 하나를 결정합니다
	if (bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::TopLeft;
	else if (bIsTop && !bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::TopRight;
	else if (!bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomLeft;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomRight;

	return HoveredTileQuadrant;
}

void UInv_InventoryGrid::OnTileParametersUpdated(const FInv_TileParameters& Parameters)
{
	if (!IsValid(HoverItem)) return;
	
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();

	const FIntPoint StartingCoordinate = CalculateStartingCoordinate(Parameters.TileCoordinats, Dimensions, Parameters.TileQuadrant);
	ItemDropIndex = UInv_WidgetUtils::GetIndexFromPosition(StartingCoordinate, Columns);
	
	CurrentQueryResult = CheckHoverPosition(StartingCoordinate, Dimensions);

	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(CurrentQueryResult.ValidItem.Get(), FragmentTags::GridFragment);
		if (!GridFragment) return;

		ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), EInv_GridSlotState::GrayedOut);
	}
}

FIntPoint UInv_InventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const EInv_TileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;

	FIntPoint StartingCoord;
	switch (Quadrant)
	{
	case EInv_TileQuadrant::TopLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EInv_TileQuadrant::TopRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EInv_TileQuadrant::BottomLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	case EInv_TileQuadrant::BottomRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	default:
		UE_LOG(LogInventory, Error, TEXT("Invalid Quadrant."))
		return FIntPoint(-1, -1);
	}
	return StartingCoord;
}

FInv_SpaceQueryResult UInv_InventoryGrid::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions)
{
	FInv_SpaceQueryResult Result;
	
	// in the grid bounds?
	if (!IsInGridBounds(UInv_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions)) return Result;

	Result.bHasSpace = true;
	
	// 둘 이상의 인덱스가 동일한 항목으로 점유되어 있는 경우 모두 동일한 왼쪽 상단 인덱스를 가지고 있는지 확인해야 합니다.
	TSet<int32> OccupiedUpperLeftIndices;
	UInv_InventoryStatics::ForEach2D(GridSlots, UInv_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions, Columns, [&](const UInv_GridSlot* GridSlot)
	{
		if (GridSlot->GetInventoryItem().IsValid())
		{
			OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftGridIndex());
			Result.bHasSpace = false;
		}
	});
	
	// 그렇다면 방해가 되는 항목은 하나뿐입니까? (교환할 수 있습니까?)
	if (OccupiedUpperLeftIndices.Num() == 1) // 위치의 단일 항목 - 교환/결합에 유효합니다.
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftGridIndex();
	}
	
	return Result;
}

bool UInv_InventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location)
{
	// 마우스가 캔버스 내에 있는지 확인하고 상태를 업데이트합니다
	bLastMouseWithinCanvas = bMouseWithinCanvas;
	bMouseWithinCanvas = UInv_WidgetUtils::IsWithinBounds(BoundaryPos, BoundarySize, Location);

	// 캔버스를 벗어났으면 하이라이트를 해제합니다
	if (!bMouseWithinCanvas && bLastMouseWithinCanvas)
	{
		UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}
	return false;
}

void UInv_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	// 마우스가 캔버스 밖에 있으면 무시
	if (!bMouseWithinCanvas) return;

	// 이전 하이라이트를 해제하고 새로운 슬롯들을 하이라이트합니다
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetOccupiedTexture();
	});
	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;
}

void UInv_InventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	// 슬롯들의 하이라이트를 해제하고 원래 상태로 되돌립니다
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		if (GridSlot->IsAvailable())
		{
			GridSlot->SetUnoccupiedTexture();
		}
		else
		{
			GridSlot->SetOccupiedTexture();
		}
	});
}

void UInv_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EInv_GridSlotState GridSlotState)
{
	// 이전 하이라이트를 해제하고 새로운 상태로 변경합니다
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [State = GridSlotState](UInv_GridSlot* GridSlot)
	{
		switch (State)
		{
		case EInv_GridSlotState::Occupied:
			GridSlot->SetOccupiedTexture();
			break;
		case EInv_GridSlotState::Unoccupied:
			GridSlot->SetUnoccupiedTexture();
			break;
		case EInv_GridSlotState::GrayedOut:
			GridSlot->SetGrayedOutTexture();
			break;
		case EInv_GridSlotState::Selected:
			GridSlot->SetSelectedTexture();
			break;
		}
	});

	LastHighlightedIndex = Index;
	LastHighlightedDimensions = Dimensions;
}

void UInv_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	// 호버 아이템을 지정된 인덱스에 배치하고 그리드를 업데이트합니다
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	ClearHoverItem();
}

void UInv_InventoryGrid::ClearHoverItem()
{
	// 호버 아이템이 없으면 무시
	if (!IsValid(HoverItem)) return;

	// 호버 아이템의 모든 속성을 초기화합니다
	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	HoverItem->SetImageBrush(FSlateNoResource());

	// 호버 아이템을 제거합니다
	HoverItem->RemoveFromParent();
	HoverItem = nullptr;

	// 커서를 다시 표시합니다
	ShowCursor();
}

UUserWidget* UInv_InventoryGrid::GetVisibleCursorWidget()
{
	// 플레이어 컨트롤러가 유효하지 않으면 nullptr 반환
	if (!IsValid(GetOwningPlayer())) return nullptr;
	// 위젯이 생성되지 않았다면 생성합니다
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}
	return VisibleCursorWidget;
}

UUserWidget* UInv_InventoryGrid::GetHiddenCursorWidget()
{
	// 플레이어 컨트롤러가 유효하지 않으면 nullptr 반환
	if (!IsValid(GetOwningPlayer())) return nullptr;
	// 위젯이 생성되지 않았다면 생성합니다
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HiddenCursorWidgetClass);
	}
	return HiddenCursorWidget;
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem)
{
	// 호버 아이템이 없으면 생성합니다
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInv_HoverItem>(GetOwningPlayer(), HoverItemClass);
	}

	// 아이템의 프래그먼트를 가져옵니다
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(InventoryItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;

	// 아이콘의 그릴 크기를 계산합니다
	const FVector2D DrawSize = GetDrawSize(GridFragment);

	// 아이콘 브러시를 설정합니다
	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);

	// 호버 아이템의 속성을 설정합니다
	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	// 마우스 커서 위젯으로 설정합니다
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}



FVector2D UInv_InventoryGrid::GetDrawSize(const FInv_GridFragment* GridFragment) const
{
	// 패딩을 제외한 타일 크기를 계산하고 아이템 크기를 곱합니다
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	return GridFragment->GetGridSize() * IconTileWidth;
}

void UInv_InventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment) const
{
	// 슬롯 아이템의 이미지 브러시를 설정합니다
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);

	SlottedItem->SetImageBrush(Brush);
}




void UInv_InventoryGrid::ConstructGrid()
{
	// 그리드 슬롯 배열의 공간을 미리 할당합니다
	GridSlots.Reserve(Rows * Columns);

	// 모든 행과 열에 대해 그리드 슬롯을 생성합니다
	for (int32 j = 0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			// 그리드 슬롯 위젯을 생성하고 캔버스에 추가합니다
			UInv_GridSlot* GridSlot = CreateWidget<UInv_GridSlot>(this, GridSlotClass);
			GridCanvasPanel->AddChild(GridSlot);
			const FIntPoint TilePosition(i, j);
			GridSlot->SetTileIndex(UInv_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));

			// 슬롯의 크기와 위치를 설정합니다
			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition * TileSize);

			// 이벤트를 바인딩하고 배열에 추가합니다
			GridSlots.Add(GridSlot);
			GridSlot->GridSlotClicked.AddDynamic(this, &ThisClass::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this, &ThisClass::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this, &ThisClass::OnGridSlotUnhovered);
		}
	}
}

bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* Item) const
{
	// 아이템의 카테고리가 이 그리드의 카테고리와 일치하는지 확인
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}
