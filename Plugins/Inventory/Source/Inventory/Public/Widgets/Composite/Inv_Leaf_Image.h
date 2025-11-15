// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Inv_Leaf_Image.generated.h"

class UImage;
class USizeBox;

/**
 * 컴포지트 위젯 계층 구조에서 이미지를 표시하는 Leaf 위젯 컴포넌트.
 *
 * 이 클래스는 인벤토리 UI 시스템의 Composite 패턴 구현에서 리프 노드를 나타냅니다.
 * 크기 설정이 가능한 이미지 위젯을 표시하고 관리하는 기능을 제공합니다.
 *
 * @see UInv_Leaf
 * @see UInv_CompositeBase
 */
UCLASS()
class INVENTORY_API UInv_Leaf_Image : public UInv_Leaf
{
    GENERATED_BODY()

public:
    /**
     * 이미지 위젯에 표시될 텍스처를 설정합니다.
     *
     * @param Texture 표시할 텍스처. nullptr을 전달하면 이미지가 지워집니다.
     */
    void SetImage(UTexture2D* Texture) const;

    /**
     * 이미지를 감싸는 SizeBox의 크기를 설정합니다.
     *
     * @param Size SizeBox의 크기 (X = 너비, Y = 높이).
     */
    void SetBoxSize(const FVector2D& Size) const;

    /**
     * 이미지 자체의 원하는 크기를 설정합니다.
     *
     * @param Size 이미지의 원하는 크기 (X = 너비, Y = 높이).
     */
    void SetImageSize(const FVector2D& Size) const;

    /**
     * 이미지의 현재 원하는 크기를 가져옵니다.
     *
     * @return 이미지 위젯의 현재 원하는 크기.
     */
    FVector2D GetImageSize() const;

private:
    /** 아이콘을 표시하는 이미지 위젯. 위젯 블루프린트에서 바인딩됩니다. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> Image_Icon;

    /** 이미지 위젯을 담는 SizeBox 컨테이너. 위젯 블루프린트에서 바인딩됩니다. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USizeBox> SizeBox_Icon;
};
 