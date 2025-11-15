#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ItemManifest.generated.h"

class UInv_CompositeBase;
struct FInv_ItemFragment;
class UInv_InventoryItem;

/**
 * 아이템 매니페스트 구조체
 * 새로운 인벤토리 아이템을 생성하는 데 필요한 모든 데이터를 포함합니다
 * 프래그먼트 배열, 카테고리, 아이템 타입 등의 정보를 관리합니다
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FInv_ItemManifest
{
	GENERATED_BODY()

	/**
	 * 프래그먼트 배열에 대한 수정 가능한 참조를 반환합니다
	 * 프래그먼트를 직접 수정하거나 Manifest()를 호출할 때 사용됩니다
	 * @return 프래그먼트 배열의 mutable 참조
	 */
	TArray<TInstancedStruct<FInv_ItemFragment>>& GetFragmentsMutable() { return Fragments; }

	/**
	 * 이 매니페스트를 사용하여 새로운 인벤토리 아이템 객체를 생성합니다
	 * @param NewOuter 생성될 아이템의 외부 객체 (Owner)
	 * @return 생성된 인벤토리 아이템 객체 포인터
	 */
	UInv_InventoryItem* Manifest(UObject* NewOuter);

	/**
	 * 이 아이템의 카테고리를 가져옵니다
	 * @return 아이템 카테고리 (장비, 소비품, 제작 재료 등)
	 */
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }

	/**
	 * 이 아이템의 타입을 나타내는 GameplayTag를 가져옵니다
	 * @return 아이템 타입 GameplayTag
	 */
	FGameplayTag GetItemType() const { return ItemType; }

	/**
	 * 이 매니페스트의 모든 인벤토리 아이템 프래그먼트를 컴포지트 위젯에 동화시킵니다
	 * FInv_InventoryItemFragment 타입의 모든 프래그먼트를 찾아서 각각을 위젯에 적용합니다
	 * 컴포지트 패턴을 사용하여 위젯 계층 구조를 순회하며 각 프래그먼트를 해당 위젯에 적용합니다
	 *
	 * @param Composite 프래그먼트 데이터를 동화시킬 대상 컴포지트 위젯
	 */
	void AssimilateInventoryFragments(UInv_CompositeBase* Composite) const;
	
	/**
	 * 특정 GameplayTag를 가진 프래그먼트를 타입별로 검색합니다
	 * @tparam T 검색할 프래그먼트 타입 (FInv_ItemFragment 파생 타입)
	 * @param FragmentTag 검색할 프래그먼트의 GameplayTag
	 * @return 찾은 프래그먼트의 const 포인터, 없으면 nullptr
	 */
	template<typename T> requires std::derived_from<T, FInv_ItemFragment>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const;

	/**
	 * 특정 타입의 프래그먼트를 검색합니다 (태그 검사 없이)
	 * @tparam T 검색할 프래그먼트 타입 (FInv_ItemFragment 파생 타입)
	 * @return 찾은 프래그먼트의 const 포인터, 없으면 nullptr
	 */
	template<typename T> requires std::derived_from<T, FInv_ItemFragment>
	const T* GetFragmentOfType() const;

	/**
	 * 특정 타입의 프래그먼트를 수정 가능한 형태로 검색합니다
	 * @tparam T 검색할 프래그먼트 타입 (FInv_ItemFragment 파생 타입)
	 * @return 찾은 프래그먼트의 포인터 (수정 가능), 없으면 nullptr
	 */
	template<typename T> requires std::derived_from<T, FInv_ItemFragment>
	T* GetFragmentOfTypeMutable();

	/**
	 * 특정 타입의 모든 프래그먼트를 검색하여 배열로 반환합니다
	 * 동일한 타입의 프래그먼트가 여러 개 존재할 수 있는 경우 사용합니다
	 * 예: 여러 개의 FInv_InventoryItemFragment를 가진 아이템
	 *
	 * @tparam T 검색할 프래그먼트 타입 (FInv_ItemFragment 파생 타입)
	 * @return 찾은 모든 프래그먼트의 const 포인터 배열
	 */
	template<typename T> requires std::derived_from<T, FInv_ItemFragment>
	TArray<const T*> GetAllFragmentsOfType() const;

	/**
	 * 이 매니페스트를 사용하여 픽업 액터를 월드에 스폰합니다
	 * 아이템이 드롭되거나 월드에 배치될 때 사용됩니다
	 * @param WorldContextObject 월드 컨텍스트 객체 (스폰 위치의 월드를 얻기 위함)
	 * @param SpawnLocation 픽업 액터를 스폰할 위치
	 * @param SpawnRotation 픽업 액터의 초기 회전
	 */
	void SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation);
private:

	/**
	 * 프래그먼트 배열을 비우고 메모리를 해제합니다
	 * 아이템 생성 후 매니페스트 데이터를 정리할 때 사용됩니다
	 */
	void ClearFragments();

	/** 이 아이템을 구성하는 프래그먼트들의 배열 (그리드, 이미지, 스택 등) */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ItemFragment>> Fragments;

	/** 이 아이템의 카테고리 (장비, 소비품, 제작 재료 등). 기본값은 None */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory {EInv_ItemCategory::None};

	/** 이 아이템의 고유 타입을 나타내는 GameplayTag (예: Axe, Sword, Potion 등) */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "GameItems"))
	FGameplayTag ItemType;

	/** 월드에 스폰될 픽업 액터의 클래스 (드롭되거나 배치될 때 사용) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AActor> PickupActorClass;
};

/**
 * 특정 GameplayTag를 가진 프래그먼트를 타입별로 검색하는 템플릿 함수 구현
 * Fragments 배열을 순회하며 타입과 태그가 모두 일치하는 프래그먼트를 찾습니다
 */
template<typename T> requires std::derived_from<T, FInv_ItemFragment>
const T* FInv_ItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const
{
	// 모든 프래그먼트를 순회
	for (const TInstancedStruct<FInv_ItemFragment>& Fragment : Fragments)
	{
		// 타입이 일치하는 프래그먼트 포인터 획득
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			// GameplayTag가 정확히 일치하는지 확인
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(FragmentTag)) continue;
			return FragmentPtr;
		}
	}
	return nullptr;
}

/**
 * 특정 타입의 프래그먼트를 검색하는 템플릿 함수 구현 (태그 검사 없음)
 * Fragments 배열을 순회하며 타입이 일치하는 첫 번째 프래그먼트를 반환합니다
 */
template <typename T> requires std::derived_from<T, FInv_ItemFragment>
const T* FInv_ItemManifest::GetFragmentOfType() const
{
	// 모든 프래그먼트를 순회
	for (const TInstancedStruct<FInv_ItemFragment>& Fragment : Fragments)
	{
		// 타입이 일치하는 프래그먼트를 찾으면 즉시 반환
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}

/**
 * 특정 타입의 프래그먼트를 수정 가능한 형태로 검색하는 템플릿 함수 구현
 * Fragments 배열을 순회하며 타입이 일치하는 프래그먼트의 수정 가능한 포인터를 반환합니다
 */
template <typename T> requires std::derived_from<T, FInv_ItemFragment>
T* FInv_ItemManifest::GetFragmentOfTypeMutable()
{
	// 모든 프래그먼트를 순회
	for (TInstancedStruct<FInv_ItemFragment>& Fragment : Fragments)
	{
		// 타입이 일치하는 수정 가능한 프래그먼트 포인터를 찾으면 즉시 반환
		if (T* FragmentPtr = Fragment.GetMutablePtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}

/**
 * 특정 타입의 모든 프래그먼트를 검색하는 템플릿 함수 구현
 * Fragments 배열을 순회하며 타입이 일치하는 모든 프래그먼트를 수집하여 반환합니다
 * 이 함수는 동일 타입의 프래그먼트가 여러 개 있을 때 유용합니다
 */
template <typename T> requires std::derived_from<T, FInv_ItemFragment>
TArray<const T*> FInv_ItemManifest::GetAllFragmentsOfType() const
{
	TArray<const T*> Result;
	// 모든 프래그먼트를 순회
	for (const TInstancedStruct<FInv_ItemFragment>& Fragment : Fragments)
	{
		// 타입이 일치하는 프래그먼트를 찾으면 결과 배열에 추가
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			Result.Add(FragmentPtr);
		}
	}
	return Result;
}
