// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Inv_PlayerContriller.generated.h"

class UInv_HUDWidget;
class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class INVENTORY_API AInv_PlayerContriller : public APlayerController
{
	GENERATED_BODY()

public:
	AInv_PlayerContriller();
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
private:

	void PrimaryInteract();
	void CreateHUDWidget();
	void TraceForItem();
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputMappingContext> DefaultIMC;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInv_HUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UInv_HUDWidget> HUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	double TraceLength;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TEnumAsByte<ECollisionChannel> ItemTraceChannel;

	TWeakObjectPtr<AActor> CurrentItemActor;
	TWeakObjectPtr<AActor> LastItemActor;
};
