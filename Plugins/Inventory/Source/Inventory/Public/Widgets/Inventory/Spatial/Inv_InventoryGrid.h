// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Inv_InventoryItem.h"
#include "Types/Inv_GridTypes.h"
#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"
#include "Inv_InventoryGrid.generated.h"

class UInv_ItemPopUp;
enum class EInv_GridSlotState : uint8;
class UInv_HoverItem;
struct FInv_ImageFragment;
struct FInv_GridFragment;
class UInv_SlottedItem;
class UInv_ItemComponent;
class UInv_InventoryComponent;
class UCanvasPanel;
class UInv_GridSlot;

/**
 * 그리드 기반 인벤토리 위젯 클래스
 * 그리드 슬롯과 슬롯에 배치된 아이템들을 관리합니다
 * 카테고리별로 필터링되며, 아이템 배치 가능 여부 검사, 드래그 앤 드롭 등의 기능을 제공합니다
 * 공간 기반 인벤토리 시스템의 핵심 클래스입니다
 */
UCLASS()
class INVENTORY_API UInv_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()
public:

	/**
	 * 위젯 초기화 시 호출됩니다
	 * 그리드를 구성하고 필요한 초기 설정을 수행합니다
	 */
	virtual void NativeOnInitialized() override;

	/**
	 * 매 프레임 호출됩니다
	 * 마우스 위치를 추적하고 호버 아이템을 업데이트합니다
	 * @param MyGeometry 위젯의 지오메트리 정보
	 * @param InDeltaTime 이전 프레임으로부터의 경과 시간
	 */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/**
	 * 이 그리드가 표시하는 아이템 카테고리를 가져옵니다
	 * @return 아이템 카테고리 (장비, 소비품, 제작 재료)
	 */
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }

	/**
	 * 아이템 컴포넌트를 위한 공간이 있는지 확인합니다 (공개 인터페이스)
	 * @param ItemComponent 확인할 아이템 컴포넌트
	 * @return 슬롯 사용 가능 여부 결과
	 */
	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_ItemComponent* ItemComponent);
	

	/**
	 * 인벤토리 아이템을 그리드에 추가합니다
	 * @param Item 추가할 인벤토리 아이템
	 */
	UFUNCTION()
	void AddItem(UInv_InventoryItem* Item);

	/**
	 * 마우스 커서를 보이게 설정합니다
	 * VisibleCursorWidget을 마우스 커서로 설정합니다
	 */
	void ShowCursor();

	/**
	 * 마우스 커서를 숨깁니다
	 * HiddenCursorWidget을 마우스 커서로 설정하여 커서를 보이지 않게 합니다
	 */
	void HideCursor();

	/**
	 * 이 그리드를 소유하는 캔버스 패널을 설정합니다
	 * 아이템 팝업 메뉴를 표시할 때 필요합니다
	 * @param OwningCanvas 소유 캔버스 패널
	 */
	void SetOwningCanvas(UCanvasPanel* OwningCanvas);

	/**
	 * 현재 호버 아이템을 드롭합니다
	 * 서버에 드롭 요청을 보내고 호버 아이템을 제거합니다
	 */
	void DropItem();

	/**
	 * 현재 드래그 중인 호버 아이템이 있는지 확인합니다
	 * 드래그 중일 때는 아이템 설명을 표시하지 않기 위해 사용됩니다
	 * @return 호버 아이템이 있으면 true
	 */
	bool HasHoverItem() const;

	/**
	 * 이 그리드의 호버 아이템 위젯을 가져옵니다
	 * @return 호버 아이템 위젯
	 */
	UInv_HoverItem* GetHoverItem() const;

	/**
	 * 그리드의 타일 크기를 가져옵니다
	 * @return 타일 크기 (픽셀 단위)
	 */
	float GetTileSize() const { return TileSize; }

	/**
	 * 호버 아이템을 제거합니다
	 * 아이템이 장착되거나 다른 위치로 이동된 후 호버 아이템을 비웁니다
	 * 호버 아이템 위젯은 제거되지 않고, 내부 아이템 데이터만 초기화됩니다
	 */
	void ClearHoverItem();
	
	/**
	 * 호버 아이템을 할당합니다
	 * @param InventoryItem 인벤토리 아이템
	 */
	void AssignHoverItem(UInv_InventoryItem* InventoryItem);
	
	void OnHide();
	
private:

	/**
	 * 그리드를 구성합니다
	 * 지정된 행과 열 수에 따라 그리드 슬롯들을 생성하고 캔버스에 배치합니다
	 */
	void ConstructGrid();
	
	/**
	 * 인벤토리 아이템을 위한 공간이 있는지 확인합니다
	 * @param Item 확인할 인벤토리 아이템
	 * @return 슬롯 사용 가능 여부 결과
	 */
	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_InventoryItem* Item, const int32 StackAmountOverride = -1);

	/**
	 * 아이템 매니페스트를 위한 공간이 있는지 확인합니다
	 * @param Manifest 확인할 아이템 매니페스트
	 * @return 슬롯 사용 가능 여부 결과
	 */
	FInv_SlotAvailabilityResult HasRoomForItem(const FInv_ItemManifest& Manifest, const int32 StackAmountOverride = -1);

	/**
	 * 결과에 따라 지정된 인덱스에 아이템을 추가합니다
	 * @param Result 슬롯 사용 가능 여부 결과
	 * @param NewItem 추가할 새 아이템
	 */
	void AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem);
	/**
	 * 슬롯에 배치될 아이템 위젯을 생성합니다
	 * @param Item 인벤토리 아이템
	 * @param bStackable 스택 가능 여부
	 * @param StackAmount 스택 수량
	 * @param GridFragment 그리드 프래그먼트
	 * @param ImageFragment 이미지 프래그먼트
	 * @param Index 그리드 인덱스
	 * @return 생성된 슬롯 아이템 위젯
	 */
	UInv_SlottedItem* CreateSlottedItem(UInv_InventoryItem* Item,
		const bool bStackable,
		const int32 StackAmount,
		const FInv_GridFragment* GridFragment,
		const FInv_ImageFragment* ImageFragment,
		const int32 Index);

	/**
	 * 슬롯 아이템을 캔버스에 추가합니다
	 * @param Index 그리드 인덱스
	 * @param GridFragment 그리드 프래그먼트 (크기 정보)
	 * @param SlottedItem 추가할 슬롯 아이템 위젯
	 */
	void AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment, UInv_SlottedItem* SlottedItem);

	/**
	 * 그리드 슬롯들을 업데이트합니다 (점유 상태 등)
	 * @param NewItem 새로 추가된 아이템
	 * @param Index 시작 인덱스
	 * @param bStackableItem 스택 가능 여부
	 * @param StackAmount 스택 수량
	 */
	void UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount);

	/**
	 * 인덱스가 이미 점유되었는지 확인합니다
	 * @param CheckedIndices 확인된 인덱스 집합
	 * @param Index 확인할 인덱스
	 * @return 점유된 경우 true
	 */
	bool IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const;

	/**
	 * 지정된 인덱스에 아이템을 배치할 공간이 있는지 확인합니다
	 * @param GridSlot 확인할 그리드 슬롯
	 * @param Dimensions 아이템의 크기 (너비, 높이)
	 * @param CheckedIndices 이미 확인된 인덱스 집합
	 * @param OutTentativelyClaimed 임시로 점유된 인덱스들을 반환할 출력 매개변수
	 * @param ItemType 아이템 타입 GameplayTag
	 * @param MaxStackSize 최대 스택 크기
	 * @return 공간이 있는 경우 true
	 */
	bool HasRoomAtIndex(const UInv_GridSlot* GridSlot,
		const FIntPoint& Dimensions,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize);

	/**
	 * 슬롯 제약 조건을 확인합니다 (스택 가능 여부, 타입 일치 등)
	 * @param GridSlot 주 그리드 슬롯
	 * @param SubGridSlot 확인할 하위 그리드 슬롯
	 * @param CheckedIndices 이미 확인된 인덱스 집합
	 * @param OutTentativelyClaimed 임시로 점유된 인덱스들
	 * @param ItemType 아이템 타입 GameplayTag
	 * @param MaxStackSize 최대 스택 크기
	 * @return 제약 조건을 만족하는 경우 true
	 */
	bool CheckSlotConstraints(const UInv_GridSlot* GridSlot,
		const UInv_GridSlot* SubGridSlot,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize) const;

	/**
	 * 그리드 슬롯에 유효한 아이템이 있는지 확인합니다
	 * @param GridSlot 확인할 그리드 슬롯
	 * @return 유효한 아이템이 있는 경우 true
	 */
	bool HasValidItem(const UInv_GridSlot* GridSlot) const;

	/**
	 * SubGridSlot이 GridSlot의 좌상단 슬롯인지 확인합니다
	 * @param GridSlot 주 그리드 슬롯
	 * @param SubGridSlot 확인할 하위 슬롯
	 * @return 좌상단 슬롯인 경우 true
	 */
	bool IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const;

	/**
	 * 아이템 타입이 일치하는지 확인합니다
	 * @param SubItem 확인할 인벤토리 아이템
	 * @param ItemType 비교할 아이템 타입 GameplayTag
	 * @return 타입이 일치하는 경우 true
	 */
	bool DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const;

	/**
	 * 아이템이 그리드 경계 내에 있는지 확인합니다
	 * @param StartIndex 시작 인덱스
	 * @param ItemDimensions 아이템 크기
	 * @return 경계 내에 있는 경우 true
	 */
	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const;

	/**
	 * 슬롯에 채울 수 있는 수량을 결정합니다
	 * @param bStackable 스택 가능 여부
	 * @param MaxStackSize 최대 스택 크기
	 * @param AmountToFill 채워야 할 총 수량
	 * @param GridSlot 대상 그리드 슬롯
	 * @return 슬롯에 채울 수량
	 */
	int32 DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UInv_GridSlot* GridSlot) const;

	/**
	 * 그리드 슬롯의 현재 스택 수량을 가져옵니다
	 * @param GridSlot 확인할 그리드 슬롯
	 * @return 스택 수량
	 */
	int32 GetStackAmount(const UInv_GridSlot* GridSlot) const;

	/**
	 * 아이템 매니페스트에서 아이템의 크기를 가져옵니다
	 * @param Manifest 아이템 매니페스트
	 * @return 아이템 크기 (너비, 높이)
	 */
	FIntPoint GetItemDimensions(const FInv_ItemManifest& Manifest) const;

	/**
	 * 아이템이 이 그리드의 카테고리와 일치하는지 확인합니다
	 * @param Item 확인할 인벤토리 아이템
	 * @return 카테고리가 일치하는 경우 true
	 */
	bool MatchesCategory(const UInv_InventoryItem* Item) const;

	/**
	 * 그리드 프래그먼트로부터 그릴 크기를 계산합니다
	 * @param GridFragment 그리드 프래그먼트
	 * @return 그릴 크기 (픽셀)
	 */
	FVector2D GetDrawSize(const FInv_GridFragment* GridFragment) const;

	/**
	 * 슬롯 아이템의 이미지를 설정합니다
	 * @param SlottedItem 슬롯 아이템 위젯
	 * @param GridFragment 그리드 프래그먼트
	 * @param ImageFragment 이미지 프래그먼트
	 */
	void SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment) const;

	/**
	 * 지정된 인덱스에 아이템을 추가합니다
	 * @param Item 추가할 인벤토리 아이템
	 * @param Index 그리드 인덱스
	 * @param bStackable 스택 가능 여부
	 * @param StackAmount 스택 수량
	 */
	void AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);
	
	/**
	 * 기존 스택에 스택을 추가합니다
	 * @param Result 슬롯 사용 가능 여부 결과
	 */
	UFUNCTION()
	void AddStacks(const FInv_SlotAvailabilityResult& Result);
	
	/**
	 * 마우스 이벤트가 오른쪽 클릭인지 확인합니다
	 * @param MouseEvent 마우스 이벤트
	 * @return 오른쪽 클릭인 경우 true
	 */
	bool IsRightClick(const FPointerEvent& MouseEvent) const;

	/**
	 * 마우스 이벤트가 왼쪽 클릭인지 확인합니다
	 * @param MouseEvent 마우스 이벤트
	 * @return 왼쪽 클릭인 경우 true
	 */
	bool IsLeftClick(const FPointerEvent& MouseEvent) const;

	/**
	 * 아이템을 집어듭니다 (드래그 시작)
	 * @param ClickedInventoryItem 클릭된 인벤토리 아이템
	 * @param GridIndex 그리드 인덱스
	 */
	void PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex);

	/**
	 * 호버 아이템을 할당합니다 (이전 위치 정보 포함)
	 * @param InventoryItem 인벤토리 아이템
	 * @param GridIndex 현재 그리드 인덱스
	 * @param PreviousGridIndex 이전 그리드 인덱스
	 */
	void AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex);
	
	/**
	 * 그리드에서 아이템을 제거합니다
	 * @param InventoryItem 제거할 인벤토리 아이템
	 * @param GridIndex 그리드 인덱스
	 */
	void RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex);

	/**
	 * 타일 파라미터를 업데이트합니다 (마우스 위치 기반)
	 * @param CanvasPosition 캔버스 위치
	 * @param MousePosition 마우스 위치
	 */
	void UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition);

	/**
	 * 마우스가 호버하고 있는 타일의 좌표를 계산합니다
	 * @param CanvasPosition 캔버스 위치
	 * @param MousePosition 마우스 위치
	 * @return 호버된 타일 좌표
	 */
	FIntPoint CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;

	/**
	 * 타일 내에서 마우스가 위치한 사분면을 계산합니다
	 * @param CanvasPosition 캔버스 위치
	 * @param MousePosition 마우스 위치
	 * @return 타일 사분면 (좌상단, 우상단, 좌하단, 우하단)
	 */
	EInv_TileQuadrant CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;

	/**
	 * 아이템의 시작 좌표를 계산합니다 (사분면 기반 배치)
	 * @param Coordinate 현재 좌표
	 * @param Dimensions 아이템 크기
	 * @param Quadrant 타일 사분면
	 * @return 계산된 시작 좌표
	 */
	FIntPoint CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const EInv_TileQuadrant Quadrant) const;

	/**
	 * 호버 위치에 공간이 있는지 확인합니다
	 * @param Position 확인할 위치
	 * @param Dimensions 아이템 크기
	 * @return 공간 쿼리 결과
	 */
	FInv_SpaceQueryResult CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions);

	/**
	 * 커서가 캔버스를 벗어났는지 확인합니다
	 * @param BoundaryPos 경계 위치
	 * @param BoundarySize 경계 크기
	 * @param Location 현재 위치
	 * @return 캔버스를 벗어난 경우 true
	 */
	bool CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location);

	/**
	 * 지정된 위치의 슬롯들을 하이라이트 처리합니다
	 * @param Index 시작 인덱스
	 * @param Dimensions 차원 (너비, 높이)
	 */
	void HighlightSlots(const int32 Index, const FIntPoint& Dimensions);

	/**
	 * 지정된 위치의 슬롯들의 하이라이트를 해제합니다
	 * @param Index 시작 인덱스
	 * @param Dimensions 차원 (너비, 높이)
	 */
	void UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions);

	/**
	 * 호버 타입을 변경합니다 (배치 가능/불가능 등의 시각적 피드백)
	 * @param Index 시작 인덱스
	 * @param Dimensions 차원 (너비, 높이)
	 * @param GridSlotState 적용할 그리드 슬롯 상태
	 */
	void ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EInv_GridSlotState GridSlotState);

	/**
	 * 지정된 인덱스에 아이템을 놓습니다
	 * @param Index 그리드 인덱스
	 */
	void PutDownOnIndex(const int32 Index);
	

	/**
	 * 표시 가능한 커서 위젯을 가져옵니다
	 * 위젯이 생성되지 않았다면 생성합니다
	 * @return 표시 가능한 커서 위젯
	 */
	UUserWidget* GetVisibleCursorWidget();

	/**
	 * 숨겨진 커서 위젯을 가져옵니다
	 * 위젯이 생성되지 않았다면 생성합니다
	 * @return 숨겨진 커서 위젯
	 */
	UUserWidget* GetHiddenCursorWidget();

	/**
	 * 클릭한 아이템이 호버 아이템과 같은 종류의 스택 가능 아이템인지 확인합니다
	 * @param ClickedInventoryItem 클릭한 인벤토리 아이템
	 * @return 같은 종류의 스택 가능 아이템인 경우 true
	 */
	bool IsSameStackable(const UInv_InventoryItem* ClickedInventoryItem) const;

	/**
	 * 호버 아이템과 클릭한 아이템의 위치를 교환합니다
	 * @param ClickedInventoryItem 클릭한 인벤토리 아이템
	 * @param GridIndex 클릭한 그리드 인덱스
	 */
	void SwapWithHoverItem(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex);

	/**
	 * 스택 수량을 교환해야 하는지 판단합니다
	 * @param RoomInClickedSlot 클릭한 슬롯의 남은 공간
	 * @param HoveredStackCount 호버 아이템의 스택 수량
	 * @param MaxStackSize 최대 스택 크기
	 * @return 교환해야 하는 경우 true (클릭한 슬롯이 가득 찼고 호버 아이템이 최대 스택 미만인 경우)
	 */
	bool ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const;

	/**
	 * 클릭한 슬롯과 호버 아이템의 스택 수량을 교환합니다
	 * @param ClickedStackCount 클릭한 슬롯의 스택 수량
	 * @param HoveredStackCount 호버 아이템의 스택 수량
	 * @param Index 그리드 인덱스
	 */
	void SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);

	/**
	 * 호버 아이템의 모든 스택을 클릭한 슬롯에 소비할 수 있는지 확인합니다
	 * @param HoveredStackCount 호버 아이템의 스택 수량
	 * @param RoomInClickedSlot 클릭한 슬롯의 남은 공간
	 * @return 클릭한 슬롯에 호버 아이템의 전체 스택을 담을 수 있는 경우 true (예: 호버 20개, 슬롯 공간 30개 남음)
	 */
	bool ShouldConsumeHoverItemStacks(const int32 HoveredStackCount, const int32 RoomInClickedSlot) const;

	/**
	 * 호버 아이템의 모든 스택을 클릭한 슬롯에 소비하고 호버 아이템을 제거합니다
	 * @param ClickedStackCount 클릭한 슬롯의 현재 스택 수량
	 * @param HoveredStackCount 호버 아이템의 스택 수량
	 * @param Index 클릭한 그리드 인덱스
	 */
	void ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);

	/**
	 * 클릭한 슬롯을 부분적으로 채워야 하는지 확인합니다
	 * @param RoomInClickedSlot 클릭한 슬롯의 남은 공간
	 * @param HoveredStackCount 호버 아이템의 스택 수량
	 * @return 클릭한 슬롯이 호버 아이템의 일부만 담을 수 있는 경우 true (예: 호버 50개, 슬롯 공간 20개 남음)
	 */
	bool ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const;

	/**
	 * 클릭한 슬롯을 최대치까지 채우고 나머지는 호버 아이템에 유지합니다
	 * @param FillAmount 슬롯에 채울 수량 (슬롯의 남은 공간)
	 * @param Remainder 호버 아이템에 남을 수량
	 * @param Index 클릭한 그리드 인덱스
	 */
	void FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index);

	/**
	 * 아이템 팝업 메뉴를 생성하고 표시합니다
	 * 우클릭으로 아이템을 선택했을 때 드롭, 분할, 소비 옵션을 제공합니다
	 * @param GridIndex 우클릭한 그리드 인덱스
	 */
	void CreateItemPopUp(const int32 GridIndex);

	void PutHoverItemBack();
	
	/**
	 * 슬롯에 배치된 아이템이 클릭되었을 때 호출됩니다
	 * @param GridIndex 그리드 인덱스
	 * @param MouseEvent 마우스 이벤트
	 */
	UFUNCTION()
	void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	/**
	 * 그리드 슬롯이 클릭되었을 때 호출됩니다
	 * @param GridIndex 그리드 인덱스
	 * @param MouseEvent 마우스 이벤트
	 */
	UFUNCTION()
	void OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	/**
	 * 그리드 슬롯에 마우스가 올라갔을 때 호출됩니다
	 * @param GridIndex 그리드 인덱스
	 * @param MouseEvent 마우스 이벤트
	 */
	UFUNCTION()
	void OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent);

	/**
	 * 그리드 슬롯에서 마우스가 벗어났을 때 호출됩니다
	 * @param GridIndex 그리드 인덱스
	 * @param MouseEvent 마우스 이벤트
	 */
	UFUNCTION()
	void OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent);

	/**
	 * 팝업 메뉴에서 분할 옵션이 선택되었을 때 호출됩니다
	 * 스택 가능한 아이템의 일부를 분할하여 호버 아이템으로 만듭니다
	 * @param SplitAmount 분할할 스택 수량
	 * @param Index 아이템이 위치한 그리드 인덱스
	 */
	UFUNCTION()
	void OnPopUpMenuSplit(int32 SplitAmount, int32 Index);

	/**
	 * 팝업 메뉴에서 드롭 옵션이 선택되었을 때 호출됩니다
	 * 아이템을 픽업하고 드롭 처리를 수행합니다
	 * @param Index 아이템이 위치한 그리드 인덱스
	 */
	UFUNCTION()
	void OnPopUpMenuDrop(int32 Index);

	/**
	 * 팝업 메뉴에서 소비 옵션이 선택되었을 때 호출됩니다
	 * 소비 가능한 아이템을 사용합니다 (현재 미구현)
	 * @param Index 아이템이 위치한 그리드 인덱스
	 */
	UFUNCTION()
	void OnPopUpMenuConsume(int32 Index);

	/**
	 * 타일 파라미터가 업데이트되었을 때 호출됩니다
	 * 호버 아이템의 배치 가능 여부를 확인하고 해당 위치의 슬롯들을 하이라이트합니다
	 * @param Parameters 업데이트된 타일 파라미터 (좌표, 인덱스, 사분면 포함)
	 */
	void OnTileParametersUpdated(const FInv_TileParameters& Parameters);

	UFUNCTION()
	void OnInventoryMenuToggled(bool bOpen);
	
	/** 인벤토리 컴포넌트에 대한 약한 참조 */
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;

	/** 이 그리드를 소유하는 캔버스 패널에 대한 약한 참조 (팝업 메뉴 표시용) */
	TWeakObjectPtr<UCanvasPanel> OwningCanvasPanel;
	
	/** 이 그리드가 표시하는 아이템 카테고리 (장비, 소비품, 제작 재료) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta=(AllowPrivateAccess = "true"))
	EInv_ItemCategory ItemCategory;

	/** 그리드 슬롯과 아이템들을 배치할 캔버스 패널 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> GridCanvasPanel;

	/** 슬롯에 배치될 아이템 위젯의 클래스 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_SlottedItem> SlottedItemClass;
	
	/**
	 * 그리드 인덱스를 키로, 해당 위치에 배치된 슬롯 아이템 위젯을 값으로 저장하는 맵
	 * 키: 아이템의 좌상단 모서리가 위치한 그리드 인덱스 (0부터 시작)
	 * 값: 해당 위치에 배치된 슬롯 아이템 위젯
	 * 아이템의 빠른 검색과 제거를 위해 사용됩니다
	 */
	UPROPERTY()
	TMap<int32, TObjectPtr<UInv_SlottedItem>> SlottedItems;

	/** 그리드 슬롯 위젯의 클래스 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_GridSlot> GridSlotClass;

	/** 생성된 모든 그리드 슬롯 위젯들의 배열 */
	UPROPERTY()
	TArray<TObjectPtr<UInv_GridSlot>> GridSlots;

	/** 드래그 중인 아이템을 표시할 호버 아이템 위젯의 클래스 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_HoverItem> HoverItemClass;

	/** 현재 드래그 중인 호버 아이템 위젯 */
	UPROPERTY()
	TObjectPtr<UInv_HoverItem> HoverItem;

	/** 현재 타일 파라미터 (좌표, 인덱스, 사분면) */
	FInv_TileParameters TileParameters;

	/** 이전 타일 파라미터 */
	FInv_TileParameters LastTileParameters;

	/** 아이템을 놓을 인덱스 */
	int32 ItemDropIndex{INDEX_NONE};

	/** 현재 공간 쿼리 결과 */
	FInv_SpaceQueryResult CurrentQueryResult;

	/** 마우스가 캔버스 내에 있는지 여부 */
	bool bMouseWithinCanvas;

	/** 이전 프레임에 마우스가 캔버스 내에 있었는지 여부 */
	bool bLastMouseWithinCanvas;

	/** 마지막으로 하이라이트된 인덱스 */
	int32 LastHighlightedIndex;

	/** 마지막으로 하이라이트된 차원 */
	FIntPoint LastHighlightedDimensions;

	/** 표시 가능한 커서 위젯의 클래스 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> VisibleCursorWidgetClass;

	/** 숨겨진 커서 위젯의 클래스 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> HiddenCursorWidgetClass;

	/** 생성된 표시 가능한 커서 위젯 인스턴스 */
	UPROPERTY()
	TObjectPtr<UUserWidget> VisibleCursorWidget;

	/** 생성된 숨겨진 커서 위젯 인스턴스 */
	UPROPERTY()
	TObjectPtr<UUserWidget> HiddenCursorWidget;

	/** 아이템 팝업 메뉴 위젯의 클래스 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_ItemPopUp> ItemPopUpClass;

	/** 현재 표시 중인 아이템 팝업 메뉴 위젯 */
	UPROPERTY()
	TObjectPtr<UInv_ItemPopUp> ItemPopUp;

	/** 팝업 메뉴의 위치 오프셋 (마우스 위치로부터) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D ItemPopUpOffset;
	
	/** 그리드의 행 수 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows;

	/** 그리드의 열 수 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns;

	/** 각 타일의 크기 (픽셀) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float TileSize;
};



