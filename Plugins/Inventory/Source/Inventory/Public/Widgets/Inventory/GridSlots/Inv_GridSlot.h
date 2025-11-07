// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UInv_ItemPopUp;
class UInv_InventoryItem;
class UImage;

/**
 * 그리드 슬롯 이벤트 델리게이트
 * 그리드 인덱스와 마우스 이벤트를 전달합니다
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent, int32, GridIndex, const FPointerEvent&, MouseEvent);

/**
 * 그리드 슬롯의 시각적 상태를 나타내는 열거형
 */
UENUM(BlueprintType)
enum class EInv_GridSlotState : uint8
{
	/** 비어있는 슬롯 */
	Unoccupied,
	/** 아이템이 있는 슬롯 */
	Occupied,
	/** 선택된 슬롯 */
	Selected,
	/** 비활성화된 슬롯 (아이템을 놓을 수 없음) */
	GrayedOut
};

/**
 * 그리드 슬롯 위젯 클래스
 * 인벤토리 그리드의 개별 슬롯을 나타냅니다
 * 아이템 배치, 마우스 상호작용, 시각적 상태 관리를 담당합니다
 */
UCLASS()
class INVENTORY_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 마우스가 슬롯에 들어갈 때 호출됩니다
	 * GridSlotHovered 이벤트를 브로드캐스트합니다
	 * @param MyGeometry 위젯의 지오메트리 정보
	 * @param MouseEvent 마우스 이벤트 정보
	 */
	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/**
	 * 마우스가 슬롯에서 나갈 때 호출됩니다
	 * GridSlotUnhovered 이벤트를 브로드캐스트합니다
	 * @param MouseEvent 마우스 이벤트 정보
	 */
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;

	/**
	 * 마우스 버튼이 슬롯 위에서 눌렸을 때 호출됩니다
	 * GridSlotClicked 이벤트를 브로드캐스트합니다
	 * @param MyGeometry 위젯의 지오메트리 정보
	 * @param MouseEvent 마우스 이벤트 정보
	 * @return 이벤트 처리 결과
	 */
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/** 타일 인덱스를 설정합니다 */
	void SetTileIndex(int32 Index) { TileIndex = Index; }

	/** 타일 인덱스를 가져옵니다 */
	int32 GetTileIndex() const { return TileIndex; }

	/** 그리드 슬롯의 현재 상태를 가져옵니다 */
	EInv_GridSlotState GetGridSlotState() const { return GridSlotState; }

	/** 이 슬롯에 배치된 인벤토리 아이템을 가져옵니다 */
	TWeakObjectPtr<UInv_InventoryItem> GetInventoryItem() const { return InventoryItem; }

	/**
	 * 이 슬롯에 인벤토리 아이템을 설정합니다
	 * @param Item 설정할 인벤토리 아이템
	 */
	void SetInventoryItem(UInv_InventoryItem* Item);

	/** 스택 수량을 가져옵니다 */
	int32 GetStackCount() const { return StackCount; }

	/** 스택 수량을 설정합니다 */
	void SetStackCount(int32 Count) { StackCount = Count; }

	/** 그리드 인덱스를 가져옵니다 */
	int32 GetIndex() const { return TileIndex; }

	/** 그리드 인덱스를 설정합니다 */
	void SetIndex(int32 Index) { TileIndex = Index; }

	/** 아이템의 좌상단 그리드 인덱스를 가져옵니다 (아이템이 여러 슬롯을 차지하는 경우) */
	int32 GetUpperLeftGridIndex() const { return UpperLeftGridIndex; }

	/** 아이템의 좌상단 그리드 인덱스를 설정합니다 */
	void SetUpperLeftGridIndex(int32 Index) { UpperLeftGridIndex = Index; }

	/** 이 슬롯이 사용 가능한지 확인합니다 */
	bool IsAvailable() const { return bAvailable; }

	/** 슬롯의 사용 가능 여부를 설정합니다 */
	void SetAvailable(bool bIsAvailable) { bAvailable = bIsAvailable; }

	/** 점유된 상태의 텍스처를 설정합니다 */
	void SetOccupiedTexture();

	/** 비어있는 상태의 텍스처를 설정합니다 */
	void SetUnoccupiedTexture();

	/** 선택된 상태의 텍스처를 설정합니다 */
	void SetSelectedTexture();

	/** 비활성화 상태의 텍스처를 설정합니다 */
	void SetGrayedOutTexture();

	void SetItemPopUp(UInv_ItemPopUp* PopUp);
	UInv_ItemPopUp* GetItemPopUp() const;
	
	/** 그리드 슬롯이 클릭되었을 때 발생하는 이벤트 */
	FGridSlotEvent GridSlotClicked;

	/** 그리드 슬롯에 마우스가 올라갔을 때 발생하는 이벤트 */
	FGridSlotEvent GridSlotHovered;

	/** 그리드 슬롯에서 마우스가 벗어났을 때 발생하는 이벤트 */
	FGridSlotEvent GridSlotUnhovered;
private:

	UFUNCTION()
	void OnItemPopUpDestruct(UUserWidget* Menu);
	
	/** 이 슬롯의 타일 인덱스 (그리드 내 위치) */
	int32 TileIndex{INDEX_NONE};

	/** 이 슬롯에 있는 아이템의 스택 수량 */
	int32 StackCount{0};

	/** 아이템의 좌상단 그리드 인덱스 (다중 슬롯 아이템의 경우) */
	int32 UpperLeftGridIndex{INDEX_NONE};

	/** 이 슬롯이 사용 가능한지 여부 (아이템을 놓을 수 있는지) */
	bool bAvailable{true};
	
	/** 이 슬롯에 배치된 인벤토리 아이템에 대한 약한 참조 */
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;

	TWeakObjectPtr<UInv_ItemPopUp> ItemPopUp;

	/** 그리드 슬롯의 이미지 위젯 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	/** 비어있는 상태의 브러시 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;

	/** 점유된 상태의 브러시 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;

	/** 선택된 상태의 브러시 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;

	/** 비활성화 상태의 브러시 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GrayedOut;

	/** 그리드 슬롯의 현재 시각적 상태 */
	EInv_GridSlotState GridSlotState;

	
};
