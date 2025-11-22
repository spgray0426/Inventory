// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"

#include "Components/SceneCaptureComponent2D.h"
#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Fragments/Inv_ItemFragment.h"

void UInv_EquipmentComponent::SetOwningSkeletalMesh(USkeletalMeshComponent* OwningMesh)
{
    OwningSkeletalMesh = OwningMesh;
}

void UInv_EquipmentComponent::InitializeOwner(APlayerController* PlayerController)
{
    if (IsValid(PlayerController))
    {
        OwningPlayerController = PlayerController;
    }
    InitInventoryComponent();
}

void UInv_EquipmentComponent::BeginPlay()
{
    Super::BeginPlay();

    InitPlayerController();
}

void UInv_EquipmentComponent::InitPlayerController()
{
    // 소유자가 플레이어 컨트롤러인지 확인
    if (OwningPlayerController = Cast<APlayerController>(GetOwner()); OwningPlayerController.IsValid())
    {
        // 현재 Pawn이 캐릭터인 경우 즉시 초기화
        if (ACharacter* OwnerCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwnerCharacter))
        {
            OwningSkeletalMesh = OwnerCharacter->GetMesh();
            OnPossessedPawnChange(nullptr, OwnerCharacter);
        }
        else
        {
            // Pawn이 없는 경우 Pawn 변경 이벤트를 기다림
            OwningPlayerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChange);
        }
    }
}

void UInv_EquipmentComponent::InitInventoryComponent()
{
    // 플레이어 컨트롤러로부터 인벤토리 컴포넌트 가져오기
    InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(OwningPlayerController.Get());
    if (!InventoryComponent.IsValid()) return;

    // 장착 이벤트 바인딩 (중복 바인딩 방지)
    if (!InventoryComponent->OnItemEquipped.IsAlreadyBound(this, &ThisClass::OnItemEquipped))
    {
        InventoryComponent->OnItemEquipped.AddDynamic(this, &ThisClass::OnItemEquipped);
    }

    // 해제 이벤트 바인딩 (중복 바인딩 방지)
    if (!InventoryComponent->OnItemUnequipped.IsAlreadyBound(this, &ThisClass::OnItemUnequipped))
    {
        InventoryComponent->OnItemUnequipped.AddDynamic(this, &ThisClass::OnItemUnequipped);
    }
}

AInv_EquipActor* UInv_EquipmentComponent::SpawnEquippedActor(FInv_EquipmentFragment* EquipmentFragment, const FInv_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh)
{
    // 장비 프래그먼트를 통해 장비 액터 스폰 및 메시에 부착
    AInv_EquipActor* SpawnedEquipActor = EquipmentFragment->SpawnAttachedActor(AttachMesh);
    SpawnedEquipActor->SetEquipmentType(EquipmentFragment->GetEquipmentType());
    SpawnedEquipActor->SetOwner(GetOwner());
    EquipmentFragment->SetEquippedActor(SpawnedEquipActor);
    return SpawnedEquipActor;
}

AInv_EquipActor* UInv_EquipmentComponent::FindEquippedActor(const FGameplayTag& EquipmentTypeTag)
{
    // 장비 타입 태그와 정확히 일치하는 장착된 액터 찾기
    auto FoundActor = EquippedActors.FindByPredicate([&EquipmentTypeTag](const AInv_EquipActor* EquippedActor)
    {
        return EquippedActor->GetEquipmentType().MatchesTagExact(EquipmentTypeTag);
    });
    return FoundActor ? *FoundActor : nullptr;
}

void UInv_EquipmentComponent::RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag)
{
    // 해당 장비 타입의 액터를 찾아 제거 및 파괴
    if (AInv_EquipActor* EquippedActor = FindEquippedActor(EquipmentTypeTag); IsValid(EquippedActor))
    {
        EquippedActors.Remove(EquippedActor);
        
        if(USceneCaptureComponent2D* CaptureComp = Cast<USceneCaptureComponent2D>(GetOwner()->GetComponentByClass(USceneCaptureComponent2D::StaticClass()));IsValid(CaptureComp))
        {
            CaptureComp->ShowOnlyActorComponents(EquippedActor);
        }
        
        EquippedActor->Destroy();
    }
}

void UInv_EquipmentComponent::OnItemEquipped(UInv_InventoryItem* EquippedItem)
{
    if (!IsValid(EquippedItem)) return;
    if (!OwningPlayerController->HasAuthority()) return;

    // 아이템 매니페스트에서 장비 프래그먼트 가져오기
    FInv_ItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
    FInv_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_EquipmentFragment>();
    if (!EquipmentFragment) return;

    // 장비 장착 로직 실행
    if (!bIsProxy)
    {
        EquipmentFragment->OnEquip(OwningPlayerController.Get());
    }
    // 스켈레탈 메시에 장비 액터 스폰 및 부착
    if (!OwningSkeletalMesh.IsValid()) return;
    AInv_EquipActor* SpawnedEquipActor = SpawnEquippedActor(EquipmentFragment, ItemManifest, OwningSkeletalMesh.Get());

    EquippedActors.Add(SpawnedEquipActor);
    
    if(USceneCaptureComponent2D* CaptureComp = Cast<USceneCaptureComponent2D>(GetOwner()->GetComponentByClass(USceneCaptureComponent2D::StaticClass()));IsValid(CaptureComp))
    {
        CaptureComp->ShowOnlyActorComponents(SpawnedEquipActor);
    }
}

void UInv_EquipmentComponent::OnItemUnequipped(UInv_InventoryItem* UnequippedItem)
{
    if (!IsValid(UnequippedItem)) return;
    if (!OwningPlayerController->HasAuthority()) return;

    // 아이템 매니페스트에서 장비 프래그먼트 가져오기
    FInv_ItemManifest& ItemManifest = UnequippedItem->GetItemManifestMutable();
    FInv_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_EquipmentFragment>();
    if (!EquipmentFragment) return;

    // 장비 해제 로직 실행 및 액터 제거
    if (!bIsProxy)
    {
        EquipmentFragment->OnUnequip(OwningPlayerController.Get());
    }
    RemoveEquippedActor(EquipmentFragment->GetEquipmentType());
}

void UInv_EquipmentComponent::OnPossessedPawnChange(APawn* OldPawn, APawn* NewPawn)
{
    // 새로운 Pawn의 스켈레탈 메시 참조 업데이트
    if (ACharacter* OwnerCharacter = Cast<ACharacter>(NewPawn); IsValid(OwnerCharacter))
    {
        OwningSkeletalMesh = OwnerCharacter->GetMesh();
    }
    InitInventoryComponent();
}


