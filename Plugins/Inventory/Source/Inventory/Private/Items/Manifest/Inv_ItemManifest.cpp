#include "Items/Manifest/Inv_ItemManifest.h"

#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"

UInv_InventoryItem* FInv_ItemManifest::Manifest(UObject* NewOuter)
{
	UInv_InventoryItem* Item = NewObject<UInv_InventoryItem>(NewOuter, UInv_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);
	return Item;
}

void FInv_ItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	// 픽업 액터 클래스와 월드 컨텍스트가 유효한지 확인합니다
	if (!IsValid(PickupActorClass) || !IsValid(WorldContextObject)) return;

	// 월드 객체를 가져오고 유효성을 확인합니다
	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World)) return;

	// 지정된 위치와 회전으로 픽업 액터를 스폰합니다
	AActor* SpawnedActor = World->SpawnActor<AActor>(PickupActorClass, SpawnLocation, SpawnRotation);
	if (!IsValid(SpawnedActor)) return;

	// 스폰된 액터에서 ItemComponent를 찾아 아이템 매니페스트를 초기화합니다
	// 이를 통해 픽업 액터가 올바른 아이템 정보(타입, 카테고리, 프래그먼트 등)를 가지게 됩니다
	UInv_ItemComponent* ItemComp = SpawnedActor->FindComponentByClass<UInv_ItemComponent>();
	check(ItemComp);

	ItemComp->InitItemManifest(*this);
}
