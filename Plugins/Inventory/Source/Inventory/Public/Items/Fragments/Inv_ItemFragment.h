#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Inv_ItemFragment.generated.h"

class APlayerController;
/**
 * 아이템 컴포지션 시스템의 기본 프래그먼트 구조체
 * 아이템은 여러 프래그먼트로 구성되며, 각 프래그먼트는 특정 동작이나 속성을 정의합니다
 * 프래그먼트는 GameplayTag로 식별되어 타입 안전한 검색이 가능합니다
 */
USTRUCT(BlueprintType)
struct FInv_ItemFragment
{
	GENERATED_BODY()

	/** 기본 생성자 */
	FInv_ItemFragment() {};

	/** 복사 생성자 */
	FInv_ItemFragment(const FInv_ItemFragment& Other) = default;

	/** 복사 대입 연산자 */
	FInv_ItemFragment& operator=(const FInv_ItemFragment& Other) = default;

	/** 이동 생성자 */
	FInv_ItemFragment(FInv_ItemFragment&&) = default;

	/** 이동 대입 연산자 */
	FInv_ItemFragment& operator=(FInv_ItemFragment&&) = default;

	/** 다형성 정리를 위한 가상 소멸자 */
	virtual ~FInv_ItemFragment() {};

	/**
	 * 이 프래그먼트 타입을 식별하는 GameplayTag를 가져옵니다
	 * @return 프래그먼트를 식별하는 GameplayTag
	 */
	FGameplayTag GetFragmentTag() const { return FrgmentTag; }

	/**
	 * 이 프래그먼트 타입을 식별하는 GameplayTag를 설정합니다
	 * @param Tag 이 프래그먼트에 할당할 GameplayTag
	 */
	void SetFragmentTag(FGameplayTag Tag) { FrgmentTag = Tag; }

private:

	/** 이 프래그먼트 타입을 고유하게 식별하는 GameplayTag */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "FragmentTags"))
	FGameplayTag FrgmentTag = FGameplayTag::EmptyTag;
};


/**
 * 인벤토리 아이템 프래그먼트
 * 컴포지트 위젯 시스템과 상호작용하여 UI 요소를 확장(Expand)하는 프래그먼트입니다
 * GameplayTag를 사용하여 특정 위젯 타입과 매칭되며, 일치하는 위젯에 데이터를 동화(Assimilate)시킵니다
 *
 * 이 프래그먼트는 아이템 매니페스트의 데이터를 UI 위젯에 반영하는 데 사용됩니다
 * 예: 아이템 설명 위젯에 아이템 이름, 설명 등을 표시
 */
class UInv_CompositeBase;
USTRUCT(BlueprintType)
struct FInv_InventoryItemFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	/**
	 * 이 프래그먼트의 데이터를 컴포지트 위젯에 동화(Assimilate)시킵니다
	 * GameplayTag가 일치하는 경우에만 위젯을 확장(Expand)합니다
	 *
	 * @param Composite 데이터를 동화시킬 대상 컴포지트 위젯
	 */
	virtual void Assimilate(UInv_CompositeBase* Composite) const;

protected:
	/**
	 * 이 프래그먼트의 GameplayTag가 위젯의 Tag와 정확히 일치하는지 확인합니다
	 *
	 * @param Composite 태그를 비교할 대상 위젯
	 * @return 태그가 정확히 일치하면 true, 아니면 false
	 */
	bool MatchesWidgetTag(const UInv_CompositeBase* Composite) const;
};


/**
 * 그리드 기반 인벤토리의 공간 차원을 정의하는 그리드 프래그먼트
 * 아이템이 차지하는 그리드 셀의 개수(너비 x 높이)와 패딩을 지정합니다
 */
USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	/**
	 * 이 아이템의 그리드 크기를 가져옵니다
	 * @return 그리드 셀 단위의 너비(X)와 높이(Y)를 담은 FIntPoint
	 */
	FIntPoint GetGridSize() const { return GridSize; }

	/**
	 * 이 아이템의 그리드 크기를 설정합니다
	 * @param Size 그리드 크기 (X = 너비, Y = 높이, 셀 단위)
	 */
	void SetGridSize(const FIntPoint& Size) { GridSize = Size; }

	/**
	 * 그리드에서 아이템 주변의 패딩을 가져옵니다
	 * @return 픽셀 단위의 패딩 값
	 */
	float GetGridPadding() const { return GridPadding; }

	/**
	 * 그리드에서 아이템 주변의 패딩을 설정합니다
	 * @param Padding 픽셀 단위의 패딩 값
	 */
	void SetGridPadding(float Padding) { GridPadding = Padding; }

private:

	/** 그리드 셀 단위의 아이템 크기 (X = 너비, Y = 높이). 기본값은 1x1 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize {1, 1};

	/** 픽셀 단위의 아이템 주변 패딩. 기본값은 0 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding {0.f};

};

/**
 * 아이템의 시각적 표현을 정의하는 이미지 프래그먼트
 * 아이콘 텍스처와 표시 크기를 포함합니다
 */
USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	/**
	 * 이 아이템의 아이콘 텍스처를 가져옵니다
	 * @return 아이템 아이콘으로 사용되는 2D 텍스처 포인터
	 */
	UTexture2D* GetIcon() const { return Icon; }

private:

	/** 인벤토리 UI에서 아이템 아이콘으로 사용되는 2D 텍스처 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon {nullptr};

	/** 픽셀 단위의 아이콘 표시 크기. 기본값은 44x44 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions {44.f, 44.f};
};

/**
 * 아이템의 스택 기능을 활성화하는 스택 가능 프래그먼트
 * 최대 스택 크기를 정의하고 현재 스택 개수를 추적합니다
 */
USTRUCT(BlueprintType)
struct FInv_StackableFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	/**
	 * 함께 스택할 수 있는 아이템의 최대 개수를 가져옵니다
	 * @return 최대 스택 크기
	 */
	int32 GetMaxStackSize() const { return MaxStackSize; }

	/**
	 * 이 스택의 현재 아이템 개수를 가져옵니다
	 * @return 현재 스택 개수
	 */
	int32 GetStackCount() const { return StackCount; }

	/**
	 * 이 스택의 현재 아이템 개수를 설정합니다
	 * @param Count 새로운 스택 개수 (MaxStackSize를 초과하지 않아야 함)
	 */
	void SetStackCount(int32 Count) { StackCount = Count; }

private:

	/** 함께 스택할 수 있는 아이템의 최대 개수. 기본값은 1 (스택 불가) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize {1};

	/** 이 스택의 현재 아이템 개수. 기본값은 1 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount{1};
};

/**
 * 소비 가능 아이템의 기본 프래그먼트
 * 아이템이 소비될 때의 동작을 정의합니다
 * 파생 구조체에서 OnConsume을 오버라이드하여 구체적인 소비 효과를 구현합니다
 */
USTRUCT(BlueprintType)
struct FInv_ConsumableFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	/**
	 * 아이템이 소비될 때 호출되는 가상 함수
	 * 파생 구조체에서 오버라이드하여 체력 회복, 마나 회복 등의 효과를 구현합니다
	 * @param PC 아이템을 소비하는 플레이어 컨트롤러
	 */
	virtual void OnConsume(APlayerController* PC) {}
};

/**
 * 체력 회복 포션 프래그먼트
 * 소비 시 플레이어의 체력을 회복시키는 기능을 제공합니다
 */
USTRUCT(BlueprintType)
struct FInv_HealthPotionFragment : public FInv_ConsumableFragment
{
	GENERATED_BODY()

	/** 이 포션이 회복시키는 체력 수치. 기본값은 20 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float HealAmount = 20.f;

	/**
	 * 체력 포션을 소비합니다
	 * 플레이어의 체력을 HealAmount만큼 회복시킵니다
	 * @param PC 아이템을 소비하는 플레이어 컨트롤러
	 */
	virtual void OnConsume(APlayerController* PC) override;
};

/**
 * 마나 회복 포션 프래그먼트
 * 소비 시 플레이어의 마나를 회복시키는 기능을 제공합니다
 */
USTRUCT(BlueprintType)
struct FInv_ManaPotionFragment : public FInv_ConsumableFragment
{
	GENERATED_BODY()

	/** 이 포션이 회복시키는 마나 수치. 기본값은 20 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float ManaAmount = 20.f;

	/**
	 * 마나 포션을 소비합니다
	 * 플레이어의 마나를 ManaAmount만큼 회복시킵니다
	 * @param PC 아이템을 소비하는 플레이어 컨트롤러
	 */
	virtual void OnConsume(APlayerController* PC) override;
};