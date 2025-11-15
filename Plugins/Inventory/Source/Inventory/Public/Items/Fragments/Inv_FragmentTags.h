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

	/** 아이템 이름 프래그먼트 태그 (FragmentTags.ItemNameFragment) - 아이템 이름 및 텍스트 정보 정의 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemNameFragment)
}