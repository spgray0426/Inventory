// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/Inv_FastArray.h"
#include "Inv_InventoryComponent.generated.h"


struct FInv_SlotAvailabilityResult;
class UInv_ItemComponent;
class UInv_InventoryBase;

/** 인벤토리 아이템이 추가되거나 제거될 때 호출되는 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChange, UInv_InventoryItem*, Item);

/** 인벤토리에 공간이 부족할 때 호출되는 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRoomInInventory);

/** 스택 수량이 변경될 때 호출되는 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackChange, const FInv_SlotAvailabilityResult&, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEquipStatusChanged, UInv_InventoryItem*, Item);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryMenuToggled, bool, bOpen);

/**
 * 인벤토리 관리를 담당하는 액터 컴포넌트
 * 서버 권한 기반의 인벤토리 연산을 처리하며, Fast Array를 통해 인벤토리 상태를 복제합니다
 * 아이템 추가/제거, 메뉴 토글 등의 기능을 제공합니다
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), blueprintable)
class INVENTORY_API UInv_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** 기본 생성자 */
	UInv_InventoryComponent();

	/**
	 * 네트워크 복제를 위한 속성들을 등록합니다
	 * @param OutLifetimeProps 복제 속성을 저장할 배열
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * 아이템을 인벤토리에 추가하려고 시도합니다 (블루프린트에서 호출 가능, 서버에서만)
	 * @param ItemComponent 추가할 아이템 컴포넌트
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void TryAddItem(UInv_ItemComponent* ItemComponent);

	/**
	 * 서버 RPC: 새로운 아이템을 인벤토리에 추가합니다
	 * @param ItemComponent 추가할 아이템 컴포넌트
	 * @param StackCount 추가할 스택 수량
	 */
	UFUNCTION(Server, Reliable)
	void Server_AddNewItem(UInv_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	/**
	 * 서버 RPC: 기존 아이템에 스택을 추가합니다
	 * @param ItemComponent 스택을 추가할 아이템 컴포넌트
	 * @param StackCount 추가할 스택 수량
	 * @param Remainder 남은 수량 (추가할 수 없는 수량)
	 */
	UFUNCTION(Server, Reliable)
	void Server_AddStacksToItem(UInv_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	/**
	 * 서버 RPC: 인벤토리에서 아이템을 드롭합니다
	 * 지정된 스택 수량만큼 인벤토리에서 제거하고 월드에 스폰합니다
	 * @param Item 드롭할 인벤토리 아이템
	 * @param StackCount 드롭할 스택 수량
	 */
	UFUNCTION(Server, Reliable)
	void Server_DropItem(UInv_InventoryItem* Item, int32 StackCount);

	/**
	 * 서버 RPC: 인벤토리에서 아이템을 소비합니다
	 * 스택 수량을 1 감소시키고, 소비 가능 프래그먼트의 OnConsume을 호출합니다
	 * 스택이 0이 되면 인벤토리에서 아이템을 제거합니다
	 * @param Item 소비할 인벤토리 아이템
	 */
	UFUNCTION(Server, Reliable)
	void Server_ConsumeItem(UInv_InventoryItem* Item);
	
	
	UFUNCTION(Server, Reliable)
	void Server_EquipSlotClicked(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EquipSlotClicked(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip);
	
	/**
	 * 인벤토리 메뉴를 열거나 닫습니다
	 */
	void ToggleInventoryMenu();

	/**
	 * 복제 서브 객체를 추가합니다
	 * @param SubObj 추가할 서브 객체
	 */
	void AddRepSubObj(UObject* SubObj);

	/**
	 * 드롭된 아이템을 월드에 스폰합니다
	 * 플레이어의 전방에 랜덤한 각도와 거리로 아이템을 생성합니다
	 * @param Item 드롭할 인벤토리 아이템
	 * @param StackCount 드롭할 스택 수량
	 */
	void SpawnDroppedItem(UInv_InventoryItem* Item, int32 StackCount);

	UInv_InventoryBase* GetInventoryMenu() const { return InventoryMenu; }
	
	bool IsMenuOpen() const { return bInventoryMenuOpen; }
	
	/** 아이템이 추가될 때 호출되는 델리게이트 */
	FInventoryItemChange OnItemAdded;

	/** 아이템이 제거될 때 호출되는 델리게이트 */
	FInventoryItemChange OnItemRemoved;

	/** 인벤토리에 공간이 부족할 때 호출되는 델리게이트 */
	FNoRoomInInventory NoRoomInInventory;

	/** 스택 수량이 변경될 때 호출되는 델리게이트 */
	FStackChange OnStackChange;
	
	FItemEquipStatusChanged OnItemEquipped;
	FItemEquipStatusChanged OnItemUnequipped;
	FInventoryMenuToggled OnInventoryMenuToggled;
	
protected:
	/**
	 * 플레이 시작 시 호출됩니다
	 * 인벤토리를 초기화합니다
	 */
	virtual void BeginPlay() override;

private:
	/**
	 * 인벤토리를 구성합니다
	 * 인벤토리 메뉴 위젯을 생성하고 초기화합니다
	 */
	void ConstructInventory();

	/**
	 * 인벤토리 메뉴를 엽니다
	 */
	void OpenInventoryMenu();

	/**
	 * 인벤토리 메뉴를 닫습니다
	 */
	void CloseInventoryMenu();

	/** 이 컴포넌트를 소유하는 플레이어 컨트롤러의 약한 참조 */
	TWeakObjectPtr<APlayerController> OwningController;

	/** 인벤토리 아이템 목록 (네트워크로 복제됨) */
	UPROPERTY(Replicated)
	FInv_InventoryFastArray InventoryList;

	/** 인벤토리 메뉴 위젯 인스턴스 */
	UPROPERTY()
	TObjectPtr<UInv_InventoryBase> InventoryMenu;
	
	/** 인벤토리 메뉴 위젯 클래스 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_InventoryBase> InventoryMenuClass;

	/** 아이템 드롭 시 최소 회전 각도 (플레이어의 전방 벡터 기준, Y축 회전) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMin {-85.f};

	/** 아이템 드롭 시 최대 회전 각도 (플레이어의 전방 벡터 기준, Y축 회전) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMax {85.f};

	/** 아이템 드롭 시 최소 거리 (플레이어로부터) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMin {50.f};

	/** 아이템 드롭 시 최대 거리 (플레이어로부터) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMax {125.f};

	/** 아이템 드롭 시 상대적 높이 오프셋 (플레이어 위치 기준 아래로) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float RelativeSpawnElevation = {70.f};
	
	/** 인벤토리 메뉴가 열려있는지 여부 */
	bool bInventoryMenuOpen;

	

};
