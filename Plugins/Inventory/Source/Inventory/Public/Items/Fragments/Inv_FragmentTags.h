#pragma once

#include "NativeGameplayTags.h"

/**
 * 모든 아이템 프래그먼트 타입에 대한 GameplayTag 선언을 포함하는 네임스페이스
 * 이 태그들은 아이템에서 특정 프래그먼트를 식별하고 검색하는 데 사용됩니다
 */
namespace FragmentTags
{
	/** 그리드 프래그먼트 태그 (FragmentTags.GridFragment) - 아이템 그리드 차원 정의 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GridFragment)

	/** 아이콘 프래그먼트 태그 (FragmentTags.IconFragment) - 아이템 시각적 표현 정의 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(IconFragment)

	/** 스택 가능 프래그먼트 태그 (FragmentTags.StackableFragment) - 아이템 스택 기능 활성화 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(StackableFragment)

	/** 소비 가능 프래그먼트 태그 (FragmentTags.ConsumableFragment) - 아이템 소비 기능 활성화 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ConsumableFragment)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentFragment)
	
	
	
	/** 아이템 이름 프래그먼트 태그 (FragmentTags.ItemNameFragment) - 아이템 이름 및 텍스트 정보 정의 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemNameFragment)

	/** 주요 스탯 프래그먼트 태그 (FragmentTags.PrimaryStatFragment) - 아이템의 주요 스탯 정보 정의 (공격력, 방어력 등) */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(PrimaryStatFragment)

	/** 아이템 타입 프래그먼트 태그 (FragmentTags.ItemTypeFragment) - 아이템의 타입 정보 정의 (무기, 방어구, 소비품 등) */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemTypeFragment)

	/** 플레이버 텍스트 프래그먼트 태그 (FragmentTags.FlavorTextFragment) - 아이템의 배경 스토리나 설명 텍스트 정의 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FlavorTextFragment)

	/** 판매 가격 프래그먼트 태그 (FragmentTags.SellValueFragment) - 아이템을 상점에 판매할 때의 가격 정의 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SellValueFragment)

	/** 요구 레벨 프래그먼트 태그 (FragmentTags.RequiredLevelFragment) - 아이템 사용에 필요한 최소 캐릭터 레벨 정의 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(RequiredLevelFragment)

	/**
	 * 스탯 수정자 프래그먼트 태그들을 포함하는 네임스페이스
	 * 소비형 아이템이 제공하는 다양한 스탯 수정 효과를 식별하는 데 사용됩니다
	 * (예: 체력 회복, 마나 회복, 버프 효과 등)
	 */
	namespace StatMod
	{
		/** 첫 번째 스탯 수정자 태그 (FragmentTags.StatMod.1) - 소비 시 첫 번째 스탯 효과 정의 */
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatMod_1)

		/** 두 번째 스탯 수정자 태그 (FragmentTags.StatMod.2) - 소비 시 두 번째 스탯 효과 정의 */
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatMod_2)

		/** 세 번째 스탯 수정자 태그 (FragmentTags.StatMod.3) - 소비 시 세 번째 스탯 효과 정의 */
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatMod_3)
	}
}