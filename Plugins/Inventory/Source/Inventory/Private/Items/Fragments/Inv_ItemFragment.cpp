/**
 * 아이템 프래그먼트 시스템 구현 파일
 * 기본 프래그먼트와 특화된 프래그먼트 타입들의 구현을 포함합니다
 */

#include "Items/Fragments/Inv_ItemFragment.h"


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