// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_SlottedItem.generated.h"

class UTextBlock;
class UInv_InventoryItem;
class UImage;

/**
 * 슬롯 아이템 클릭 이벤트 델리게이트
 * 그리드 인덱스와 마우스 이벤트를 전달합니다
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlottedItemCliced, int32, GridIndex, const FPointerEvent&, MouseEvent);

/**
 * 슬롯 아이템 위젯 클래스
 * 그리드에 배치된 아이템을 표시합니다
 * 아이템 아이콘, 스택 개수, 클릭 이벤트 등을 관리합니다
 */
UCLASS()
class INVENTORY_API UInv_SlottedItem : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 마우스 버튼이 눌렸을 때 호출됩니다
	 * @param MyGeometry 위젯의 지오메트리 정보
	 * @param MouseEvent 마우스 이벤트 정보
	 * @return 이벤트 처리 결과
	 */
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/**
	 * 마우스가 위젯 위로 들어왔을 때 호출됩니다
	 * 아이템 설명 표시를 트리거합니다
	 * @param MyGeometry 위젯의 지오메트리 정보
	 * @param MouseEvent 마우스 이벤트 정보
	 */
	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/**
	 * 마우스가 위젯에서 벗어났을 때 호출됩니다
	 * 아이템 설명 숨김을 트리거합니다
	 * @param MouseEvent 마우스 이벤트 정보
	 */
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	/** 아이템이 스택 가능한지 확인합니다 */
	bool IsStackable() const { return bIsStackable; }

	/** 스택 가능 여부를 설정합니다 */
	void SetIsStackable(bool bStackable) { bIsStackable = bStackable; }

	/** 아이콘 이미지를 가져옵니다 */
	UImage* GetImage_Icon() const { return Image_Icon; }

	/** 그리드 인덱스를 설정합니다 */
	void SetGridIndex(int32 Index) { GridIndex = Index; }

	/** 그리드 인덱스를 가져옵니다 */
	int32 GetGridIndex() const { return GridIndex; }

	/** 그리드 차원을 설정합니다 */
	void SetGridDimensions(const FIntPoint& Dimenstions) { GridDimensions = Dimenstions; }

	/** 그리드 차원을 가져옵니다 */
	FIntPoint GetGridDimensions() const { return GridDimensions; }

	/**
	 * 인벤토리 아이템을 설정합니다
	 * @param Item 설정할 인벤토리 아이템
	 */
	void SetInventoryItem(UInv_InventoryItem* Item);

	/** 인벤토리 아이템을 가져옵니다 */
	UInv_InventoryItem* GetInventoryItem() const { return InventoryItem.Get(); }

	/**
	 * 아이템 이미지 브러시를 설정합니다
	 * @param Brush 적용할 슬레이트 브러시
	 */
	void SetImageBrush(const FSlateBrush& Brush) const;

	/**
	 * 스택 개수를 업데이트합니다
	 * @param StackCount 표시할 스택 개수
	 */
	void UpdateStackCount(int32 StackCount);

	/** 슬롯 아이템이 클릭되었을 때 발생하는 이벤트 */
	FSlottedItemCliced OnSlottedItemClicked;
private:

	/** 아이템 아이콘 이미지 위젯 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	/** 스택 개수 텍스트 위젯 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	/** 이 아이템이 배치된 그리드 인덱스 */
	int32 GridIndex;

	/** 아이템의 그리드 차원 (너비, 높이) */
	FIntPoint GridDimensions;

	/** 인벤토리 아이템에 대한 약한 참조 */
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;

	/** 아이템이 스택 가능한지 여부 */
	bool bIsStackable;
	
};
