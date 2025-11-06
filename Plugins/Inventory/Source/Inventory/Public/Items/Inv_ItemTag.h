#pragma once

#include "NativeGameplayTags.h"

/**
 * 게임 아이템들에 대한 GameplayTag 선언을 포함하는 네임스페이스
 * 각 아이템은 고유한 GameplayTag로 식별되며, 계층적 구조로 관리됩니다
 */
namespace GameItems
{
	/** 장비 아이템 카테고리 */
	namespace Equipment
	{
		/** 무기 타입 장비 */
		namespace Weapons
		{
			/** 도끼 아이템 태그 */
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Axe)

			/** 검 아이템 태그 */
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword)
		}

		/** 망토 타입 장비 */
		namespace Cloaks
		{
			/** 붉은 망토 아이템 태그 */
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(RedCloak)
		}

		/** 가면 타입 장비 */
		namespace Masks
		{
			/** 강철 가면 아이템 태그 */
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(SteelMask)
		}
	}

	/** 소비 아이템 카테고리 */
	namespace Consumables
	{
		/** 포션 타입 소비품 */
		namespace Potions
		{
			/** 빨간 포션 */
			namespace Red
			{
				/** 작은 빨간 포션 아이템 태그 */
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Small)

				/** 큰 빨간 포션 아이템 태그 */
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Large)
			}

			/** 파란 포션 */
			namespace Blue
			{
				/** 작은 파란 포션 아이템 태그 */
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Small)

				/** 큰 파란 포션 아이템 태그 */
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Large)
			}
		}
	}

	/** 제작 재료 아이템 카테고리 */
	namespace Craftables
	{
		/** 불꽃 양치류 열매 아이템 태그 */
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireFernFruit)

		/** 빛나는 데이지 아이템 태그 */
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(LuminDaisy)

		/** 그을린 꽃잎 꽃 아이템 태그 */
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ScorchPetalBlossom)
	}
}