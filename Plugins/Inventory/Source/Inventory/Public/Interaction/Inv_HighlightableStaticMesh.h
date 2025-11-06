// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Highlightable.h"
#include "Components/StaticMeshComponent.h"
#include "Inv_HighlightableStaticMesh.generated.h"

/**
 * 하이라이트 가능한 스태틱 메시 컴포넌트
 * IInv_Highlightable 인터페이스를 구현하여 재질 기반 하이라이트 효과를 제공합니다
 * 플레이어가 아이템을 바라볼 때 하이라이트 재질로 전환됩니다
 */
UCLASS()
class INVENTORY_API UInv_HighlightableStaticMesh : public UStaticMeshComponent, public IInv_Highlightable
{
	GENERATED_BODY()

public:
	/**
	 * 하이라이트 처리 구현
	 * 메시의 재질을 하이라이트 재질로 교체합니다
	 */
	virtual void Highlight_Implementation() override;

	/**
	 * 하이라이트 해제 구현
	 * 메시의 재질을 원래 재질로 복원합니다
	 */
	virtual void UnHighlight_Implementation() override;

private:

	/** 하이라이트 시 적용할 재질 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UMaterialInterface> HighlightMaterial;
};
