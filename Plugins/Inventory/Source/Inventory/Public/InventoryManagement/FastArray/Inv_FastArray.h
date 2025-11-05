#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inv_FastArray.generated.h"

struct FGameplayTag;
class UInv_ItemComponent;
class UInv_InventoryComponent;
class UInv_InventoryItem;

/**
 * 인벤토리의 단일 항목을 나타내는 Fast Array 엔트리
 * Fast Array Serialization을 통해 효율적인 네트워크 복제를 제공합니다
 * 델타 직렬화를 지원하여 변경된 항목만 네트워크를 통해 전송됩니다
 */
USTRUCT(BlueprintType)
struct FInv_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	/** 기본 생성자 */
	FInv_InventoryEntry() {};

private:
	friend struct FInv_InventoryFastArray;
	friend UInv_InventoryComponent;

	/** 이 엔트리가 나타내는 인벤토리 아이템 */
	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item;
};

/**
 * 인벤토리 항목을 저장하는 Fast Array 구조체
 * 멀티플레이어 환경에서 효율적인 인벤토리 복제를 위한 핵심 시스템입니다
 * 델타 직렬화를 사용하여 변경된 항목만 클라이언트에 전송하므로 네트워크 대역폭을 절약합니다
 */
USTRUCT(BlueprintType)
struct FInv_InventoryFastArray : public FFastArraySerializer
{

	GENERATED_BODY()

public:
	/** 기본 생성자 */
	FInv_InventoryFastArray() : OwnerComponent(nullptr){};

	/**
	 * 파라미터 생성자
	 * @param InOwnerComponent 이 Fast Array를 소유하는 액터 컴포넌트
	 */
	FInv_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent){};

	/**
	 * 인벤토리에 있는 모든 아이템을 가져옵니다
	 * @return 모든 인벤토리 아이템의 배열
	 */
	TArray<UInv_InventoryItem*> GetAllItems() const;

	/**
	 * Fast Array에서 항목이 제거되기 전에 호출됩니다
	 * @param RemovedIndices 제거될 항목들의 인덱스
	 * @param FinalSize 제거 후의 최종 배열 크기
	 */
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);

	/**
	 * Fast Array에 항목이 추가된 후 호출됩니다
	 * @param AddedIndices 추가된 항목들의 인덱스
	 * @param FinalSize 추가 후의 최종 배열 크기
	 */
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);

	/**
	 * 네트워크 델타 직렬화 함수
	 * 변경된 항목만 직렬화하여 네트워크 효율성을 높입니다
	 * @param DeltaParams 델타 직렬화 정보
	 * @return 직렬화 성공 여부
	 */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInv_InventoryEntry, FInv_InventoryFastArray>(Entries, DeltaParams, *this);
	}

	/**
	 * 아이템 컴포넌트를 사용하여 새 엔트리를 추가합니다
	 * @param InItemComponent 추가할 아이템 컴포넌트
	 * @return 생성된 인벤토리 아이템
	 */
	UInv_InventoryItem* AddEntry(UInv_ItemComponent* InItemComponent);

	/**
	 * 인벤토리 아이템을 사용하여 새 엔트리를 추가합니다
	 * @param InItem 추가할 인벤토리 아이템
	 * @return 추가된 인벤토리 아이템
	 */
	UInv_InventoryItem* AddEntry(UInv_InventoryItem* InItem);

	/**
	 * 인벤토리에서 아이템을 제거합니다
	 * @param InItem 제거할 인벤토리 아이템
	 */
	void RemoveEntry(UInv_InventoryItem* InItem);

	/**
	 * 특정 타입의 첫 번째 아이템을 찾습니다
	 * @param ItemType 찾을 아이템의 GameplayTag 타입
	 * @return 찾은 아이템, 없으면 nullptr
	 */
	UInv_InventoryItem* FindFirstItemByType(const FGameplayTag& ItemType);

private:
	friend UInv_InventoryComponent;

	/** 복제된 항목 목록, NetDeltaSerialize에서 자동으로 복제됩니다 */
	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;

	/** 이 Fast Array를 소유하는 컴포넌트 (복제되지 않음) */
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 * FInv_InventoryFastArray에 대한 구조체 특성 정의
 * 네트워크 델타 직렬화 지원을 활성화합니다
 */
template<>
struct TStructOpsTypeTraits<FInv_InventoryFastArray> : public TStructOpsTypeTraitsBase2<FInv_InventoryFastArray>
{
	/** 이 구조체가 네트워크 델타 직렬화를 지원함을 나타냅니다 */
	enum { WithNetDeltaSerializer = true };
};