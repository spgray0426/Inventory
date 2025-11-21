// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/ProxyMesh/Inv_ProxyMesh.h"

#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"


AInv_ProxyMesh::AInv_ProxyMesh()
{
    PrimaryActorTick.bCanEverTick = false;

    // 루트 컴포넌트 생성
    RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

    // 프록시 스켈레탈 메시 생성 및 부착
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
    Mesh->SetupAttachment(RootComponent);

    // 장비 컴포넌트 생성 및 프록시 모드 설정
    EquipmentComponent = CreateDefaultSubobject<UInv_EquipmentComponent>("Equipment");
    EquipmentComponent->SetOwningSkeletalMesh(Mesh);
    EquipmentComponent->SetIsProxy(true); // UI 프리뷰용임을 명시
}

void AInv_ProxyMesh::BeginPlay()
{
    Super::BeginPlay();
}