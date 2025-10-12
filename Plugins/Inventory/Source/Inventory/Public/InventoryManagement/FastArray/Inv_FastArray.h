#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inv_FastArray.generated.h"

class UInv_ItemComponent;
class UInv_InventoryComponent;
class UInv_InventoryItem;

/** 인벤토리의 단일 항목 */
USTRUCT(BlueprintType)
struct FInv_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	FInv_InventoryEntry() {};

private:
	friend struct FInv_InventoryFastArray;
	friend UInv_InventoryComponent;

	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item;
};

/** 인벤토리의 항목을 저장하는 배열 */
USTRUCT(BlueprintType)
struct FInv_InventoryFastArray : public FFastArraySerializer
{
	
	GENERATED_BODY()

public:
	FInv_InventoryFastArray() : OwnerComponent(nullptr){};
	FInv_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent){};

	TArray<UInv_InventoryItem*> GetAllItems() const;

	// FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	// End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInv_InventoryEntry, FInv_InventoryFastArray>(Entries, DeltaParams, *this);
	}

	UInv_InventoryItem* AddEntry(UInv_ItemComponent* InItemComponent);
	UInv_InventoryItem* AddEntry(UInv_InventoryItem* InItem);
	void RemoveEntry(UInv_InventoryItem* InItem);
	
private:
	friend UInv_InventoryComponent;

	// 복제된 항목 목록으로 NetDeltaSerialize에서 복제함
	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FInv_InventoryFastArray> : public TStructOpsTypeTraitsBase2<FInv_InventoryFastArray>
{
	enum { WithNetDeltaSerializer = true };
};