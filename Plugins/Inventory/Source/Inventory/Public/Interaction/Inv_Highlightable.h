// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inv_Highlightable.generated.h"

/**
 * 하이라이트 가능한 객체를 위한 UInterface 클래스
 * 이 클래스는 수정할 필요가 없습니다
 */
UINTERFACE()
class UInv_Highlightable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 하이라이트 가능한 객체 인터페이스
 * 플레이어가 상호작용 가능한 아이템을 바라볼 때 시각적 하이라이트 효과를 제공합니다
 * 이 인터페이스를 구현하는 클래스는 하이라이트 및 해제 기능을 제공해야 합니다
 */
class INVENTORY_API IInv_Highlightable
{
	GENERATED_BODY()

public:

	/**
	 * 객체를 하이라이트 처리합니다
	 * 플레이어가 아이템을 바라볼 때 호출됩니다
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void Highlight();

	/**
	 * 객체의 하이라이트를 해제합니다
	 * 플레이어가 아이템에서 시선을 돌릴 때 호출됩니다
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory")
	void UnHighlight();
};
