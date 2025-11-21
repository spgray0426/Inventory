/**
 * 아이템 프래그먼트 시스템 구현 파일
 * 기본 프래그먼트와 특화된 프래그먼트 타입들의 구현을 포함합니다
 */

#include "Items/Fragments/Inv_ItemFragment.h"

#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"
#include "Widgets/Composite/Inv_CompositeBase.h"
#include "Widgets/Composite/Inv_Leaf_Image.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"
#include "Widgets/Composite/Inv_Leaf_Text.h"
#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"

/**
 * 이 프래그먼트의 데이터를 컴포지트 위젯에 동화시킵니다
 * 먼저 GameplayTag가 일치하는지 확인하고, 일치하면 위젯을 확장(Expand)합니다
 * 이를 통해 위젯이 이 프래그먼트의 데이터를 표시할 준비를 합니다
 */
void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
    if (!MatchesWidgetTag(Composite)) return;
    Composite->Expand();
}

/**
 * 이 프래그먼트의 GameplayTag와 위젯의 FragmentTag를 정확히 비교합니다
 * 태그가 정확히 일치해야만 true를 반환하며, 부모 태그 매칭은 허용하지 않습니다
 */
bool FInv_InventoryItemFragment::MatchesWidgetTag(const UInv_CompositeBase* Composite) const
{
    return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}

/**
 * 이미지 프래그먼트 데이터를 컴포지트 위젯에 동화시킵니다
 *
 * 위젯 태그가 프래그먼트 태그와 일치하는지 확인한 후,
 * UInv_Leaf_Image 위젯에 아이콘 텍스처와 크기 정보를 설정합니다.
 */
void FInv_ImageFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	// 부모 클래스의 Assimilate를 먼저 호출합니다
	FInv_InventoryItemFragment::Assimilate(Composite);

	// 위젯 태그가 프래그먼트 태그와 일치하는지 확인합니다
	if (!MatchesWidgetTag(Composite)) return;

	// 컴포지트를 UInv_Leaf_Image로 캐스팅합니다
	UInv_Leaf_Image* Image = Cast<UInv_Leaf_Image>(Composite);
	if (!IsValid(Image)) return;

	// 이미지 위젯에 아이콘과 크기를 설정합니다
	Image->SetImage(Icon);
	Image->SetBoxSize(IconDimensions);
	Image->SetImageSize(IconDimensions);
}

/**
 * 텍스트 프래그먼트 데이터를 컴포지트 위젯에 동화시킵니다
 *
 * 위젯 태그가 프래그먼트 태그와 일치하는지 확인한 후,
 * UInv_Leaf_Text 위젯에 텍스트 정보를 설정합니다.
 */
void FInv_TextFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	// 부모 클래스의 Assimilate를 먼저 호출합니다
	FInv_InventoryItemFragment::Assimilate(Composite);

	// 위젯 태그가 프래그먼트 태그와 일치하는지 확인합니다
	if (!MatchesWidgetTag(Composite)) return;

	// 컴포지트를 UInv_Leaf_Text로 캐스팅합니다
	UInv_Leaf_Text* LeafText = Cast<UInv_Leaf_Text>(Composite);
	if (!IsValid(LeafText)) return;

	// 텍스트 위젯에 텍스트를 설정합니다
	LeafText->SetText(FragmentText);
}

/**
 * 라벨과 숫자 데이터를 컴포지트 위젯에 동화시킵니다
 *
 * 위젯 태그가 프래그먼트 태그와 일치하는지 확인한 후,
 * UInv_Leaf_LabeledValue 위젯에 라벨과 포맷팅된 숫자 값을 설정합니다.
 */
void FInv_LabeledNumberFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	// 부모 클래스의 Assimilate를 먼저 호출합니다
	FInv_InventoryItemFragment::Assimilate(Composite);

	// 위젯 태그가 프래그먼트 태그와 일치하는지 확인합니다
	if (!MatchesWidgetTag(Composite)) return;

	// 컴포지트를 UInv_Leaf_LabeledValue로 캐스팅합니다
	UInv_Leaf_LabeledValue* LabeledValue = Cast<UInv_Leaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValue)) return;

	// 라벨 텍스트를 설정합니다 (예: "공격력:")
	LabeledValue->SetText_Label(Text_Label, bCollapseLabel);

	// 숫자 포맷 옵션을 설정합니다 (소수점 자릿수 제어)
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = MinFractionalDigits;  // 최소 소수점 자릿수
	Options.MaximumFractionalDigits = MaxFractionalDigits;  // 최대 소수점 자릿수

	// 값을 포맷팅하여 텍스트로 설정합니다 (예: "25.5")
	LabeledValue->SetText_Value(FText::AsNumber(Value, &Options), bCollapseValue);
}

/**
 * 프래그먼트 초기화 시 랜덤 값을 생성합니다
 *
 * 처음 아이템이 생성될 때만 Min~Max 범위에서 랜덤 값을 생성하고,
 * 이후에는 동일한 값을 유지합니다 (장착/해제 시에도 값 유지).
 */
void FInv_LabeledNumberFragment::Manifest()
{
	// 부모 클래스의 Manifest를 먼저 호출합니다
	FInv_InventoryItemFragment::Manifest();

	// 처음 생성 시에만 랜덤 값을 생성합니다
	if (bRandomizeOnManifest)
	{
		// Min과 Max 사이의 랜덤 float 값을 생성합니다
		Value = FMath::FRandRange(Min, Max);
	}

	// 한 번 랜덤화한 후에는 더 이상 랜덤화하지 않도록 플래그를 false로 설정합니다
	// 이를 통해 아이템을 장착했다가 떨어뜨려도 동일한 스탯 값을 유지합니다
	bRandomizeOnManifest = false;
}

/**
 * 소비형 아이템을 사용하여 모든 수정자의 효과를 적용합니다
 *
 * 이 함수는 컴포지트 패턴을 활용하여 ConsumeModifiers 배열의 모든 수정자에 대해
 * OnConsume()을 순차적으로 호출합니다. 이를 통해 하나의 아이템이 여러 효과를
 * 동시에 제공할 수 있습니다 (예: 체력 +20, 마나 +10).
 *
 * @param PC 아이템을 소비하는 플레이어 컨트롤러
 */
void FInv_ConsumableFragment::OnConsume(APlayerController* PC)
{
	// 모든 소비 수정자를 순회하며 각각의 효과를 적용합니다
	for (auto& Modifier : ConsumeModifiers)
	{
		// TInstancedStruct에서 가변 참조를 얻어 OnConsume을 호출합니다
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnConsume(PC);
	}
}

/**
 * 소비형 프래그먼트 데이터를 컴포지트 위젯에 동화시킵니다
 *
 * 부모 클래스의 Assimilate를 호출한 후, 모든 ConsumeModifier의 데이터를
 * 순회하며 각각 위젯에 동화시킵니다. 이를 통해 UI에 모든 소비 효과 정보가
 * 표시됩니다 (예: "체력 +20", "마나 +10").
 *
 * @param Composite 데이터를 동화시킬 컴포지트 위젯
 */
void FInv_ConsumableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	// 부모 클래스의 Assimilate를 먼저 호출합니다
	FInv_InventoryItemFragment::Assimilate(Composite);

	// 모든 소비 수정자를 순회하며 각각을 위젯에 동화시킵니다
	for (const auto& Modifier : ConsumeModifiers)
	{
		// TInstancedStruct에서 상수 참조를 얻어 Assimilate를 호출합니다
		const auto& ModRef = Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

/**
 * 프래그먼트 초기화 시 모든 수정자를 초기화합니다
 *
 * 부모 클래스의 Manifest를 호출한 후, 모든 ConsumeModifier에 대해
 * Manifest()를 호출하여 랜덤 값 생성 등의 초기화 작업을 수행합니다.
 * 이를 통해 각 소비 효과의 수치가 Min~Max 범위에서 자동 설정됩니다.
 */
void FInv_ConsumableFragment::Manifest()
{
	// 부모 클래스의 Manifest를 먼저 호출합니다
	FInv_InventoryItemFragment::Manifest();

	// 모든 소비 수정자를 순회하며 각각을 초기화합니다
	for (auto& Modifier : ConsumeModifiers)
	{
		// TInstancedStruct에서 가변 참조를 얻어 Manifest를 호출합니다
		auto& ModRef = Modifier.GetMutable();
		ModRef.Manifest();
	}
}

void FInv_HealthPotionFragment::OnConsume(APlayerController* PC)
{
    // 실제 구현에서는 다음 중 하나를 사용하여 체력을 회복시킵니다:
    // - PC 또는 PC->GetPawn()에서 스탯 컴포넌트를 가져와서 체력 증가
    // - Ability System Component를 가져와서 Gameplay Effect 적용
    // - Healing() 인터페이스 함수 호출

    // 현재는 디버그 메시지만 출력합니다 (테스트/프로토타입 용도)
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Health Potion consumed! Healing by: %f"), GetValue()));
}

void FInv_ManaPotionFragment::OnConsume(APlayerController* PC)
{
    // 실제 구현에서는 프로젝트의 마나 시스템에 맞게 마나를 회복시킵니다
    // 예: Ability System Component를 통한 마나 회복 Gameplay Effect 적용

    // 현재는 디버그 메시지만 출력합니다 (테스트/프로토타입 용도)
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Mana Potion consumed! Mana replenished by: %f"), GetValue()));
}

/**
 * 힘 스탯 수정자 장착 처리
 * 실제 구현에서는 플레이어의 스탯 시스템에 힘 증가 효과를 적용해야 합니다
 * 현재는 디버그 메시지만 출력합니다 (테스트/프로토타입 용도)
 */
void FInv_StrengthModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Green,
		FString::Printf(TEXT("힘 증가: %f"),
			GetValue()));
}

/**
 * 힘 스탯 수정자 해제 처리
 * 실제 구현에서는 플레이어의 스탯 시스템에서 힘 증가 효과를 제거해야 합니다
 * 현재는 디버그 메시지만 출력합니다 (테스트/프로토타입 용도)
 */
void FInv_StrengthModifier::OnUnequip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Red,
		FString::Printf(TEXT("아이템은 장착되지 않았습니다. 힘 감소: %f"),
			GetValue()));
}

/**
 * 장비를 장착하여 모든 수정자의 효과를 적용합니다
 * 이미 장착된 상태면 중복 장착을 방지하기 위해 즉시 반환합니다
 *
 * @param PC 장비를 장착하는 플레이어 컨트롤러
 */
void FInv_EquipmentFragment::OnEquip(APlayerController* PC)
{
	// 이미 장착된 상태면 중복 장착 방지
	if (bEquipped) return;
	bEquipped = true;

	// 모든 장비 수정자를 순회하며 각각의 효과를 적용합니다
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnEquip(PC);
	}
}

/**
 * 장비를 해제하여 모든 수정자의 효과를 제거합니다
 * 장착되지 않은 상태면 중복 해제를 방지하기 위해 즉시 반환합니다
 *
 * @param PC 장비를 해제하는 플레이어 컨트롤러
 */
void FInv_EquipmentFragment::OnUnequip(APlayerController* PC)
{
	// 장착되지 않은 상태면 중복 해제 방지
	if (!bEquipped) return;
	bEquipped = false;

	// 모든 장비 수정자를 순회하며 각각의 효과를 제거합니다
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnUnequip(PC);
	}
}

/**
 * 장비 프래그먼트 데이터를 컴포지트 위젯에 동화시킵니다
 * 부모 클래스의 Assimilate를 호출한 후, 모든 EquipModifier의 데이터를
 * 순회하며 각각 위젯에 동화시킵니다
 *
 * @param Composite 데이터를 동화시킬 컴포지트 위젯
 */
void FInv_EquipmentFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);

	// 모든 장비 수정자를 순회하며 각각을 위젯에 동화시킵니다
	for (const auto& Modifier : EquipModifiers)
	{
		const auto& ModRef = Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

/**
 * 프래그먼트 초기화 시 모든 수정자를 초기화합니다
 * 부모 클래스의 Manifest를 호출한 후, 모든 EquipModifier에 대해
 * Manifest()를 호출하여 랜덤 스탯 값 생성 등의 초기화 작업을 수행합니다
 */
void FInv_EquipmentFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();

	// 모든 장비 수정자를 순회하며 각각을 초기화합니다
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.Manifest();
	}
}

/**
 * 장비 액터를 스폰하고 스켈레탈 메시에 부착합니다
 * EquipActorClass를 사용하여 월드에 액터를 생성하고,
 * 지정된 소켓에 부착합니다
 *
 * @param AttachMesh 장비를 부착할 스켈레탈 메시 컴포넌트
 * @return 스폰되어 부착된 장비 액터 (실패 시 nullptr)
 */
AInv_EquipActor* FInv_EquipmentFragment::SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const
{
	// 장비 액터 클래스와 부착할 메시가 유효한지 확인
	if (!IsValid(EquipActorClass) || !IsValid(AttachMesh)) return nullptr;

	// 월드에 장비 액터 스폰
	AInv_EquipActor* SpawnedActor = AttachMesh->GetWorld()->SpawnActor<AInv_EquipActor>(EquipActorClass);

	// 지정된 소켓에 액터 부착 (스케일 제외하고 스냅)
	SpawnedActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketAttachPoint);

	return SpawnedActor;
}

/**
 * 현재 장착된 장비 액터를 파괴합니다
 * EquippedActor 참조가 유효한 경우에만 파괴를 시도합니다
 */
void FInv_EquipmentFragment::DestroyAttachedActor() const
{
	if (EquippedActor.IsValid())
	{
		EquippedActor->Destroy();
	}
}

/**
 * 장착된 장비 액터 참조를 설정합니다
 * 장비 컴포넌트에서 액터를 스폰한 후 이 함수를 호출하여 참조를 저장합니다
 *
 * @param EquipActor 장착된 장비 액터
 */
void FInv_EquipmentFragment::SetEquippedActor(AInv_EquipActor* EquipActor)
{
	EquippedActor = EquipActor;
}
