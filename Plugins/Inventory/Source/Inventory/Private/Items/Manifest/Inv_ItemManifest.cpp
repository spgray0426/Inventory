#include "Items/Manifest/Inv_ItemManifest.h"

#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"

UInv_InventoryItem* FInv_ItemManifest::Manifest(UObject* NewOuter)
{
	// 새로운 인벤토리 아이템 객체를 생성합니다
	UInv_InventoryItem* Item = NewObject<UInv_InventoryItem>(NewOuter, UInv_InventoryItem::StaticClass());

	// 아이템에 이 매니페스트를 설정합니다
	Item->SetItemManifest(*this);

	// 각 프래그먼트의 Manifest() 메서드를 호출하여 초기화합니다
	// 예: FInv_LabeledNumberFragment의 경우 Min~Max 범위에서 랜덤 값 생성
	for (auto& Fragment : Item->GetItemManifestMutable().GetFragmentsMutable())
	{
		Fragment.GetMutable().Manifest();
	}

	// 매니페스트의 프래그먼트 데이터를 정리합니다 (아이템에 복사되었으므로)
	ClearFragments();

	return Item;
}

/**
 * 이 매니페스트의 모든 인벤토리 아이템 프래그먼트를 컴포지트 위젯에 동화시킵니다
 *
 * 동작 방식:
 * 1. GetAllFragmentsOfType을 사용하여 모든 FInv_InventoryItemFragment 타입의 프래그먼트를 가져옵니다
 * 2. 각 프래그먼트에 대해 컴포지트 위젯 계층 구조를 순회합니다
 * 3. 각 위젯에 대해 Assimilate 함수를 호출하여 프래그먼트 데이터를 동화시킵니다
 *
 * 이를 통해 아이템의 모든 관련 프래그먼트 데이터가 UI 위젯 계층에 반영됩니다
 * 예: 아이템 이름 프래그먼트가 제목 위젯에, 설명 프래그먼트가 본문 위젯에 동화됨
 */
void FInv_ItemManifest::AssimilateInventoryFragments(UInv_CompositeBase* Composite) const
{
	// 모든 인벤토리 아이템 프래그먼트를 가져옵니다
	const auto& InventoryItemFragments = GetAllFragmentsOfType<FInv_InventoryItemFragment>();

	// 각 프래그먼트에 대해
	for (const auto* Fragment : InventoryItemFragments)
	{
		// 컴포지트 위젯 계층 구조의 모든 위젯에 함수를 적용합니다
		Composite->ApplyFunction([Fragment](UInv_CompositeBase* Widget)
		{
			// 프래그먼트를 위젯에 동화시킵니다
			Fragment->Assimilate(Widget);
		});
	}
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

/**
 * 프래그먼트 배열을 비우고 메모리를 해제합니다
 *
 * 아이템이 생성된 후 매니페스트의 프래그먼트 데이터를 정리합니다.
 * 아이템에 이미 프래그먼트가 복사되었으므로 매니페스트의 원본 데이터는 필요 없습니다.
 */
void FInv_ItemManifest::ClearFragments()
{
	// 각 프래그먼트의 데이터를 리셋합니다
	for (auto& Fragment : Fragments)
	{
		Fragment.Reset();
	}

	// 프래그먼트 배열을 비우고 메모리를 해제합니다
	Fragments.Empty();
}
