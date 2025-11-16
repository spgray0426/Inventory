// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Inv_GridTypes.h"
#include "Inv_InventoryBase.generated.h"

class UInv_HoverItem;
class UInv_ItemComponent;
class UInv_InventoryItem;
/**
 * 인벤토리 UI의 기본 클래스
 * 모든 인벤토리 위젯이 상속받아야 하는 추상 기본 클래스입니다
 * 공간 확인, 아이템 호버 처리 등의 공통 인터페이스를 제공합니다
 */
UCLASS()
class INVENTORY_API UInv_InventoryBase : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 아이템을 배치할 공간이 있는지 확인합니다
	 * 파생 클래스에서 오버라이드하여 구체적인 공간 확인 로직을 구현합니다
	 * @param ItemComponent 배치할 아이템 컴포넌트
	 * @return 슬롯 사용 가능 여부 결과
	 */
	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent) const { return FInv_SlotAvailabilityResult();}

	/**
	 * 아이템에 마우스를 올렸을 때 호출됩니다
	 * 파생 클래스에서 오버라이드하여 아이템 설명 표시 등의 처리를 구현합니다
	 * @param Item 마우스를 올린 아이템
	 */
	virtual void OnItemHovered(UInv_InventoryItem* Item) {}

	/**
	 * 아이템에서 마우스를 뗐을 때 호출됩니다
	 * 파생 클래스에서 오버라이드하여 아이템 설명 숨김 등의 처리를 구현합니다
	 */
	virtual void OnItemUnHovered() {}

	/**
	 * 현재 드래그 중인 아이템이 있는지 확인합니다
	 * 드래그 중일 때는 아이템 설명을 표시하지 않기 위해 사용됩니다
	 * @return 호버 아이템이 있으면 true
	 */
	virtual bool HasHoverItem() const { return false; }

	/**
	 * 호버 아이템 위젯을 가져옵니다
	 * 호버 아이템은 마우스 커서를 따라다니며 현재 들고 있는 아이템을 표시합니다
	 * @return 호버 아이템 위젯 (기본 구현은 nullptr 반환)
	 */
	virtual UInv_HoverItem* GetHoverItem() const { return nullptr; }

	/**
	 * 그리드의 타일 크기를 가져옵니다
	 * 타일 크기는 그리드의 각 셀이 차지하는 픽셀 단위 크기입니다
	 * 장착 아이템의 크기를 계산하는 데 사용됩니다
	 * @return 타일 크기 (픽셀 단위, 기본 구현은 0 반환)
	 */
	virtual float GetTileSize() const { return 0.f; }
};
