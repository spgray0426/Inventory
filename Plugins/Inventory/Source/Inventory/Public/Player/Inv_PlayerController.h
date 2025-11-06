// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Inv_PlayerController.generated.h"

class UInv_InventoryComponent;
class UInv_HUDWidget;
class UInputAction;
class UInputMappingContext;

/**
 * 커스텀 플레이어 컨트롤러
 * 인벤토리 시스템과 연동되어 아이템 상호작용 및 UI 관리를 담당합니다
 * 입력 매핑 컨텍스트, HUD 위젯 생성, 아이템 감지 레이 트레이스를 처리합니다
 */
UCLASS()
class INVENTORY_API AInv_PlayerController : public APlayerController
{
	GENERATED_BODY()

	
public:
	/** 기본 생성자 */
	AInv_PlayerController();

	/**
	 * 인벤토리 메뉴를 열거나 닫습니다
	 * 블루프린트에서 호출 가능합니다
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventoryMenu();

protected:
	/**
	 * 플레이 시작 시 호출됩니다
	 * HUD 위젯을 생성하고 인벤토리 컴포넌트를 초기화합니다
	 */
	virtual void BeginPlay() override;

	/**
	 * 입력 컴포넌트를 설정합니다
	 * 입력 액션을 바인딩합니다
	 */
	virtual void SetupInputComponent() override;

	/**
	 * 매 프레임 호출됩니다
	 * 아이템 감지를 위한 레이 트레이스를 수행합니다
	 * @param DeltaTime 이전 프레임으로부터의 경과 시간
	 */
	virtual void Tick(float DeltaTime) override;

private:

	/**
	 * 기본 상호작용을 처리합니다
	 * 아이템을 집어들거나 다른 상호작용을 수행합니다
	 */
	void PrimaryInteract();

	/**
	 * HUD 위젯을 생성하고 뷰포트에 추가합니다
	 */
	void CreateHUDWidget();

	/**
	 * 아이템 감지를 위한 레이 트레이스를 수행합니다
	 * 플레이어가 바라보는 방향으로 트레이스를 실행하여 아이템을 감지합니다
	 */
	void TraceForItem();

	/** 인벤토리 컴포넌트에 대한 약한 참조 */
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;

	/** 기본 입력 매핑 컨텍스트 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputMappingContext> DefaultIMC;

	/** 기본 상호작용 입력 액션 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractAction;

	/** 인벤토리 토글 입력 액션 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> ToggleInventoryAction;

	/** HUD 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInv_HUDWidget> HUDWidgetClass;

	/** 생성된 HUD 위젯 인스턴스 */
	UPROPERTY()
	TObjectPtr<UInv_HUDWidget> HUDWidget;

	/** 레이 트레이스 길이 (감지 거리) */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	double TraceLength;

	/** 아이템 트레이스에 사용할 충돌 채널 */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TEnumAsByte<ECollisionChannel> ItemTraceChannel;

	/** 현재 트레이스로 감지된 아이템 액터에 대한 약한 참조 */
	TWeakObjectPtr<AActor> CurrentItemActor;

	/** 이전 프레임에 감지된 아이템 액터에 대한 약한 참조 */
	TWeakObjectPtr<AActor> LastItemActor;
};
