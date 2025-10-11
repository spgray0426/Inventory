// Copyright Epic Games, Inc. All Rights Reserved.

#include "Inventory.h"

#define LOCTEXT_NAMESPACE "FInventoryModule"

DEFINE_LOG_CATEGORY(LogInventory);

void FInventoryModule::StartupModule()
{
	// 이 코드는 모듈이 메모리에 로드된 후에 실행됩니다. 정확한 타이밍은 모듈별 .uplugin 파일에 지정됩니다.
}

void FInventoryModule::ShutdownModule()
{
	// 이 함수는 모듈을 정리하기 위해 종료 중에 호출될 수 있습니다.  동적 재로드를 지원하는 모듈의 경우
	// 모듈을 언로드하기 전에 이 함수를 호출합니다.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInventoryModule, Inventory)