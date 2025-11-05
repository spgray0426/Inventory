// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Inv_HoverItem.generated.h"

class UInv_InventoryItem;
class UTextBlock;
class UImage;

/**
 * 호버 아이템 위젯 클래스
 * 드래그 중인 아이템을 마우스 커서를 따라다니며 표시합니다
 * 아이템 아이콘, 스택 개수, 그리드 차원 등의 정보를 표시합니다
 */
UCLASS()
class INVENTORY_API UInv_HoverItem : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * 아이템 이미지 브러시를 설정합니다
     * @param Brush 적용할 슬레이트 브러시
     */
    void SetImageBrush(const FSlateBrush& Brush) const;

    /**
     * 스택 개수 텍스트를 업데이트합니다
     * @param Count 표시할 스택 개수
     */
    void UpdateStackCount(const int32 Count);

    /**
     * 아이템 타입을 가져옵니다
     * @return 아이템 타입 GameplayTag
     */
    FGameplayTag GetItemType() const;

    /**
     * 현재 스택 개수를 가져옵니다
     * @return 스택 개수
     */
    int32 GetStackCount() const { return StackCount; }

    /**
     * 아이템이 스택 가능한지 확인합니다
     * @return 스택 가능한 경우 true
     */
    bool IsStackable() const { return bIsStackable; }

    /**
     * 스택 가능 여부를 설정합니다
     * @param bStacks 스택 가능 여부
     */
    void SetIsStackable(bool bStacks);

    /**
     * 이전 그리드 인덱스를 가져옵니다
     * @return 이전 그리드 인덱스
     */
    int32 GetPreviousGridIndex() const { return PreviousGridIndex; }

    /**
     * 이전 그리드 인덱스를 설정합니다
     * @param Index 이전 그리드 인덱스
     */
    void SetPreviousGridIndex(int32 Index) { PreviousGridIndex = Index; }

    /**
     * 그리드 차원을 가져옵니다
     * @return 그리드 차원 (너비, 높이)
     */
    FIntPoint GetGridDimensions() const { return GridDimensions; }

    /**
     * 그리드 차원을 설정합니다
     * @param Dimensions 그리드 차원 (너비, 높이)
     */
    void SetGridDimensions(const FIntPoint& Dimensions) { GridDimensions = Dimensions; }

    /**
     * 인벤토리 아이템을 가져옵니다
     * @return 인벤토리 아이템 포인터
     */
    UInv_InventoryItem* GetInventoryItem() const;

    /**
     * 인벤토리 아이템을 설정합니다
     * @param Item 설정할 인벤토리 아이템
     */
    void SetInventoryItem(UInv_InventoryItem* Item);

private:

    /** 아이템 아이콘 이미지 위젯 (블루프린트에서 바인딩됨) */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> Image_Icon;

    /** 스택 개수 텍스트 위젯 (블루프린트에서 바인딩됨) */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_StackCount;

    /** 아이템의 이전 그리드 인덱스 (픽업 전 위치) */
    int32 PreviousGridIndex;

    /** 아이템의 그리드 차원 (너비, 높이) */
    FIntPoint GridDimensions;

    /** 드래그 중인 인벤토리 아이템에 대한 약한 참조 */
    TWeakObjectPtr<UInv_InventoryItem> InventoryItem;

    /** 아이템이 스택 가능한지 여부 */
    bool bIsStackable{false};

    /** 현재 스택 개수 */
    int32 StackCount{0};

};
