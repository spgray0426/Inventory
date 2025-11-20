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

	UE_DEFINE_GAMEPLAY_TAG(EquipmentFragment, "FragmentTags.EquipmentFragment")
	
	
	
	
	
	/** 아이템 이름 및 텍스트 정보를 식별하기 위한 아이템 이름 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(ItemNameFragment, "FragmentTags.ItemNameFragment")

	/** 주요 스탯 정보를 식별하기 위한 주요 스탯 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(PrimaryStatFragment, "FragmentTags.PrimaryStatFragment")

	/** 아이템 타입 정보를 식별하기 위한 아이템 타입 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(ItemTypeFragment, "FragmentTags.ItemTypeFragment")

	/** 플레이버 텍스트 정보를 식별하기 위한 플레이버 텍스트 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(FlavorTextFragment, "FragmentTags.FlavorTextFragment")

	/** 판매 가격 정보를 식별하기 위한 판매 가격 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(SellValueFragment, "FragmentTags.SellValueFragment")

	/** 요구 레벨 정보를 식별하기 위한 요구 레벨 프래그먼트 태그 정의 */
	UE_DEFINE_GAMEPLAY_TAG(RequiredLevelFragment, "FragmentTags.RequiredLevelFragment")

	/**
	 * 스탯 수정자 프래그먼트 태그 정의
	 * 소비형 아이템의 스탯 효과를 식별하는 데 사용됩니다
	 */
	namespace StatMod
	{
		/** 첫 번째 스탯 수정자를 식별하기 위한 태그 정의 */
		UE_DEFINE_GAMEPLAY_TAG(StatMod_1, "FragmentTags.StatMod.1")

		/** 두 번째 스탯 수정자를 식별하기 위한 태그 정의 */
		UE_DEFINE_GAMEPLAY_TAG(StatMod_2, "FragmentTags.StatMod.2")

		/** 세 번째 스탯 수정자를 식별하기 위한 태그 정의 */
		UE_DEFINE_GAMEPLAY_TAG(StatMod_3, "FragmentTags.StatMod.3")
	}
}