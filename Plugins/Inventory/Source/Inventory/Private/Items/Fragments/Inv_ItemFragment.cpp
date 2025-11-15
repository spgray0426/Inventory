/**
 * 아이템 프래그먼트 시스템 구현 파일
 * 기본 프래그먼트와 특화된 프래그먼트 타입들의 구현을 포함합니다
 */

#include "Items/Fragments/Inv_ItemFragment.h"

#include "Widgets/Composite/Inv_CompositeBase.h"
#include "Widgets/Composite/Inv_Leaf_Image.h"
#include "Widgets/Composite/Inv_Leaf_Text.h"

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

void FInv_HealthPotionFragment::OnConsume(APlayerController* PC)
{
    // 실제 구현에서는 다음 중 하나를 사용하여 체력을 회복시킵니다:
    // - PC 또는 PC->GetPawn()에서 스탯 컴포넌트를 가져와서 체력 증가
    // - Ability System Component를 가져와서 Gameplay Effect 적용
    // - Healing() 인터페이스 함수 호출

    // 현재는 디버그 메시지만 출력합니다 (테스트/프로토타입 용도)
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Health Potion consumed! Healing by: %f"), HealAmount));
}

void FInv_ManaPotionFragment::OnConsume(APlayerController* PC)
{
    // 실제 구현에서는 프로젝트의 마나 시스템에 맞게 마나를 회복시킵니다
    // 예: Ability System Component를 통한 마나 회복 Gameplay Effect 적용

    // 현재는 디버그 메시지만 출력합니다 (테스트/프로토타입 용도)
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Mana Potion consumed! Mana replenished by: %f"), ManaAmount));
}