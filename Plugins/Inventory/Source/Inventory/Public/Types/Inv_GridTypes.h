#pragma once

#include "Inv_GridTypes.generated.h"

class UInv_InventoryItem;

UENUM(BlueprintType)
enum class EInv_ItemCategory : uint8
{
	Equippable,
	Consumable,
	Craftable,
	None
};

USTRUCT()
struct FInv_SlotAvailability
{
	GENERATED_BODY()
	FInv_SlotAvailability() {};
	FInv_SlotAvailability(int32 ItemIndex, int32 Room, bool bHasItem) : Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {};
	
	// 인벤토리 슬롯의 인덱스
	int32 Index{INDEX_NONE};
	
	// 해당 슬롯에 채울 수 있는 아이템의 수량
	int32 AmountToFill{0};
	
	// 해당 슬롯에 아이템이 있는지 여부
	bool bItemAtIndex{false};
};

USTRUCT()
struct FInv_SlotAvailabilityResult
{
	GENERATED_BODY()
	FInv_SlotAvailabilityResult() {};

	// 검사 중인 인벤토리 아이템에 대한 약한 참조
	TWeakObjectPtr<UInv_InventoryItem> Item;
	
	// 전체 슬롯들에 채울 수 있는 총 아이템 수량
	int32 TotalRoomToFill{0};
	
	// 남은 아이템 수량
	int32 Remainder{0};
	
	// 아이템이 중첩 가능한지 여부
	bool bStackable{false};
	
	// 사용 가능한 슬롯들의 정보 배열
	TArray<FInv_SlotAvailability> SlotAvailabilities;
};


UENUM(BlueprintType)
enum class EInv_TileQuadrant : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	None
};

USTRUCT(BlueprintType)
struct FInv_TileParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	FIntPoint TileCoordinats{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	int32 TileIndex{INDEX_NONE};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	EInv_TileQuadrant TileQuadrant{EInv_TileQuadrant::None};
};

inline bool operator==(const FInv_TileParameters& A, const FInv_TileParameters& B)
{
	return A.TileCoordinats == B.TileCoordinats && A.TileIndex == B.TileIndex && A.TileQuadrant == B.TileQuadrant;
}