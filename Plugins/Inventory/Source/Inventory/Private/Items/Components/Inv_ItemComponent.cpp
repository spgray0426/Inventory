// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Components/Inv_ItemComponent.h"

#include "Net/UnrealNetwork.h"


UInv_ItemComponent::UInv_ItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	PickupMessage = FString(TEXT("E - Item picked up"));

}

void UInv_ItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemManifest);
}

void UInv_ItemComponent::InitItemManifest(FInv_ItemManifest CopyOfManifest)
{
	// 전달받은 매니페스트로 아이템 정보를 설정합니다
	// 드롭된 아이템이 월드에 스폰될 때 올바른 아이템 정보를 가지도록 합니다
	ItemManifest = CopyOfManifest;
}

void UInv_ItemComponent::PickedUp()
{
	// 블루프린트에서 구현 가능한 OnPickedUp 이벤트를 발생시킵니다
	OnPickedUp();
	// 픽업된 아이템 액터를 월드에서 제거합니다
	GetOwner()->Destroy();
}

