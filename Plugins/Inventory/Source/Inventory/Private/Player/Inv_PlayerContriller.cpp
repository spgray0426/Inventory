// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Inv_PlayerContriller.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Inventory.h"
#include "Interaction/Inv_Highlightable.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/HUD/Inv_HUDWidget.h"

AInv_PlayerContriller::AInv_PlayerContriller()
{
	PrimaryActorTick.bCanEverTick = true;
	
	TraceLength = 500.f;
}

void AInv_PlayerContriller::ToggleInventoryMenu()
{
	if (!InventoryComponent.IsValid()) return;
	InventoryComponent->ToggleInventoryMenu();
}

void AInv_PlayerContriller::BeginPlay()
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

void AInv_PlayerContriller::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ThisClass::PrimaryInteract);

	EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ThisClass::ToggleInventoryMenu);
}

void AInv_PlayerContriller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceForItem();
	
}

void AInv_PlayerContriller::PrimaryInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("상호작용"));
}

void AInv_PlayerContriller::CreateHUDWidget()
{
	if (!IsLocalPlayerController()) return;

	HUDWidget = CreateWidget<UInv_HUDWidget>(this, HUDWidgetClass);

	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

void AInv_PlayerContriller::TraceForItem()
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
