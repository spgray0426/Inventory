// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

/**
 * 인벤토리 플러그인 로그 카테고리
 * 플러그인 내의 모든 로깅에 사용됩니다
 */
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

/**
 * 인벤토리 플러그인 모듈 클래스
 * 플러그인의 초기화 및 종료를 담당합니다
 * IModuleInterface를 구현하여 언리얼 엔진의 모듈 시스템과 통합됩니다
 */
class FInventoryModule : public IModuleInterface
{
public:

	/**
	 * 모듈 시작 시 호출됩니다
	 * 플러그인 초기화 작업을 수행합니다
	 */
	virtual void StartupModule() override;

	/**
	 * 모듈 종료 시 호출됩니다
	 * 플러그인 정리 작업을 수행합니다
	 */
	virtual void ShutdownModule() override;
};
