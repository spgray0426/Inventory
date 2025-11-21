// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Composite/Inv_Composite.h"
#include "Inv_ItemDescription.generated.h"

class USizeBox;
/**
 * 아이템 설명 위젯
 * 마우스를 아이템 위에 올렸을 때 아이템 정보를 표시합니다
 * SizeBox를 사용하여 동적으로 크기가 조정됩니다
 */
UCLASS()
class INVENTORY_API UInv_ItemDescription : public UInv_Composite
{
    GENERATED_BODY()

public:

    /**
     * 설명 위젯의 크기를 가져옵니다
     * 화면 경계 내에 위젯을 배치할 때 사용됩니다
     * @return 위젯의 desired size (픽셀 단위)
     */
    FVector2D GetBoxSize() const;

    virtual void SetVisibility(ESlateVisibility InVisibility) override;
private:

    /** 위젯의 크기를 결정하는 SizeBox 컴포넌트 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USizeBox> SizeBox;
};
