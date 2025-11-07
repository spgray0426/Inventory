// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Inv_InventoryItem.generated.h"

/**
 * `UInv_InventoryItem` 클래스는 인벤토리 아이템을 나타내는 데 사용됩니다.
 * 이 클래스는 아이템의 데이터와 관련된 속성 및 동작을 정의합니다.
 */
UCLASS()
class INVENTORY_API UInv_InventoryItem : public UObject
{
	GENERATED_BODY()
	/**
	 * 아이템의 복제 속성을 설정하기 위한 함수입니다. 네트워크 동기화를 위한 속성들을 설정합니다.
	 *
	 * @param OutLifetimeProps 복제할 속성을 저장하는 TArray. 복제를 위한 속성 정보를 추가하는 데 사용됩니다.
	 */
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/**
	 * 네트워킹을 지원하는지 여부를 확인합니다.
	 *
	 * @return 네트워킹이 지원된다면 true를 반환하고, 그렇지 않다면 false를 반환합니다.
	 */
	virtual bool IsSupportedForNetworking() const override { return true; }

	/**
	 * 주어진 FInv_ItemManifest 객체를 통해 인벤토리 아이템의 ItemManifest를 설정합니다.
	 *
	 * @param Manifest 설정할 FInv_ItemManifest 객체
	 */
	void SetItemManifest(const FInv_ItemManifest& Manifest);
	/**
	 * 아이템의 매니페스트(ItemManifest)를 반환합니다.
	 *
	 * @return 아이템의 FInv_ItemManifest 참조. 이 매니페스트는 아이템의 주요 정보와 관련된 데이터를 포함합니다.
	 */
	const FInv_ItemManifest& GetItemManifest() const { return ItemManifest.Get<FInv_ItemManifest>(); }
	/**
	 * @brief FInv_ItemManifest 객체의 수정 가능한 참조를 반환합니다.
	 *
	 * 이 메서드는 ItemManifest의 내용을 수정할 수 있도록 FInv_ItemManifest 타입의 참조를 반환합니다.
	 *
	 * @return FInv_ItemManifest& 수정 가능한 FInv_ItemManifest 객체의 참조를 반환합니다.
	 */
	FInv_ItemManifest& GetItemManifestMutable() { return ItemManifest.GetMutable<FInv_ItemManifest>(); }
	/**
	 * 이 함수는 현재 인벤토리 아이템이 스택 가능 여부를 확인합니다.
	 *
	 * @return 아이템이 스택 가능한 경우 true를 반환하며, 그렇지 않은 경우 false를 반환합니다.
	 */
	bool IsStackable() const;

	bool IsConsumable() const;
	/**
	 * 총 스택 수를 반환합니다.
	 *
	 * @return 아이템의 현재 총 스택 수를 반환합니다.
	 */
	int32 GetTotalStackCount() const { return TotalStackCount; }
	/**
	 * 총 스택 개수를 설정합니다.
	 *
	 * @param Count 설정할 총 스택 개수입니다.
	 */
	void SetTotalStackCount(int32 Count) { TotalStackCount = Count; }
private:
	/**
	 * @brief 아이템의 상세 데이터를 포함하는 구조체입니다.
	 *
	 * ItemManifest 변수는 FInv_ItemManifest 구조체의 인스턴스를 나타내며,
	 * 아이템의 카테고리, 유형, 그리고 태그로 필터링된 프래그먼트 정보를 제공합니다.
	 * 이 변수는 네트워크 동기화를 위해 복제되며, 직렬화된 메모리 구조를 가진 FInstancedStruct를 기반으로 구현됩니다.
	 *
	 * @details
	 * - 사용자는 Setter 및 Getter 메서드를 통해 이 변수를 수정하거나 액세스할 수 있습니다.
	 * - 네트워크 복제를 지원하며, UInv_InventoryItem 클래스 내에서 DOREPLIFETIME 매크로를 통해 동기화됩니다.
	 * - BaseStruct로 지정된 "Inv_ItemManifest" 구조체를 참조합니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (BaseStruct = "/Script/Inventory.Inv_ItemManifest"), Replicated)
	FInstancedStruct ItemManifest;

	/**
	 * TotalStackCount는 이 인벤토리 아이템에 대한 현재 전체 스택 개수를 나타냅니다.
	 *
	 * - 네트워크 동기화를 위해 Replicated 속성이 적용되었습니다.
	 * - 이 값은 GetTotalStackCount()를 통해 접근하거나 SetTotalStackCount()를 이용해 설정할 수 있습니다.
	 * - 인벤토리 시스템 내에서 스택 가능한 아이템의 누적 개수를 관리하는 데 사용됩니다.
	 */
	UPROPERTY(Replicated)
	int32 TotalStackCount {0};
};

template<typename FragmentType>
const FragmentType* GetFragment(const UInv_InventoryItem* Item, const FGameplayTag& Tag)
{
	if (!IsValid(Item)) return nullptr;

	const FInv_ItemManifest& Manifest = Item->GetItemManifest();

	return Manifest.GetFragmentOfTypeWithTag<FragmentType>(Tag);
}