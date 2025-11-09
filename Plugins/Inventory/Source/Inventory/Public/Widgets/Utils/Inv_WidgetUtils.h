// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Inv_WidgetUtils.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_WidgetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 위젯의 뷰포트상 위치를 반환하는 함수 (블루프린트에서도 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static FVector2D GetWidgetPosition(UWidget* Widget);

	// 위젯의 크기를 가져오는 함수 (블루프린트에서도 호출 가능) 
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static FVector2D GetWidgetSize(UWidget* Widget);
	
	// 특정 위치가 경계 영역 내에 있는지 확인하는 함수 (블루프린트에서도 호출 가능)
	// @param BoundaryPos - 경계 영역의 시작 위치
	// @param WidgetSize - 경계 영역의 크기
	// @param MousePos - 확인할 마우스 위치
	// @return - 마우스가 경계 영역 내에 있으면 true, 아니면 false 반환
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static bool IsWithinBounds(const FVector2D& BoundaryPos, const FVector2D& WidgetSize, const FVector2D& MousePos);
	
	// 2D 좌표(Position)를 1D 인덱스로 변환하는 함수
	// @param Position - 변환할 2D 좌표 (x,y)
	// @param Columns - 그리드의 열 수
	// @return - 1차원 배열에서의 인덱스 값
	static int32 GetIndexFromPosition(const FIntPoint& Position, const int32 Columns);

	// 1D 인덱스를 2D 좌표로 변환하는 함수
	// @param Index - 변환할 1차원 배열 인덱스
	// @param Columns - 그리드의 열 수
	// @return - 2D 좌표 (x,y)
	static FIntPoint GetPositionFromIndex(const int32 Index, const int32 Columns);

	/**
	 * 위젯이 화면 경계를 벗어나지 않도록 위치를 보정합니다
	 * 아이템 설명 위젯을 마우스 커서 근처에 표시할 때 화면 밖으로 나가지 않도록 합니다
	 * @param Boundary 화면 경계 크기 (뷰포트 크기)
	 * @param WidgetSize 배치할 위젯의 크기
	 * @param MousePos 마우스 커서 위치
	 * @return 화면 경계 내로 보정된 위젯 위치
	 */
	static FVector2D GetClampedWidgetPosition(const FVector2D& Boundary, const FVector2D& WidgetSize, const FVector2D& MousePos);
};
