// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"
#include "Widgets/ItemDescription/Inv_ItemDescription.h"
#include "Inv_SpatialInventory.generated.h"

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
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/**
	 * 아이템을 배치할 공간이 있는지 확인합니다
	 * 현재 활성화된 그리드에서 공간을 확인합니다
	 * @param ItemComponent 확인할 아이템 컴포넌트
	 * @return 슬롯 사용 가능 여부 결과
	 */
	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent) const override;

	virtual void OnItemHovered(UInv_InventoryItem* Item) override;
	virtual void OnItemUnHovered() override;
	virtual bool HasHoverItem() const override;
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

	UInv_ItemDescription* GetItemDescription();

	void SetItemDescriptionSizeAndPosition(UInv_ItemDescription* Description, UCanvasPanel* Canvas) const;
	
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

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_ItemDescription> ItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UInv_ItemDescription> ItemDescription;

	FTimerHandle DescriptionTimer;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DescriptionTimerDelay {0.5f};

	
};

inline UInv_ItemDescription* UInv_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UInv_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}
