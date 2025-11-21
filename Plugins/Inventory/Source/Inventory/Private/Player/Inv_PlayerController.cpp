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
	// 아이템 감지를 위해 틱 활성화
	PrimaryActorTick.bCanEverTick = true;

	// 라인 트레이스 길이 (언리얼 단위: cm)
	TraceLength = 500.f;
}

void AInv_PlayerController::ToggleInventoryMenu()
{
	if (!InventoryComponent.IsValid()) return;

	// 인벤토리 컴포넌트의 메뉴 토글 호출
	InventoryComponent->ToggleInventoryMenu();

	// 인벤토리 메뉴가 열리면 HUD 숨기기, 닫히면 다시 표시
	if (InventoryComponent->IsMenuOpen())
	{
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		HUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void AInv_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input 서브시스템 가져오기 및 매핑 컨텍스트 추가
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (IsValid(Subsystem))
	{
		Subsystem->AddMappingContext(DefaultIMC, 0);
	}

	// 인벤토리 컴포넌트 찾기
	InventoryComponent = FindComponentByClass<UInv_InventoryComponent>();

	// HUD 위젯 생성 및 추가
	CreateHUDWidget();
}

void AInv_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Enhanced Input 컴포넌트 가져오기
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	// 상호작용 액션 바인딩 (아이템 줍기)
	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ThisClass::PrimaryInteract);

	// 인벤토리 토글 액션 바인딩
	EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ThisClass::ToggleInventoryMenu);
}

void AInv_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 매 프레임마다 화면 중앙에서 아이템 감지
	TraceForItem();
}

void AInv_PlayerController::PrimaryInteract()
{
	// 현재 감지된 아이템이 없으면 종료
	if (!CurrentItemActor.IsValid()) return;

	// 아이템 컴포넌트 가져오기
	UInv_ItemComponent* ItemComponent = CurrentItemActor->FindComponentByClass<UInv_ItemComponent>();
	if (!IsValid(ItemComponent) || !InventoryComponent.IsValid()) return;

	// 인벤토리에 아이템 추가 시도
	InventoryComponent->TryAddItem(ItemComponent);
}

void AInv_PlayerController::CreateHUDWidget()
{
	// 로컬 플레이어인 경우에만 HUD 생성
	if (!IsLocalPlayerController()) return;

	// HUD 위젯 생성 및 뷰포트에 추가
	HUDWidget = CreateWidget<UInv_HUDWidget>(this, HUDWidgetClass);

	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

void AInv_PlayerController::TraceForItem()
{
	// 게임 엔진 및 뷰포트 유효성 검사
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;

	// 뷰포트 중앙 좌표 계산
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter {ViewportSize / 2.f};

	// 스크린 좌표를 월드 공간 좌표로 변환
	FVector TraceStart;
	FVector Forward;

	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCenter, TraceStart, Forward)) return;

	// 라인 트레이스 실행
	const FVector TraceEnd {TraceStart + Forward * TraceLength};
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ItemTraceChannel);

	// 이전 프레임의 아이템과 현재 아이템 업데이트
	LastItemActor = CurrentItemActor;
	CurrentItemActor = HitResult.GetActor();

	// 아이템이 감지되지 않으면 픽업 메시지 숨기기
	if (!CurrentItemActor.IsValid())
	{
		if (IsValid(HUDWidget)) HUDWidget->HidePickupMessage();
	}

	// 같은 아이템이면 더 이상 처리할 필요 없음
	if (LastItemActor == CurrentItemActor) return;

	// 새로운 아이템 감지 시 하이라이트 및 픽업 메시지 표시
	if(CurrentItemActor.IsValid())
	{
		// 하이라이트 인터페이스를 구현한 경우 하이라이트 활성화
		if (UActorComponent* Highlightable = CurrentItemActor->FindComponentByInterface(UInv_Highlightable::StaticClass()); IsValid(Highlightable))
		{
			IInv_Highlightable::Execute_Highlight(Highlightable);
		}

		// 아이템 컴포넌트에서 픽업 메시지 가져와서 HUD에 표시
		UInv_ItemComponent* ItemComponent = CurrentItemActor->FindComponentByClass<UInv_ItemComponent>();
		if (!IsValid(ItemComponent)) return;

		if (IsValid(HUDWidget)) HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
	}

	// 이전 아이템의 하이라이트 비활성화
	if (LastItemActor.IsValid())
	{
		if (UActorComponent* Highlightable = LastItemActor->FindComponentByInterface(UInv_Highlightable::StaticClass()); IsValid(Highlightable))
		{
			IInv_Highlightable::Execute_UnHighlight(Highlightable);
		}
	}
}
