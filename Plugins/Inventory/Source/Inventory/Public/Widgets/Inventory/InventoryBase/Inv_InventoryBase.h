// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Inv_GridTypes.h"
#include "Inv_InventoryBase.generated.h"

class UInv_ItemComponent;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent) const { return FInv_SlotAvailabilityResult();}
};
