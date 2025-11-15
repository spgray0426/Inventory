// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Inv_Leaf_Text.generated.h"

class UTextBlock;

/**
 * 컴포지트 위젯 계층 구조에서 텍스트를 표시하는 Leaf 위젯 컴포넌트.
 *
 * 이 클래스는 인벤토리 UI 시스템의 Composite 패턴 구현에서 리프 노드를 나타냅니다.
 * 폰트 크기 설정이 가능한 텍스트 위젯을 표시하고 관리하는 기능을 제공합니다.
 *
 * @see UInv_Leaf
 * @see UInv_CompositeBase
 */
UCLASS()
class INVENTORY_API UInv_Leaf_Text : public UInv_Leaf
{
    GENERATED_BODY()

public:
    /**
     * 텍스트 위젯에 표시될 텍스트를 설정합니다.
     *
     * @param Text 표시할 텍스트.
     */
    void SetText(const FText& Text) const;

    /**
     * 위젯이 구성될 때 호출되어 폰트 크기를 적용합니다.
     */
    virtual void NativePreConstruct() override;

private:
    /** 텍스트를 표시하는 TextBlock 위젯. 위젯 블루프린트에서 바인딩됩니다. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_LeafText;

    /** 텍스트의 폰트 크기. 에디터에서 설정할 수 있습니다. */
    UPROPERTY(EditAnywhere, Category = "Inventory")
    int32 FontSize{12};
};
