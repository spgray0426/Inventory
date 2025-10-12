// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"
#include "Inv_SpatialInventory.generated.h"

class UButton;
class UWidgetSwitcher;
class UInv_InventoryGrid;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_SpatialInventory : public UInv_InventoryBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent) const override;
private:

	UFUNCTION()
	void ShowEquippables();
	
	UFUNCTION()
	void ShowConsumables();
	
	UFUNCTION()
	void ShowCraftables();

	void DisableButtons(UButton* Button);

	void SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Equippables;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Consumables;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Craftables;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Equippables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Consumables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Craftables;
};
