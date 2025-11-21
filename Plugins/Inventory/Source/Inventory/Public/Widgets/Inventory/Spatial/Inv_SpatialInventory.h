// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"
#include "Widgets/ItemDescription/Inv_ItemDescription.h"
#include "Inv_SpatialInventory.generated.h"

class UInv_EquippedSlottedItem;
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

	/**
	 * 현재 활성화된 그리드의 호버 아이템 위젯을 가져옵니다
	 * @return 호버 아이템 위젯
	 */
	virtual UInv_HoverItem* GetHoverItem() const override;

	/**
	 * 현재 활성화된 그리드의 타일 크기를 가져옵니다
	 * @return 타일 크기 (픽셀 단위)
	 */
	virtual float GetTileSize() const override;
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
	 * 장착 그리드 슬롯이 클릭되었을 때 호출됩니다
	 * 호버 아이템을 해당 슬롯에 장착하는 로직을 처리합니다
	 * @param EquippedGridSlot 클릭된 장착 슬롯
	 * @param EquipmentTypeTag 장비 타입 태그 (예: Weapon, Armor 등)
	 */
	UFUNCTION()
	void EquippedGridSlotClicked(UInv_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag);

	/**
	 * 장착된 아이템이 클릭되었을 때 호출됩니다
	 * 장착된 아이템을 해제하거나 다른 작업을 처리합니다
	 * @param EquippedSlottedItem 클릭된 장착 아이템 위젯
	 */
	UFUNCTION()
	void EquippedSlottedItemClicked(UInv_EquippedSlottedItem* EquippedSlottedItem);
	
	UFUNCTION()
	void ShowEquippedItemDescription(UInv_InventoryItem* Item);
	
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

	/**
	 * 호버 아이템을 지정된 장비 슬롯에 장착할 수 있는지 확인합니다
	 *
	 * 다음 조건들을 모두 만족해야 true를 반환합니다:
	 * - 슬롯이 유효하고 비어있어야 함
	 * - 호버 아이템이 존재해야 함
	 * - 호버 아이템이 스택 가능한 아이템이 아니어야 함
	 * - 호버 아이템이 장비 카테고리여야 함
	 * - 아이템의 장비 타입이 슬롯의 장비 타입 태그와 일치해야 함
	 *
	 * @param EquippedGridSlot 확인할 장비 슬롯
	 * @param EquipmentTypeTag 슬롯이 요구하는 장비 타입 태그 (예: Weapon, Helmet)
	 * @return 장착 가능하면 true, 불가능하면 false
	 */
	bool CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const;

	/**
	 * 특정 아이템이 장착된 슬롯을 찾습니다
	 *
	 * 모든 장비 슬롯을 순회하여 지정된 아이템을 포함하는 슬롯을 찾습니다.
	 * 장비 해제 시 어느 슬롯에서 아이템을 제거해야 하는지 알기 위해 사용됩니다.
	 *
	 * @param EquippedItem 찾을 장착된 아이템
	 * @return 아이템이 장착된 슬롯 (찾지 못하면 nullptr)
	 */
	UInv_EquippedGridSlot* FindSlotWithEquippedItem(UInv_InventoryItem* EquippedItem) const;

	/**
	 * 장비 슬롯에서 아이템을 제거합니다
	 *
	 * 슬롯의 인벤토리 아이템과 장착 아이템 위젯을 모두 nullptr로 설정하여
	 * 슬롯을 비어있는 상태로 만듭니다. 장비 해제 시 사용됩니다.
	 *
	 * @param EquippedGridSlot 비울 장비 슬롯
	 */
	void ClearSlotOfItem(UInv_EquippedGridSlot* EquippedGridSlot);

	/**
	 * 장착 아이템 위젯을 제거하고 정리합니다
	 *
	 * 동작 순서:
	 * 1. 클릭 이벤트 델리게이트 바인딩 해제
	 * 2. 부모 위젯에서 제거 (시각적으로 사라짐)
	 *
	 * 장비 해제 시 UI에서 장착 아이템 위젯을 제거하기 위해 사용됩니다.
	 *
	 * @param EquippedSlottedItem 제거할 장착 아이템 위젯
	 */
	void RemoveEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem);

	/**
	 * 장착 슬롯에 새로운 장착 아이템 위젯을 생성합니다
	 *
	 * 장비 교체 시 호출되며, 기존 장착 아이템의 정보를 사용하여
	 * 새로운 아이템을 같은 슬롯에 장착합니다.
	 *
	 * 동작 순서:
	 * 1. 슬롯의 OnItemEquipped() 호출하여 위젯 생성
	 * 2. 기존 장착 아이템의 장비 타입 태그 사용
	 * 3. 새 위젯에 클릭 이벤트 바인딩
	 * 4. 슬롯에 위젯 참조 설정
	 *
	 * 사용 시나리오:
	 * - A 무기를 들고 B 무기를 클릭: B를 해제하고 A를 장착
	 * - 빈 슬롯에서 장착된 아이템 클릭: 해제 후 다른 아이템 장착
	 *
	 * @param EquippedSlottedItem 기존 장착 아이템 위젯 (장비 타입 태그 참조용)
	 * @param EquippedGridSlot 아이템을 장착할 슬롯
	 * @param ItemToEquip 장착할 새 인벤토리 아이템
	 */
	void MakeEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem, UInv_EquippedGridSlot* EquippedGridSlot, UInv_InventoryItem* ItemToEquip);

	/**
	 * 장비 장착/해제 델리게이트를 브로드캐스트합니다
	 *
	 * 서버와 클라이언트 모두에서 장비 상태 변경을 알리기 위해
	 * 인벤토리 컴포넌트의 델리게이트를 브로드캐스트합니다.
	 *
	 * 동작:
	 * 1. 서버에 장비 교체 RPC 전송 (모든 네트워크 모드에서)
	 * 2. 데디케이티드 서버가 아닌 경우:
	 *    - OnItemEquipped 브로드캐스트 (새 아이템 장착 알림)
	 *    - OnItemUnequipped 브로드캐스트 (기존 아이템 해제 알림)
	 *
	 * 네트워크 고려사항:
	 * - Server_EquipSlotClicked는 모든 클라이언트에서 호출
	 * - UI 델리게이트는 데디케이티드 서버에서는 불필요 (UI 없음)
	 * - 리슨 서버와 클라이언트는 UI 업데이트를 위해 델리게이트 필요
	 *
	 * @param ItemToEquip 장착할 아이템 (nullptr 가능)
	 * @param ItemToUnequip 해제할 아이템 (nullptr 가능)
	 */
	void BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip) const;
	
	void SetEquippedItemDescriptionSizeAndPosition(UInv_ItemDescription* Description, UInv_ItemDescription* EquippedDescription, UCanvasPanel* Canvas) const;
	
	UInv_ItemDescription* GetEquippedItemDescription();
	
	/** 위젯 트리에서 찾은 모든 장비 슬롯들의 배열 (무기, 방어구 등의 장착 슬롯) */
	UPROPERTY()
	TArray<TObjectPtr<UInv_EquippedGridSlot>> EquippedGridSlots;

	/** 호버 아이템과 아이템 설명 위젯을 표시할 루트 캔버스 패널 (블루프린트에서 바인딩됨) */
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
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_ItemDescription> EquippedItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UInv_ItemDescription> EquippedItemDescription;
	
	/** 아이템 설명 표시를 지연시키는 타이머 핸들 */
	FTimerHandle DescriptionTimer;

	FTimerHandle EquippedDescriptionTimer;
	
	/** 아이템 설명이 표시되기까지의 지연 시간 (초). 기본값은 0.5초 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DescriptionTimerDelay {0.5f};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float EquippedDescriptionTimerDelay {0.5f};
};


