// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Inv_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Inventory.h"
#include "Interaction/Inv_Highlightable.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/HUD/Inv_HUDWidget.h"

AInv_PlayerController::AInv_PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	TraceLength = 500.f;
}

void AInv_PlayerController::ToggleInventoryMenu()
{
	if (!InventoryComponent.IsValid()) return;
	InventoryComponent->ToggleInventoryMenu();
}

void AInv_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (IsValid(Subsystem))
	{
		Subsystem->AddMappingContext(DefaultIMC, 0);
	}

	InventoryComponent = FindComponentByClass<UInv_InventoryComponent>();

	CreateHUDWidget();
}

void AInv_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ThisClass::PrimaryInteract);

	EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ThisClass::ToggleInventoryMenu);
}

void AInv_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceForItem();
	
}

void AInv_PlayerController::PrimaryInteract()
{
	if (!CurrentItemActor.IsValid()) return;

	UInv_ItemComponent* ItemComponent = CurrentItemActor->FindComponentByClass<UInv_ItemComponent>();
	if (!IsValid(ItemComponent) || !InventoryComponent.IsValid()) return;

	InventoryComponent->TryAddItem(ItemComponent);
	
}

void AInv_PlayerController::CreateHUDWidget()
{
	if (!IsLocalPlayerController()) return;

	HUDWidget = CreateWidget<UInv_HUDWidget>(this, HUDWidgetClass);

	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

void AInv_PlayerController::TraceForItem()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;
	
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter {ViewportSize / 2.f};

	FVector TraceStart;
	FVector Forward;

	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCenter, TraceStart, Forward)) return;

	const FVector TraceEnd {TraceStart + Forward * TraceLength};
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ItemTraceChannel);

	LastItemActor = CurrentItemActor;
	CurrentItemActor = HitResult.GetActor();

	if (!CurrentItemActor.IsValid())
	{
		if (IsValid(HUDWidget)) HUDWidget->HidePickupMessage();
	}
	
	if (LastItemActor == CurrentItemActor) return;

	if(CurrentItemActor.IsValid())
	{
		if (UActorComponent* Highlightable = CurrentItemActor->FindComponentByInterface(UInv_Highlightable::StaticClass()); IsValid(Highlightable))
		{
			IInv_Highlightable::Execute_Highlight(Highlightable);
		}
		
		UInv_ItemComponent* ItemComponent = CurrentItemActor->FindComponentByClass<UInv_ItemComponent>();
		if (!IsValid(ItemComponent)) return;

		if (IsValid(HUDWidget)) HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
	}

	if (LastItemActor.IsValid())
	{
		if (UActorComponent* Highlightable = LastItemActor->FindComponentByInterface(UInv_Highlightable::StaticClass()); IsValid(Highlightable))
		{
			IInv_Highlightable::Execute_UnHighlight(Highlightable);
		}
	}
	
}
