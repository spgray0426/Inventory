// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Inv_EquipmentComponent.generated.h"

/**
 * 
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
    
protected:
    virtual void BeginPlay() override;

private:
    
    UFUNCTION()
    void OnItemEquipped(UInv_InventoryItem* EquippedItem);

    UFUNCTION()
    void OnItemUnequipped(UInv_InventoryItem* UnequippedItem);

    void InitInventoryComponent();
    
    AInv_EquipActor* SpawnEquippedActor(FInv_EquipmentFragment* EquipmentFragment, const FInv_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh);

    AInv_EquipActor* FindEquippedActor(const FGameplayTag& EquipmentTypeTag);
    void RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag);
    
    UPROPERTY()
    TArray<TObjectPtr<AInv_EquipActor>> EquippedActors;
    
    TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
    TWeakObjectPtr<APlayerController> OwningPlayerController;
    TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletalMesh;


};
