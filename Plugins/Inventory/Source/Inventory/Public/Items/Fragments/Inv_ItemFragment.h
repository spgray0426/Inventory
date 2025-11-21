#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"

#include "Inv_ItemFragment.generated.h"

class AInv_EquipActor;
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

	/**
	 * 프래그먼트가 처음 생성될 때 호출되는 초기화 메서드입니다
	 * 랜덤 값 생성, 초기 상태 설정 등의 용도로 사용됩니다
	 * 파생 클래스에서 오버라이드하여 자체 초기화 로직을 구현할 수 있습니다
	 */
	virtual void Manifest() {}
	
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
struct FInv_ImageFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	/**
	 * 이 아이템의 아이콘 텍스처를 가져옵니다
	 * @return 아이템 아이콘으로 사용되는 2D 텍스처 포인터
	 */
	UTexture2D* GetIcon() const { return Icon; }

	/**
	 * 이미지 프래그먼트 데이터를 컴포지트 위젯에 동화시킵니다
	 * UInv_Leaf_Image 위젯에 아이콘과 크기 정보를 설정합니다
	 *
	 * @param Composite 데이터를 동화시킬 컴포지트 위젯
	 */
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

private:

	/** 인벤토리 UI에서 아이템 아이콘으로 사용되는 2D 텍스처 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon {nullptr};

	/** 픽셀 단위의 아이콘 표시 크기. 기본값은 44x44 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions {88.f, 88.f};
};


/**
 * 아이템의 텍스트 정보를 담는 텍스트 프래그먼트
 * 아이템 이름, 설명 등의 텍스트 데이터를 저장 고 UI에 표시합니다
 */
USTRUCT(BlueprintType)
struct FInv_TextFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	/**
	 * 프래그먼트에 저장된 텍스트를 가져옵니다
	 * @return 프래그먼트의 텍스트 데이터
	 */
	FText GetText() const { return FragmentText; }

	/**
	 * 프래그먼트의 텍스트를 설정합니다
	 * @param Text 설정할 텍스트 데이터
	 */
	void SetText(const FText& Text) { FragmentText = Text; }

	/**
	 * 텍스트 프래그먼트 데이터를 컴포지트 위젯에 동화시킵니다
	 * UInv_Leaf_Text 위젯에 텍스트 정보를 설정합니다
	 *
	 * @param Composite 데이터를 동화시킬 컴포지트 위젯
	 */
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

private:
	/** 프래그먼트가 저장하는 텍스트 데이터 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText;
};

/**
 * 라벨과 숫자 값을 함께 표시하는 프래그먼트
 * 아이템의 스탯 정보(공격력, 방어력 등)를 표시하는 데 사용됩니다
 *
 * 예시: "공격력: 25.5", "방어력: 10.0", "내구도: 75.3"
 *
 * 주요 기능:
 * - Min~Max 범위 내에서 랜덤 값 생성
 * - 한 번 생성된 값은 유지 (장착/해제 시에도 동일한 값 유지)
 * - 소수점 자릿수 제어
 * - 라벨/값 개별 표시 제어
 */
USTRUCT(BlueprintType)
struct FInv_LabeledNumberFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	/**
	 * 라벨과 숫자 데이터를 컴포지트 위젯에 동화시킵니다
	 * UInv_Leaf_LabeledValue 위젯에 라벨과 포맷팅된 숫자 값을 설정합니다
	 *
	 * @param Composite 데이터를 동화시킬 컴포지트 위젯
	 */
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	/**
	 * 프래그먼트 초기화 시 랜덤 값을 생성합니다
	 * bRandomizeOnManifest가 true일 때 Min~Max 범위에서 Value를 랜덤 설정합니다
	 * 한 번 실행 후 bRandomizeOnManifest를 false로 설정하여 값을 고정합니다
	 */
	virtual void Manifest() override;

	/**
	 * 프래그먼트가 저장하고 있는 현재 숫자 값을 가져옵니다
	 * @return 프래그먼트의 현재 값 (Manifest() 호출 후 Min~Max 범위에서 설정된 값)
	 */
	float GetValue() const { return Value; }
	
	/**
	 * 프래그먼트 초기화 시 랜덤화 여부
	 * true: 처음 생성 시 랜덤 값 생성
	 * false: 랜덤화하지 않고 기존 값 유지 (아이템 장착/해제 후에도 동일한 값 유지)
	 */
	bool bRandomizeOnManifest{true};

private:
	/** 표시할 라벨 텍스트 (예: "공격력:", "방어력:") */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText Text_Label{};

	/** 현재 값 (Manifest() 호출 시 Min~Max 범위에서 자동 설정됨) */
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{0.f};

	/** 랜덤 값 생성 시 최소값 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Min{0};

	/** 랜덤 값 생성 시 최대값 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Max{0};

	/** true일 경우 라벨을 숨깁니다 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel{false};

	/** true일 경우 값을 숨깁니다 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue{false};

	/** 표시할 최소 소수점 자릿수 (예: 1이면 25.0으로 표시) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{1};

	/** 표시할 최대 소수점 자릿수 (예: 1이면 25.5까지 표시) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{1};
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
 * 소비형 아이템의 개별 효과를 정의하는 수정자 프래그먼트
 * FInv_LabeledNumberFragment를 상속받아 라벨과 수치값을 가진 효과를 표현합니다
 *
 * 이 구조체는 컴포지트 패턴의 일부로, 하나의 소비형 아이템이 여러 효과를
 * 가질 수 있도록 합니다 (예: 체력 +20, 마나 +10을 동시에 제공하는 포션)
 */
USTRUCT(BlueprintType)
struct FInv_ConsumeModifier : public FInv_LabeledNumberFragment
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
 * 소비 가능한 아이템을 정의하는 프래그먼트
 * 여러 개의 FInv_ConsumeModifier를 포함할 수 있어 하나의 아이템이 복합적인 효과를
 * 제공할 수 있습니다 (예: 체력과 마나를 동시에 회복하는 포션)
 *
 * 컴포지트 패턴을 사용하여 다양한 소비 효과를 조합할 수 있습니다:
 * - 각 ConsumeModifier는 독립적인 효과를 정의
 * - OnConsume() 호출 시 모든 수정자의 효과가 순차적으로 적용됨
 * - UI 동화(Assimilate) 시 모든 수정자 정보가 위젯에 반영됨
 */
USTRUCT(BlueprintType)
struct FInv_ConsumableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	/**
	 * 아이템을 소비하여 모든 수정자의 효과를 적용합니다
	 * 배열의 각 ConsumeModifier에 대해 OnConsume()을 호출합니다
	 * @param PC 아이템을 소비하는 플레이어 컨트롤러
	 */
	virtual void OnConsume(APlayerController* PC);

	/**
	 * 소비형 프래그먼트 데이터를 컴포지트 위젯에 동화시킵니다
	 * 모든 ConsumeModifier의 데이터를 순회하며 각각 위젯에 동화시킵니다
	 * @param Composite 데이터를 동화시킬 컴포지트 위젯
	 */
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	/**
	 * 프래그먼트 초기화 시 모든 수정자를 초기화합니다
	 * 각 ConsumeModifier의 Manifest()를 호출하여 랜덤 값 등을 생성합니다
	 */
	virtual void Manifest() override;

private:
	/**
	 * 이 소비형 아이템이 제공하는 효과 수정자들의 배열
	 * 각 수정자는 독립적인 효과를 정의합니다 (체력 회복, 마나 회복 등)
	 * meta = (ExcludeBaseStruct): 에디터에서 기본 구조체를 제외하고 파생 타입만 선택 가능
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ConsumeModifier>> ConsumeModifiers;
};

/**
 * 체력 회복 포션 프래그먼트
 * 소비 시 플레이어의 체력을 회복시키는 기능을 제공합니다
 */
USTRUCT(BlueprintType)
struct FInv_HealthPotionFragment : public FInv_ConsumeModifier
{
	GENERATED_BODY()

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
struct FInv_ManaPotionFragment : public FInv_ConsumeModifier
{
	GENERATED_BODY()

	/**
	 * 마나 포션을 소비합니다
	 * 플레이어의 마나를 ManaAmount만큼 회복시킵니다
	 * @param PC 아이템을 소비하는 플레이어 컨트롤러
	 */
	virtual void OnConsume(APlayerController* PC) override;
};

// ============================================================================
// 장비 시스템 (Equipment System)
// ============================================================================

/**
 * 장비 아이템의 개별 효과를 정의하는 수정자 프래그먼트
 * FInv_LabeledNumberFragment를 상속받아 라벨과 수치값을 가진 스탯 효과를 표현합니다
 *
 * 하나의 장비 아이템이 여러 스탯 효과를 가질 수 있도록 합니다
 * 예: 힘 +5, 민첩 +3을 동시에 제공하는 갑옷
 */
USTRUCT(BlueprintType)
struct FInv_EquipModifier : public FInv_LabeledNumberFragment
{
	GENERATED_BODY()

	/**
	 * 장비를 장착했을 때 호출되는 가상 함수
	 * 파생 구조체에서 오버라이드하여 스탯 증가 등의 효과를 구현합니다
	 * @param PC 장비를 장착하는 플레이어 컨트롤러
	 */
	virtual void OnEquip(APlayerController* PC) {}

	/**
	 * 장비를 해제했을 때 호출되는 가상 함수
	 * 파생 구조체에서 오버라이드하여 스탯 감소 등의 효과를 구현합니다
	 * @param PC 장비를 해제하는 플레이어 컨트롤러
	 */
	virtual void OnUnequip(APlayerController* PC) {}
};

/**
 * 힘(Strength) 스탯을 증가시키는 장비 수정자
 * 장착 시 힘을 증가시키고, 해제 시 감소시킵니다
 */
USTRUCT(BlueprintType)
struct FInv_StrengthModifier : public FInv_EquipModifier
{
	GENERATED_BODY()

	/**
	 * 장착 시 힘 스탯 증가
	 * @param PC 장비를 장착하는 플레이어 컨트롤러
	 */
	virtual void OnEquip(APlayerController* PC) override;

	/**
	 * 해제 시 힘 스탯 감소
	 * @param PC 장비를 해제하는 플레이어 컨트롤러
	 */
	virtual void OnUnequip(APlayerController* PC) override;
};


/**
 * 장착 가능한 장비 아이템을 정의하는 프래그먼트
 * 여러 개의 FInv_EquipModifier를 포함할 수 있어 하나의 장비가 복합적인 스탯 효과를
 * 제공할 수 있습니다 (예: 힘과 민첩을 동시에 증가시키는 갑옷)
 *
 * 장비 액터를 스폰하여 캐릭터의 스켈레탈 메시에 부착하고,
 * 장착/해제 시 스탯 효과를 적용/제거합니다
 */
USTRUCT(BlueprintType)
struct FInv_EquipmentFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	/**
	 * 장비를 장착하여 모든 수정자의 효과를 적용합니다
	 * 배열의 각 EquipModifier에 대해 OnEquip()을 호출합니다
	 * @param PC 장비를 장착하는 플레이어 컨트롤러
	 */
	void OnEquip(APlayerController* PC);

	/**
	 * 장비를 해제하여 모든 수정자의 효과를 제거합니다
	 * 배열의 각 EquipModifier에 대해 OnUnequip()을 호출합니다
	 * @param PC 장비를 해제하는 플레이어 컨트롤러
	 */
	void OnUnequip(APlayerController* PC);

	/**
	 * 장비 프래그먼트 데이터를 컴포지트 위젯에 동화시킵니다
	 * 모든 EquipModifier의 데이터를 순회하며 각각 위젯에 동화시킵니다
	 * @param Composite 데이터를 동화시킬 컴포지트 위젯
	 */
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	/**
	 * 프래그먼트 초기화 시 모든 수정자를 초기화합니다
	 * 각 EquipModifier의 Manifest()를 호출하여 랜덤 스탯 값 등을 생성합니다
	 */
	virtual void Manifest() override;

	/**
	 * 장비 액터를 스폰하고 지정된 소켓에 부착합니다
	 * @param AttachMesh 장비를 부착할 스켈레탈 메시 컴포넌트
	 * @return 스폰된 장비 액터
	 */
	AInv_EquipActor* SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const;

	/**
	 * 부착된 장비 액터를 파괴합니다
	 */
	void DestroyAttachedActor() const;

	/**
	 * 장비 타입 태그를 가져옵니다 (예: Inventory.Equipment.Type.Cloak)
	 * @return 장비 타입을 나타내는 GameplayTag
	 */
	FGameplayTag GetEquipmentType() const { return EquipmentType; }

	/**
	 * 장착된 액터 참조를 설정합니다
	 * @param EquipActor 장착된 장비 액터
	 */
	void SetEquippedActor(AInv_EquipActor* EquipActor);

	/** 현재 장착 상태 여부 */
	bool bEquipped{false};

private:
	/**
	 * 이 장비가 제공하는 스탯 수정자들의 배열
	 * 각 수정자는 독립적인 스탯 효과를 정의합니다 (힘, 민첩, 지능 등)
	 * meta = (ExcludeBaseStruct): 에디터에서 기본 구조체를 제외하고 파생 타입만 선택 가능
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_EquipModifier>> EquipModifiers;

	/** 장착 시 스폰할 장비 액터 클래스 (예: 망토, 검, 방패 등의 비주얼 표현) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AInv_EquipActor> EquipActorClass {nullptr};

	/** 현재 장착된 장비 액터 참조 (약한 참조) */
	TWeakObjectPtr<AInv_EquipActor> EquippedActor {nullptr};

	/** 장비를 부착할 스켈레탈 메시의 소켓 이름 (예: "BackSocket", "RightHandSocket") */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName SocketAttachPoint {NAME_None};

	/** 장비 타입을 식별하는 GameplayTag (같은 타입의 장비는 동시에 하나만 장착 가능) */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EquipmentType {FGameplayTag::EmptyTag};
};