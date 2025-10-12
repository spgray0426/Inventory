// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Utils/Inv_InventoryStatics.h"

#include "InventoryManagement/Components/Inv_InventoryComponent.h"

UInv_InventoryComponent* UInv_InventoryStatics::GetInventoryComponent(const APlayerController* PlayerController)
{

	if (!IsValid(PlayerController)) return nullptr;
	
	return PlayerController->FindComponentByClass<UInv_InventoryComponent>();
}
