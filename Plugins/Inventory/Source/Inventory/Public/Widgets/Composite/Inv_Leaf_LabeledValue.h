// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Inv_Leaf_LabeledValue.generated.h"


class UTextBlock;

/**
 * 라벨과 값을 함께 표시하는 Leaf 위젯 컴포넌트.
 *
 * 이 클래스는 컴포지트 패턴의 리프 노드로, 라벨과 값 두 개의 텍스트를 함께 표시합니다.
 * 주로 아이템 스탯 정보(공격력, 방어력 등)를 표시하는 데 사용됩니다.
 *
 * 예시: "공격력: 25.5"
 *        ^^^^^^^^  ^^^^
 *        라벨      값
 *
 * @see UInv_Leaf
 * @see FInv_LabeledNumberFragment
 */
UCLASS()
class INVENTORY_API UInv_Leaf_LabeledValue : public UInv_Leaf
{
    GENERATED_BODY()

public:
    /**
     * 라벨 텍스트를 설정합니다.
     *
     * @param Text 설정할 라벨 텍스트 (예: "공격력:")
     * @param bCollapse true일 경우 라벨을 숨깁니다
     */
    void SetText_Label(const FText& Text, bool bCollapse) const;

    /**
     * 값 텍스트를 설정합니다.
     *
     * @param Text 설정할 값 텍스트 (예: "25.5")
     * @param bCollapse true일 경우 값을 숨깁니다
     */
    void SetText_Value(const FText& Text, bool bCollapse) const;

    /**
     * 위젯이 구성될 때 호출되어 폰트 크기를 적용합니다.
     */
    virtual void NativePreConstruct() override;

private:
    /** 라벨을 표시하는 TextBlock 위젯. 위젯 블루프린트에서 바인딩됩니다. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_Label;

    /** 값을 표시하는 TextBlock 위젯. 위젯 블루프린트에서 바인딩됩니다. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_Value;

    /** 라벨의 폰트 크기. 에디터에서 설정할 수 있습니다. */
    UPROPERTY(EditAnywhere, Category = "Inventory")
    int32 FontSize_Label{12};

    /** 값의 폰트 크기. 에디터에서 설정할 수 있습니다. */
    UPROPERTY(EditAnywhere, Category = "Inventory")
    int32 FontSize_Value{18};
};
