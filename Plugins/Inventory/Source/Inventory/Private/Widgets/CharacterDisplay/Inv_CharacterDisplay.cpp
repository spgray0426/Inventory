// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterDisplay/Inv_CharacterDisplay.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "EquipmentManagement/ProxyMesh/Inv_ProxyMesh.h"
#include "Kismet/GameplayStatics.h"

FReply UInv_CharacterDisplay::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    // 드래그 시작 - 현재 마우스 위치 저장
    CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
    LastPosition = CurrentPosition;

    bIsDragging = true;
    return FReply::Handled();
}

FReply UInv_CharacterDisplay::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 드래그 종료
    bIsDragging = false;
    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UInv_CharacterDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    // 마우스가 위젯을 벗어나면 드래그 취소
    bIsDragging = false;
}

void UInv_CharacterDisplay::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // 월드에서 프록시 메시 액터 찾기
    TArray<AActor*> Actors;
    UGameplayStatics::GetAllActorsOfClass(this, AInv_ProxyMesh::StaticClass(), Actors);

    if (!Actors.IsValidIndex(0)) return;

    AInv_ProxyMesh* ProxyMesh = Cast<AInv_ProxyMesh>(Actors[0]);
    if (!IsValid(ProxyMesh)) return;

    // 프록시 메시의 스켈레탈 메시 컴포넌트 참조 저장
    Mesh = ProxyMesh->GetMesh();
}

void UInv_CharacterDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (!bIsDragging) return;

    // 마우스 이동 거리 계산
    LastPosition = CurrentPosition;
    CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

    const float HorizontalDelta = LastPosition.X - CurrentPosition.X;

    // 메시를 Yaw축으로 회전 (좌우 드래그에 따라 캐릭터 회전)
    if (!Mesh.IsValid()) return;
    Mesh->AddRelativeRotation(FRotator(0.f, HorizontalDelta, 0.f));
}