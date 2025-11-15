/**
 * 프래그먼트 GameplayTag 구현 파일
 * 프래그먼트 식별에 사용되는 실제 GameplayTag 인스턴스를 정의합니다
 */

#include "Items/Fragments/Inv_FragmentTags.h"

namespace FragmentTags
{
	/** 그리드 차원 프래그먼트를 식별하기 위한 그리드 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(GridFragment, "FragmentTags.GridFragment")

	/** 시각적 표현 프래그먼트를 식별하기 위한 아이콘 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(IconFragment, "FragmentTags.IconFragment")

	/** 스택 기능 프래그먼트를 식별하기 위한 스택 가능 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(StackableFragment, "FragmentTags.StackableFragment")

	/** 소비 가능 프래그먼트를 식별하기 위한 소비 가능 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(ConsumableFragment, "FragmentTags.ConsumableFragment")

	/** 아이템 이름 및 텍스트 정보를 식별하기 위한 아이템 이름 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(ItemNameFragment, "FragmentTags.ItemNameFragment")

	/** 주요 스탯 정보를 식별하기 위한 주요 스탯 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(PrimaryStatFragment, "FragmentTags.PrimaryStatFragment")
}