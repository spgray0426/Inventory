// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/Inv_GridTypes.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "Inv_InventoryStatics.generated.h"

class UInv_InventoryBase;
class UInv_HoverItem;
class UInv_ItemComponent;
class UInv_InventoryComponent;

/**
 * 인벤토리 시스템의 정적 유틸리티 함수들을 제공하는 블루프린트 함수 라이브러리
 * 그리드 수학, 컴포넌트 검색 등의 공통 기능을 제공합니다
 */
UCLASS()
class INVENTORY_API UInv_InventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * 플레이어 컨트롤러에서 인벤토리 컴포넌트를 가져옵니다
	 * @param PlayerController 대상 플레이어 컨트롤러
	 * @return 인벤토리 컴포넌트, 없으면 nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UInv_InventoryComponent* GetInventoryComponent(const APlayerController* PlayerController);

	/**
	 * 아이템 컴포넌트에서 아이템 카테고리를 가져옵니다
	 * @param ItemComp 대상 아이템 컴포넌트
	 * @return 아이템 카테고리 (장비, 소비품, 제작 재료 등)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static EInv_ItemCategory GetItemCategoryFromItemComp(UInv_ItemComponent* ItemComp);

	/**
	 * 2D 범위에 대해 1D 배열을 순회하는 템플릿 함수
	 * 선형 인덱스를 2D 좌표로 변환하고 그 역변환도 수행합니다
	 * 그리드 기반 인벤토리에서 아이템이 차지하는 모든 슬롯에 대해 작업할 때 사용됩니다
	 * @tparam T 배열의 요소 타입
	 * @tparam FuncT 각 요소에 대해 실행할 함수 타입
	 * @param Array 작업할 1D 배열
	 * @param Index 시작 인덱스 (왼쪽 위 모서리)
	 * @param Range2D 2D 범위 (너비, 높이)
	 * @param GridColumns 그리드의 열 개수
	 * @param Function 각 요소에 대해 실행할 함수
	 */
	template<typename T, typename FuncT>
	static void ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns,const FuncT& Function);

	/**
	 * 아이템에 마우스를 올렸을 때 호출됩니다
	 * 인벤토리 UI에 아이템 설명을 표시하는 이벤트를 트리거합니다
	 * @param PC 플레이어 컨트롤러
	 * @param Item 마우스를 올린 아이템
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static void ItemHovered(APlayerController* PC, UInv_InventoryItem* Item);

	/**
	 * 아이템에서 마우스를 뗐을 때 호출됩니다
	 * 인벤토리 UI의 아이템 설명을 숨기는 이벤트를 트리거합니다
	 * @param PC 플레이어 컨트롤러
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static void ItemUnhovered(APlayerController* PC);

	/**
	 * 플레이어 컨트롤러의 호버 아이템 위젯을 가져옵니다
	 * 호버 아이템은 마우스 커서를 따라다니며 현재 들고 있는 아이템을 표시합니다
	 * @param PC 플레이어 컨트롤러
	 * @return 호버 아이템 위젯 (유효하지 않으면 nullptr)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UInv_HoverItem* GetHoverItem(APlayerController* PC);

	/**
	 * 플레이어 컨트롤러의 인벤토리 위젯을 가져옵니다
	 * 인벤토리 컴포넌트에서 메인 인벤토리 UI 위젯을 반환합니다
	 * @param PC 플레이어 컨트롤러
	 * @return 인벤토리 베이스 위젯 (유효하지 않으면 nullptr)
	 */
	static UInv_InventoryBase* GetInventoryWidget(APlayerController* PC);
};

/**
 * 2D 범위 순회 템플릿 함수 구현
 * 시작 인덱스로부터 지정된 2D 범위 내의 모든 요소에 대해 함수를 실행합니다
 * 예: 3x2 크기 아이템의 경우, 6개의 슬롯에 대해 함수를 실행합니다
 */
template <typename T, typename FuncT>
void UInv_InventoryStatics::ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns, const FuncT& Function)
{
	// Y축 (세로) 순회
	for (int32 j = 0; j < Range2D.Y; ++j)
	{
		// X축 (가로) 순회
		for (int32 i = 0; i < Range2D.X; ++i)
		{
			// 시작 인덱스를 2D 좌표로 변환하고 오프셋 추가
			const FIntPoint Coordinates = UInv_WidgetUtils::GetPositionFromIndex(Index, GridColumns) + FIntPoint(i, j);

			// 2D 좌표를 다시 1D 인덱스로 변환
			const int32 TileIndex = UInv_WidgetUtils::GetIndexFromPosition(Coordinates, GridColumns);

			// 인덱스가 유효한 경우에만 함수 실행
			if ( Array.IsValidIndex(TileIndex) )
			{
				Function(Array[TileIndex]);
			}
		}
	}
}
