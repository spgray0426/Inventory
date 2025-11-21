// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_CharacterDisplay.generated.h"

/**
 * 인벤토리 UI에서 캐릭터를 3D로 표시하고 회전 가능하도록 하는 위젯
 * 프록시 메시를 드래그하여 캐릭터를 회전시킬 수 있습니다.
 */
UCLASS()
class INVENTORY_API UInv_CharacterDisplay : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 마우스 버튼 눌림 - 드래그 시작 */
    virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    /** 마우스 버튼 떼어짐 - 드래그 종료 */
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    /** 마우스가 위젯 영역을 벗어남 - 드래그 취소 */
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    /** 위젯 초기화 - 프록시 메시 찾기 */
    virtual void NativeOnInitialized() override;

    /** 틱 - 드래그 중 회전 처리 */
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    /** 현재 드래그 중인지 여부 */
    bool bIsDragging{false};

    /** 회전시킬 프록시 메시 참조 */
    TWeakObjectPtr<USkeletalMeshComponent> Mesh;

    /** 현재 마우스 위치 */
    FVector2D CurrentPosition;

    /** 이전 프레임의 마우스 위치 */
    FVector2D LastPosition;
};
