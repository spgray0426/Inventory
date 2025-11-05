// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Inv_InventoryItem.h"
#include "Types/Inv_GridTypes.h"
#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"
#include "Inv_InventoryGrid.generated.h"

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
	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_InventoryItem* Item);

	/**
	 * 아이템 매니페스트를 위한 공간이 있는지 확인합니다
	 * @param Manifest 확인할 아이템 매니페스트
	 * @return 슬롯 사용 가능 여부 결과
	 */
	FInv_SlotAvailabilityResult HasRoomForItem(const FInv_ItemManifest& Manifest);

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
	bool HasRoomAtIndex(const UInv_GridSlot* GridSlot,
		const FIntPoint& Dimensions,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize);
	bool CheckSlotConstraints(const UInv_GridSlot* GridSlot,
		const UInv_GridSlot* SubGridSlot,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize) const;
	bool HasValidItem(const UInv_GridSlot* GridSlot) const;
	bool IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const;
	bool DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const;
	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const;
	int32 DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UInv_GridSlot* GridSlot) const;
	int32 GetStackAmount(const UInv_GridSlot* GridSlot) const;
	
	FIntPoint GetItemDimensions(const FInv_ItemManifest& Manifest) const;
	
	bool MatchesCategory(const UInv_InventoryItem* Item) const;
	FVector2D GetDrawSize(const FInv_GridFragment* GridFragment) const;
	void SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment) const;
	void AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);
	
	/**
	 * 기존 스택에 스택을 추가합니다
	 * @param Result 슬롯 사용 가능 여부 결과
	 */
	UFUNCTION()
	void AddStacks(const FInv_SlotAvailabilityResult& Result);

	/**
	 * 슬롯에 배치된 아이템이 클릭되었을 때 호출됩니다
	 * @param GrideIndex 그리드 인덱스
	 * @param MouseEvent 마우스 이벤트
	 */
	UFUNCTION()
	void OnSlottedItemClicked(int32 GrideIndex, const FPointerEvent& MouseEvent);

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
	 * 호버 아이템을 할당합니다
	 * @param InventoryItem 인벤토리 아이템
	 */
	void AssignHoverItem(UInv_InventoryItem* InventoryItem);

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
	void UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition);

	FIntPoint CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	EInv_TileQuadrant CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	void OnTileParametersUpdated(const FInv_TileParameters& Parameters);
	FIntPoint CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const EInv_TileQuadrant Quadrant) const;

	FInv_SpaceQueryResult CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions);
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
	 * 호버 아이템을 제거합니다
	 */
	void ClearHoverItem();

	/** 인벤토리 컴포넌트에 대한 약한 참조 */
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;

	/** 이 그리드가 표시하는 아이템 카테고리 (장비, 소비품, 제작 재료) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta=(AllowPrivateAccess = "true"))
	EInv_ItemCategory ItemCategory;

	/** 그리드 슬롯과 아이템들을 배치할 캔버스 패널 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> GridCanvasPanel;

	/** 슬롯에 배치될 아이템 위젯의 클래스 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_SlottedItem> SlottedItemClass;

	/** 인덱스와 슬롯 아이템 위젯의 맵 */
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


