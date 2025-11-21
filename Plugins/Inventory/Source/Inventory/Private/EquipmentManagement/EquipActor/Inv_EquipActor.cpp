// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"


AInv_EquipActor::AInv_EquipActor()
{
    // 장비 액터는 틱이 필요하지 않음
    PrimaryActorTick.bCanEverTick = false;

    // 복제 비활성화 (클라이언트 측에서만 시각적 표현용)
    bReplicates = false;
}
