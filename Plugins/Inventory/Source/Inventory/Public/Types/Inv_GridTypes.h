#pragma once

#include "Inv_GridTypes.generated.h"

class UInv_InventoryItem;

/**
 * 아이템 카테고리를 정의하는 열거형
 * 인벤토리 UI에서 탭별로 아이템을 분류하는 데 사용됩니다
 */
UENUM(BlueprintType)
enum class EInv_ItemCategory : uint8
{
	/** 장착 가능한 장비 아이템 (무기, 방어구 등) */
	Equippable,

	/** 소비 가능한 아이템 (포션 등) */
	Consumable,

	/** 제작 재료 아이템 */
	Craftable,

	/** 카테고리 없음 */
	None
};

/**
 * 단일 슬롯의 사용 가능 여부 정보를 나타내는 구조체
 * 스택 가능한 아이템의 슬롯 공간 계산에 사용됩니다
 */
USTRUCT()
struct FInv_SlotAvailability
{
	GENERATED_BODY()

	/** 기본 생성자 */
	FInv_SlotAvailability() {};

	/**
	 * 파라미터 생성자
	 * @param ItemIndex 슬롯의 인덱스
	 * @param Room 채울 수 있는 아이템 수량
	 * @param bHasItem 슬롯에 이미 아이템이 있는지 여부
	 */
	FInv_SlotAvailability(int32 ItemIndex, int32 Room, bool bHasItem) : Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {};

	/** 인벤토리 슬롯의 인덱스 */
	int32 Index{INDEX_NONE};

	/** 해당 슬롯에 채울 수 있는 아이템의 수량 */
	int32 AmountToFill{0};

	/** 해당 슬롯에 아이템이 있는지 여부 */
	bool bItemAtIndex{false};
};

/**
 * 슬롯 공간 확인 결과를 담는 구조체
 * 스택 가능 및 불가능한 아이템 모두를 처리하며, 전체 공간 정보를 제공합니다
 */
USTRUCT()
struct FInv_SlotAvailabilityResult
{
	GENERATED_BODY()

	/** 기본 생성자 */
	FInv_SlotAvailabilityResult() {};

	/** 검사 중인 인벤토리 아이템에 대한 약한 참조 */
	TWeakObjectPtr<UInv_InventoryItem> Item;

	/** 전체 슬롯들에 채울 수 있는 총 아이템 수량 */
	int32 TotalRoomToFill{0};

	/** 남은 아이템 수량 (인벤토리에 넣을 수 없는 수량) */
	int32 Remainder{0};

	/** 아이템이 중첩 가능한지 여부 */
	bool bStackable{false};

	/** 사용 가능한 슬롯들의 정보 배열 */
	TArray<FInv_SlotAvailability> SlotAvailabilities;
};


/**
 * 타일의 사분면 위치를 나타내는 열거형
 * 그리드 타일 내에서 마우스 위치나 아이템 배치 위치를 결정하는 데 사용됩니다
 */
UENUM(BlueprintType)
enum class EInv_TileQuadrant : uint8
{
	/** 왼쪽 위 사분면 */
	TopLeft,

	/** 오른쪽 위 사분면 */
	TopRight,

	/** 왼쪽 아래 사분면 */
	BottomLeft,

	/** 오른쪽 아래 사분면 */
	BottomRight,

	/** 사분면 없음 */
	None
};

/**
 * 타일 파라미터 구조체
 * 그리드 타일의 좌표, 인덱스, 사분면 정보를 포함합니다
 */
USTRUCT(BlueprintType)
struct FInv_TileParameters
{
	GENERATED_BODY()

	/** 타일의 2D 좌표 (X, Y) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	FIntPoint TileCoordinats{};

	/** 1차원 배열에서의 타일 인덱스 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	int32 TileIndex{INDEX_NONE};

	/** 타일 내 사분면 위치 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	EInv_TileQuadrant TileQuadrant{EInv_TileQuadrant::None};
};

/**
 * FInv_TileParameters 비교 연산자
 * 모든 멤버 변수가 동일한지 확인합니다
 */
inline bool operator==(const FInv_TileParameters& A, const FInv_TileParameters& B)
{
	return A.TileCoordinats == B.TileCoordinats && A.TileIndex == B.TileIndex && A.TileQuadrant == B.TileQuadrant;
}

/**
 * 공간 쿼리 결과를 담는 구조체
 * 특정 그리드 공간이 비어있는지, 기존 아이템과 교환 가능한지 등의 정보를 제공합니다
 */
USTRUCT()
struct FInv_SpaceQueryResult
{
	GENERATED_BODY()

	/** 쿼리된 공간에 빈 자리가 있는 경우 true */
	bool bHasSpace{false};

	/** 교환할 수 있는 단일 아이템이 있는 경우 유효한 아이템 참조 */
	TWeakObjectPtr<UInv_InventoryItem> ValidItem = nullptr;

	/** 유효한 아이템의 왼쪽 위 인덱스 (아이템이 있는 경우) */
	int32 UpperLeftIndex{INDEX_NONE};
};