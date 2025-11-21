// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Components/Inv_InventoryComponent.h"

#include "Items/Components/Inv_ItemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Fragments/Inv_ItemFragment.h"

UInv_InventoryComponent::UInv_InventoryComponent() : InventoryList(this)
{
	// 인벤토리는 틱이 필요하지 않음
	PrimaryComponentTick.bCanEverTick = false;

	// 네트워크 복제 활성화
	SetIsReplicatedByDefault(true);

	// 서브 오브젝트 리스트를 통한 복제 사용 (인벤토리 아이템 복제를 위함)
	bReplicateUsingRegisteredSubObjectList = true;

	bInventoryMenuOpen = false;
}

void UInv_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 인벤토리 리스트를 복제 속성으로 등록
	DOREPLIFETIME(ThisClass, InventoryList);
}

void UInv_InventoryComponent::TryAddItem(UInv_ItemComponent* ItemComponent)
{
	// 인벤토리에 아이템을 추가할 공간이 있는지 확인
	FInv_SlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);

	// 같은 타입의 아이템이 이미 인벤토리에 있는지 검색
	UInv_InventoryItem* FoundItem = InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());
	Result.Item = FoundItem;

	// 추가할 공간이 전혀 없으면 실패
	if (Result.TotalRoomToFill == 0)
	{
		NoRoomInInventory.Broadcast();
		return;
	}

	// 기존 아이템이 있고 스택 가능하면 스택에 추가
	if (Result.Item.IsValid() && Result.bStackable)
	{
		OnStackChange.Broadcast(Result);
		Server_AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	// 새로운 아이템으로 추가
	else if (Result.TotalRoomToFill > 0)
	{
		Server_AddNewItem(ItemComponent, Result.bStackable ? Result.TotalRoomToFill : 0, Result.Remainder);
	}
}

void UInv_InventoryComponent::Server_AddNewItem_Implementation(UInv_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	// 인벤토리 리스트에 새 아이템 엔트리 추가
	UInv_InventoryItem* NewItem = InventoryList.AddEntry(ItemComponent);
	NewItem->SetTotalStackCount(StackCount);

	// 리슨 서버나 스탠드얼론에서는 로컬에서도 델리게이트 호출
	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}

	// 남은 수량이 없으면 아이템 컴포넌트를 픽업 처리 (월드에서 제거)
	if (Remainder == 0)
	{
		ItemComponent->PickedUp();
	}
	// 남은 수량이 있으면 아이템 컴포넌트의 스택 수량 업데이트
	else if (FInv_StackableFragment* StackableFragment = ItemComponent->GetItemManifestMutable().GetFragmentOfTypeMutable<FInv_StackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}
}

void UInv_InventoryComponent::Server_AddStacksToItem_Implementation(UInv_ItemComponent* ItemComponent, int32 StackCount,int32 Remainder)
{
	// 아이템 컴포넌트에서 아이템 타입 가져오기
	const FGameplayTag& ItemType = IsValid(ItemComponent) ? ItemComponent->GetItemManifest().GetItemType() : FGameplayTag::EmptyTag;

	// 인벤토리에서 해당 타입의 아이템 찾기
	UInv_InventoryItem* Item = InventoryList.FindFirstItemByType(ItemType);
	if (!IsValid(Item)) return;

	// 기존 스택에 새로운 스택 수량 추가
	Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);

	if (Remainder == 0)
	{
		ItemComponent->PickedUp();
	}
	else if (FInv_StackableFragment* StackableFragment = ItemComponent->GetItemManifestMutable().GetFragmentOfTypeMutable<FInv_StackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}
}

void UInv_InventoryComponent::Server_DropItem_Implementation(UInv_InventoryItem* Item, int32 StackCount)
{
	// 드롭 후 남을 스택 수량을 계산합니다
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;
	
	// 모든 스택을 드롭하면 인벤토리에서 아이템을 완전히 제거합니다
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		// 일부만 드롭하면 남은 스택 수량을 업데이트합니다
		Item->SetTotalStackCount(NewStackCount);
	}

	SpawnDroppedItem(Item, StackCount);
}

void UInv_InventoryComponent::Server_ConsumeItem_Implementation(UInv_InventoryItem* Item)
{
	// 아이템의 스택 수량을 1 감소시킵니다
	const int32 NewStackCount = Item->GetTotalStackCount() - 1;

	// 스택이 0 이하가 되면 인벤토리에서 아이템을 완전히 제거합니다
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		// 스택이 남아있으면 새로운 수량으로 업데이트합니다
		Item->SetTotalStackCount(NewStackCount);
	}

	// 아이템의 소비 가능 프래그먼트를 찾아서 OnConsume을 호출합니다
	// 이를 통해 체력 회복, 마나 회복 등의 효과가 실행됩니다
	if (FInv_ConsumableFragment* ConsumableFragment = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FInv_ConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningController.Get());
	}
}

void UInv_InventoryComponent::Server_EquipSlotClicked_Implementation(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip)
{
	Multicast_EquipSlotClicked(ItemToEquip, ItemToUnequip);
}

void UInv_InventoryComponent::Multicast_EquipSlotClicked_Implementation(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip)
{
	// 장비 컴포넌트가 이 델리게이트를 리슨하여 장착/해제 처리
	OnItemEquipped.Broadcast(ItemToEquip);
	OnItemUnequipped.Broadcast(ItemToUnequip);
}

void UInv_InventoryComponent::ToggleInventoryMenu()
{
	// 인벤토리 메뉴 열기/닫기 토글
	if (bInventoryMenuOpen)
	{
		CloseInventoryMenu();
	}
	else
	{
		OpenInventoryMenu();
	}
	OnInventoryMenuToggled.Broadcast(bInventoryMenuOpen);
}

void UInv_InventoryComponent::AddRepSubObj(UObject* SubObj)
{
	// 복제 서브 오브젝트 리스트에 오브젝트 추가
	// 인벤토리 아이템을 복제하기 위해 사용됨
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObj))
	{
		AddReplicatedSubObject(SubObj);
	}
}

void UInv_InventoryComponent::SpawnDroppedItem(UInv_InventoryItem* Item, int32 StackCount)
{
	// 플레이어 폰의 참조를 가져옵니다
	const APawn* OwningPawn = OwningController->GetPawn();

	// 플레이어의 전방 벡터를 랜덤한 각도로 회전시킵니다 (Y축 기준)
	// 이를 통해 아이템이 플레이어 전방의 부채꼴 영역에 드롭됩니다
	FVector RotatedForward = OwningPawn->GetActorForwardVector();
	RotatedForward = RotatedForward.RotateAngleAxis(FMath::FRandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);

	// 회전된 벡터 방향으로 랜덤 거리만큼 떨어진 위치를 계산합니다
	FVector SpawnLocation = OwningPawn->GetActorLocation() + RotatedForward * FMath::FRandRange(DropSpawnDistanceMin, DropSpawnDistanceMax);

	// 바닥에 가까운 위치에 스폰하기 위해 Z축 오프셋을 적용합니다
	SpawnLocation.Z -= RelativeSpawnElevation;
	const FRotator SpawnRotation = FRotator::ZeroRotator;
	
	FInv_ItemManifest& ItemManifest = Item->GetItemManifestMutable();
	if (FInv_StackableFragment* StackableFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_StackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}

	// 설정된 매니페스트를 사용하여 월드에 픽업 액터를 스폰합니다
	ItemManifest.SpawnPickupActor(GetWorld(), SpawnLocation, SpawnRotation);
}

void UInv_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ConstructInventory();
}

void UInv_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("InventoryComponent must be attached to a PlayerController"));

	if (!OwningController->IsLocalController()) return;

	InventoryMenu = CreateWidget<UInv_InventoryBase>(OwningController.Get(), InventoryMenuClass);
	InventoryMenu->AddToViewport();
	CloseInventoryMenu();
}

void UInv_InventoryComponent::OpenInventoryMenu()
{
	 if (!IsValid(InventoryMenu)) return;

	InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	bInventoryMenuOpen = true;

	if (!OwningController.IsValid()) return;

	FInputModeGameAndUI InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(true);
	
}

void UInv_InventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu)) return;

	InventoryMenu->SetVisibility(ESlateVisibility::Hidden);
	bInventoryMenuOpen = false;

	if (!OwningController.IsValid()) return;
	FInputModeGameOnly InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(false);
	
}


