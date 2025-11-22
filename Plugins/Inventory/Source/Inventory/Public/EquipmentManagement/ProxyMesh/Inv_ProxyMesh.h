// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inv_ProxyMesh.generated.h"

class UInv_EquipmentComponent;

/**
 * 인벤토리 UI에서 캐릭터와 장비를 미리보기 위한 프록시 메시 액터
 * 실제 플레이어 캐릭터와 별개로 장비 시스템을 시뮬레이션합니다.
 */
UCLASS()
class INVENTORY_API AInv_ProxyMesh : public AActor
{
    GENERATED_BODY()

public:
    AInv_ProxyMesh();

    /** 프록시 스켈레탈 메시 컴포넌트 가져오기 */
    USkeletalMeshComponent* GetMesh() const { return Mesh; }

protected:
    virtual void BeginPlay() override;

private:
    
    void DelayedInitializeOwner();
    void DelayedInitialization();
    
    /** 플레이어 캐릭터의 원본 메시 참조 (향후 확장용) */
    TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;

    /** 프록시 메시용 장비 컴포넌트 (UI 미리보기 전용) */
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UInv_EquipmentComponent> EquipmentComponent;

    /** 인벤토리 메뉴에서 표시될 프록시 스켈레탈 메시 */
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USkeletalMeshComponent> Mesh;
    
    FTimerHandle TimerForNextTick;
};
