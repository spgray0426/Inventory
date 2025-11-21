// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Inv_EquipActor.generated.h"

/**
 * 캐릭터에 실제로 부착되는 장비 액터 클래스
 * 장비 프래그먼트에 의해 스폰되며, 스켈레탈 메시의 소켓에 부착됩니다.
 */
UCLASS()
class INVENTORY_API AInv_EquipActor : public AActor
{
    GENERATED_BODY()

public:
    AInv_EquipActor();

    /** 장비 타입 태그 가져오기 (예: Inventory.Equipment.Type.Cloak) */
    FGameplayTag GetEquipmentType() const { return EquipmentType; }

    /** 장비 타입 태그 설정 */
    void SetEquipmentType(FGameplayTag Type) { EquipmentType = Type; }

private:
    /** 이 장비의 타입을 나타내는 GameplayTag */
    UPROPERTY(EditAnywhere, Category = "Inventory")
    FGameplayTag EquipmentType;
};
