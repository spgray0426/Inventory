// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Inv_ItemComponent.generated.h"

/**
 * 월드에 배치된 주울 수 있는 아이템에 부착되는 액터 컴포넌트
 * 아이템의 매니페스트를 포함하고 있으며, 멀티플레이어에서 아이템 픽업을 관리합니다
 * 이 컴포넌트가 부착된 액터를 플레이어가 상호작용하여 인벤토리에 추가할 수 있습니다
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** 기본 생성자 */
	UInv_ItemComponent();

	/**
	 * 네트워크 복제를 위한 속성들을 등록합니다
	 * @param OutLifetimeProps 복제 속성을 저장할 배열
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * 아이템 매니페스트를 초기화합니다
	 * 드롭된 아이템이나 동적으로 생성된 픽업 액터에 아이템 정보를 설정할 때 사용됩니다
	 * @param CopyOfManifest 설정할 아이템 매니페스트의 복사본
	 */
	void InitItemManifest(FInv_ItemManifest CopyOfManifest);

	/**
	 * 아이템을 주웠을 때 표시할 메시지를 가져옵니다
	 * @return 픽업 메시지 문자열
	 */
	FString GetPickupMessage() const { return PickupMessage; }

	/**
	 * 이 컴포넌트가 나타내는 아이템의 매니페스트를 가져옵니다
	 * @return 아이템 매니페스트 (프래그먼트, 카테고리, 타입 정보 포함)
	 */
	FInv_ItemManifest GetItemManifest() const { return ItemManifest; }
	
	FInv_ItemManifest& GetItemManifestMutable() { return ItemManifest; }
	/**
	 * 아이템이 주워졌을 때 호출되는 함수
	 * OnPickedUp 블루프린트 이벤트를 발생시킵니다
	 */
	void PickedUp();

protected:

	/**
	 * 아이템이 주워졌을 때 호출되는 블루프린트 구현 가능 이벤트
	 * 블루프린트에서 이 이벤트를 구현하여 커스텀 픽업 효과를 추가할 수 있습니다
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnPickedUp();

private:

	/** 이 아이템의 매니페스트 데이터 (복제됨) */
	UPROPERTY(Replicated, EditAnywhere, Category = "Inventory")
	FInv_ItemManifest ItemManifest;

	/** 아이템을 주웠을 때 표시될 메시지 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FString PickupMessage;
};
