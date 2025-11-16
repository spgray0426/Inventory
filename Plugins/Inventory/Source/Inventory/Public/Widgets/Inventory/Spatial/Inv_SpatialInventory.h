// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"
#include "Widgets/ItemDescription/Inv_ItemDescription.h"
#include "Inv_SpatialInventory.generated.h"

class UInv_EquippedGridSlot;
class UInv_ItemDescription;
class UCanvasPanel;
class UButton;
class UWidgetSwitcher;
class UInv_InventoryGrid;

/**
 * 공간 인벤토리 위젯 클래스
 * 탭 기반의 인벤토리 UI를 제공합니다
 * 장비, 소비품, 제작 재료 카테고리로 분류된 그리드를 관리합니다
 */
UCLASS()
class INVENTORY_API UInv_SpatialInventory : public UInv_InventoryBase
{
	GENERATED_BODY()

public:
	/**
	 * 위젯 초기화 시 호출됩니다
	 * 버튼 이벤트를 바인딩하고 초기 그리드를 설정합니다
	 */
	virtual void NativeOnInitialized() override;

	/**
	 * 마우스 버튼이 눌렸을 때 호출됩니다
	 * 그리드 외부 클릭 시 호버 아이템 드롭 등을 처리합니다
	 * @param MyGeometry 위젯의 지오메트리 정보
	 * @param MouseEvent 마우스 이벤트 정보
	 * @return 이벤트 처리 결과
	 */
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/**
	 * 매 프레임마다 호출됩니다
	 * 아이템 설명 위젯의 위치와 크기를 업데이트합니다
	 * @param MyGeometry 위젯의 지오메트리 정보
	 * @param InDeltaTime 이전 프레임으로부터의 경과 시간
	 */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/**
	 * 아이템을 배치할 공간이 있는지 확인합니다
	 * 현재 활성화된 그리드에서 공간을 확인합니다
	 * @param ItemComponent 확인할 아이템 컴포넌트
	 * @return 슬롯 사용 가능 여부 결과
	 */
	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent) const override;

	/**
	 * 아이템에 마우스를 올렸을 때 호출됩니다
	 * 지연 타이머를 설정하여 일정 시간 후 아이템 설명을 표시합니다
	 * @param Item 마우스를 올린 아이템
	 */
	virtual void OnItemHovered(UInv_InventoryItem* Item) override;

	/**
	 * 아이템에서 마우스를 뗐을 때 호출됩니다
	 * 아이템 설명을 숨기고 타이머를 중지합니다
	 */
	virtual void OnItemUnHovered() override;

	/**
	 * 현재 드래그 중인 호버 아이템이 있는지 확인합니다
	 * 모든 그리드를 검사하여 호버 아이템 존재 여부를 반환합니다
	 * @return 호버 아이템이 있으면 true
	 */
	virtual bool HasHoverItem() const override;
	
	virtual UInv_HoverItem* GetHoverItem() const override;
private:

	/**
	 * 장비 그리드를 표시합니다
	 * 위젯 스위처를 통해 장비 그리드로 전환합니다
	 */
	UFUNCTION()
	void ShowEquippables();

	/**
	 * 소비품 그리드를 표시합니다
	 * 위젯 스위처를 통해 소비품 그리드로 전환합니다
	 */
	UFUNCTION()
	void ShowConsumables();

	/**
	 * 제작 재료 그리드를 표시합니다
	 * 위젯 스위처를 통해 제작 재료 그리드로 전환합니다
	 */
	UFUNCTION()
	void ShowCraftables();
	
	UFUNCTION()
	void EquippedGridSlotClicked(UInv_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag);
	
	/**
	 * 모든 버튼을 비활성화하고 선택된 버튼만 활성화합니다
	 * @param Button 활성화 상태로 유지할 버튼
	 */
	void DisableButtons(UButton* Button);

	/**
	 * 활성 그리드를 설정합니다
	 * @param Grid 활성화할 그리드
	 * @param Button 관련 버튼
	 */
	void SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button);

	/**
	 * 아이템 설명 위젯을 가져옵니다
	 * 처음 호출 시 위젯을 생성하고 캔버스에 추가합니다 (지연 초기화)
	 * @return 아이템 설명 위젯
	 */
	UInv_ItemDescription* GetItemDescription();

	/**
	 * 아이템 설명 위젯의 크기와 위치를 설정합니다
	 * 마우스 커서 근처에 표시하되 화면 밖으로 나가지 않도록 보정합니다
	 * @param Description 설정할 아이템 설명 위젯
	 * @param Canvas 위젯을 포함하는 캔버스 패널
	 */
	void SetItemDescriptionSizeAndPosition(UInv_ItemDescription* Description, UCanvasPanel* Canvas) const;
	
	UPROPERTY()
	TArray<TObjectPtr<UInv_EquippedGridSlot>> EquippedGridSlots;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	/** 카테고리 그리드를 전환하는 위젯 스위처 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	/** 장비 아이템 그리드 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Equippables;

	/** 소비품 아이템 그리드 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Consumables;

	/** 제작 재료 아이템 그리드 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Craftables;

	/** 장비 탭 버튼 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Equippables;

	/** 소비품 탭 버튼 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Consumables;

	/** 제작 재료 탭 버튼 (블루프린트에서 바인딩됨) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Craftables;

	/** 현재 활성화된 그리드에 대한 약한 참조 */
	TWeakObjectPtr<UInv_InventoryGrid> ActiveGrid;

	/** 아이템 설명 위젯 클래스 (에디터에서 설정 가능) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_ItemDescription> ItemDescriptionClass;

	/** 생성된 아이템 설명 위젯 인스턴스 */
	UPROPERTY()
	TObjectPtr<UInv_ItemDescription> ItemDescription;
	
	/** 아이템 설명 표시를 지연시키는 타이머 핸들 */
	FTimerHandle DescriptionTimer;

	/** 아이템 설명이 표시되기까지의 지연 시간 (초). 기본값은 0.5초 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DescriptionTimerDelay {0.5f};

	
};


