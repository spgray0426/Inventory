// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Inv_EquipmentComponent.generated.h"

/**
 * 장비 시스템을 관리하는 액터 컴포넌트
 * 인벤토리의 장착/해제 이벤트를 받아 실제 장비 액터를 스폰하고 관리합니다.
 */

struct FGameplayTag;
struct FInv_ItemManifest;
struct FInv_EquipmentFragment;
class AInv_EquipActor;
class UInv_InventoryItem;
class UInv_InventoryComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_EquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** 장비가 부착될 스켈레탈 메시 설정 */
    void SetOwningSkeletalMesh(USkeletalMeshComponent* OwningMesh);

    /** 프록시 메시 여부 설정 (UI용 프리뷰 메시인 경우 true) */
    void SetIsProxy(bool bProxy) { bIsProxy = bProxy; }
    
    void InitializeOwner(APlayerController* PlayerController);
protected:
    virtual void BeginPlay() override;

private:
    /** 인벤토리 컴포넌트 초기화 및 이벤트 바인딩 */
    void InitInventoryComponent();

    /** 플레이어 컨트롤러 초기화 */
    void InitPlayerController();

    /** 아이템이 장착되었을 때 호출되는 콜백 */
    UFUNCTION()
    void OnItemEquipped(UInv_InventoryItem* EquippedItem);

    /** 아이템이 해제되었을 때 호출되는 콜백 */
    UFUNCTION()
    void OnItemUnequipped(UInv_InventoryItem* UnequippedItem);

    /** Pawn이 변경되었을 때 호출되는 콜백 (캐릭터 변경 대응) */
    UFUNCTION()
    void OnPossessedPawnChange(APawn* OldPawn, APawn* NewPawn);

    /** 장비 액터를 스폰하고 스켈레탈 메시에 부착 */
    AInv_EquipActor* SpawnEquippedActor(FInv_EquipmentFragment* EquipmentFragment, const FInv_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh);

    /** 특정 장비 타입의 장착된 액터를 찾기 */
    AInv_EquipActor* FindEquippedActor(const FGameplayTag& EquipmentTypeTag);

    /** 특정 장비 타입의 장착된 액터를 제거 */
    void RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag);

    /** 현재 장착된 모든 장비 액터 배열 */
    UPROPERTY()
    TArray<TObjectPtr<AInv_EquipActor>> EquippedActors;

    /** 인벤토리 컴포넌트 참조 (약한 참조) */
    TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;

    /** 소유 플레이어 컨트롤러 참조 (약한 참조) */
    TWeakObjectPtr<APlayerController> OwningPlayerController;

    /** 장비가 부착될 스켈레탈 메시 참조 (약한 참조) */
    TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletalMesh;

    /** UI 프리뷰용 프록시 메시인지 여부 */
    bool bIsProxy{false};


};
