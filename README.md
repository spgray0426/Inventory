# Inventory System Plugin

언리얼 엔진 5.5용 고급 인벤토리 시스템 플러그인입니다. 컴포지션 패턴 기반의 유연한 아이템 시스템과 그리드 기반 공간 인벤토리를 제공합니다.

## 주요 특징

- **컴포지션 기반 아이템 시스템**: 프래그먼트를 조합하여 다양한 아이템 생성
- **GameplayTag 기반 UI 자동화**: 프래그먼트와 위젯이 자동으로 매칭되는 동적 UI 시스템
- **그리드 기반 공간 인벤토리**: Resident Evil 스타일의 테트리스형 인벤토리
- **멀티플레이어 지원**: Fast Array Serialization을 통한 효율적인 네트워크 복제
- **플러그인 아키텍처**: 독립적인 플러그인으로 다른 프로젝트에 재사용 가능

## 시스템 아키텍처

### 1. 아이템 프래그먼트 시스템

아이템은 여러 프래그먼트의 조합으로 구성됩니다. 각 프래그먼트는 GameplayTag로 식별되Q며, 특정 기능이나 속성을 정의합니다.

#### 핵심 프래그먼트 타입

**기본 프래그먼트**
- `FInv_GridFragment`: 그리드 크기 및 패딩 정의
- `FInv_ImageFragment`: 아이콘 이미지 및 크기
- `FInv_StackableFragment`: 스택 가능 여부 및 최대 스택 수
- `FInv_TextFragment`: 텍스트 정보 (이름, 설명 등)

**UI 표시 프래그먼트** (`FInv_InventoryItemFragment` 파생)
- `FInv_LabeledNumberFragment`: 라벨과 숫자 값 (스탯, 가격 등)
  - 랜덤 값 생성 및 고정 (Manifest 패턴)
  - 소수점 자릿수 제어
  - 라벨/값 개별 표시 제어

**소비형 아이템 프래그먼트**
- `FInv_ConsumeModifier`: 개별 소비 효과 정의 (체력 회복, 마나 회복 등)
- `FInv_ConsumableFragment`: 여러 수정자를 포함하는 컴포지트 구조
- `FInv_HealthPotionFragment`: 체력 회복 포션 구현
- `FInv_ManaPotionFragment`: 마나 회복 포션 구현

#### GameplayTag 목록

```cpp
namespace FragmentTags
{
    GridFragment              // 그리드 차원
    IconFragment              // 아이콘 이미지
    StackableFragment         // 스택 가능
    ConsumableFragment        // 소비 가능
    ItemNameFragment          // 아이템 이름
    PrimaryStatFragment       // 주요 스탯
    ItemTypeFragment          // 아이템 타입
    FlavorTextFragment        // 플레이버 텍스트
    SellValueFragment         // 판매 가격
    RequiredLevelFragment     // 요구 레벨

    namespace StatMod
    {
        StatMod_1             // 첫 번째 스탯 수정자
        StatMod_2             // 두 번째 스탯 수정자
        StatMod_3             // 세 번째 스탯 수정자
    }
}
```

### 2. 컴포지트 위젯 시스템

GameplayTag를 통해 프래그먼트와 UI 위젯이 자동으로 매칭되는 동적 UI 시스템입니다.

#### 위젯 계층 구조

```
UInv_CompositeBase (Abstract)
├─ UInv_Composite           // 컨테이너 위젯 (다른 위젯 포함)
└─ UInv_Leaf                // 리프 위젯 (실제 콘텐츠 표시)
   ├─ UInv_Leaf_Text        // 텍스트 표시
   ├─ UInv_Leaf_Image       // 이미지 표시
   └─ UInv_Leaf_LabeledValue // 라벨+값 표시
```

#### 동작 원리

1. **Assimilate (동화)**: 프래그먼트가 위젯에 데이터 전달
   ```cpp
   // 프래그먼트의 GameplayTag와 위젯의 Tag가 일치하면
   Fragment->Assimilate(Widget);
   // 위젯이 확장(Expand)되고 데이터가 설정됨
   ```

2. **Expand/Collapse**: 동적 위젯 표시/숨김
   - 프래그먼트가 있으면 Expand (표시)
   - 프래그먼트가 없으면 Collapse (숨김)

### 3. 인벤토리 관리 시스템

#### 컴포넌트 아키텍처

**UInv_InventoryComponent**
- 서버 권한 인벤토리 작업
- Fast Array를 통한 자동 복제
- 델리게이트: `OnItemAdded`, `OnItemRemoved`, `NoRoomInInventory`

**UInv_ItemComponent**
- 월드의 픽업 가능 아이템에 부착
- `FInv_ItemManifest` 포함 (아이템 정의)
- 멀티플레이어 복제 지원

#### Fast Array 복제

```cpp
FInv_InventoryFastArray    // Fast Array 구현
FInv_InventoryEntry        // 개별 복제 엔트리
```

멀티플레이어에서 효율적인 델타 직렬화를 제공합니다.

### 4. 그리드 인벤토리 UI

#### 위젯 구조

**UInv_SpatialInventory**
- 탭 기반 카테고리 전환 (Equippables/Consumables/Craftables)
- 여러 그리드 관리

**UInv_InventoryGrid**
- 그리드 기반 시각화
- `UInv_GridSlot`: 개별 슬롯
- `UInv_SlottedItem`: 슬롯에 배치된 아이템
- 카테고리 필터링 (`EInv_ItemCategory`)

#### 그리드 수학

**UInv_InventoryStatics::ForEach2D()**
```cpp
// 2D 그리드 반복을 위한 템플릿 함수
// 선형 인덱스 ↔ 2D 좌표 변환
ForEach2D(GridSize, StartPos, ItemSize, [](int32 X, int32 Y, int32 Index) {
    // 각 셀에 대한 작업
});
```

**슬롯 가용성 확인**
```cpp
FInv_SlotAvailability         // 단일 슬롯 용량
FInv_SlotAvailabilityResult   // 전체 결과 (스택 지원 포함)
```

### 5. 아이템 호버 및 설명 시스템

#### UInv_ItemDescription

컴포지트 패턴을 활용한 아이템 설명 위젯:
- 아이템의 모든 프래그먼트를 순회
- 각 프래그먼트가 매칭되는 위젯에 데이터 동화
- 동적으로 위젯 확장/축소

#### 구현된 설명 위젯

- `WBP_Inv_Leaf_ItemName`: 아이템 이름
- `WBP_Inv_Leaf_ItemType`: 아이템 타입
- `WBP_Inv_Leaf_Icon`: 아이콘 이미지
- `WBP_Inv_Leaf_FlavorText`: 플레이버 텍스트
- `WBP_Inv_LabeledValue_PrimaryStat`: 주요 스탯
- `WBP_Inv_LabeledValue_StatMod`: 스탯 수정자
- `WBP_Inv_LabeledValue_SellValue`: 판매 가격
- `WBP_Inv_LabeledValue_ReqLevel`: 요구 레벨

## 파일 구조

```
Plugins/Inventory/
├── Content/
│   ├── Items/                         # 아이템 블루프린트
│   │   ├── BP_Inv_Potion_Small_Blue.uasset
│   │   └── BP_Inv_Potion_Small_Red.uasset
│   └── Widgets/
│       ├── ItemDescription/           # 아이템 설명 위젯
│       └── Inventory/                 # 인벤토리 UI
├── Source/Inventory/
│   ├── Public/
│   │   ├── Items/
│   │   │   ├── Fragments/            # 프래그먼트 정의
│   │   │   │   ├── Inv_ItemFragment.h
│   │   │   │   └── Inv_FragmentTags.h
│   │   │   └── Manifest/             # 아이템 매니페스트
│   │   ├── InventoryManagement/
│   │   │   ├── FastArray/            # 복제 시스템
│   │   │   ├── Components/           # 컴포넌트
│   │   │   └── Utils/                # 유틸리티
│   │   └── Widgets/
│   │       ├── Composite/            # 컴포지트 위젯
│   │       └── ItemDescription/      # 설명 위젯
│   └── Private/                       # 구현 파일
└── Inventory.uplugin                  # 플러그인 디스크립터
```

## 사용 방법

### 빌드

1. **언리얼 에디터**
   ```
   InventoryProject.uproject를 UE 5.5에서 열기
   ```

2. **Visual Studio**
   ```
   프로젝트 우클릭 → Generate Visual Studio project files
   .sln 파일 열어서 빌드
   ```

3. **Rider**
   ```
   .uproject 파일을 Rider에서 직접 열기
   ```

### 새 아이템 생성

1. **아이템 블루프린트 생성**
   - `BP_Inv_Item` 상속
   - `ItemComponent` 추가

2. **프래그먼트 구성**
   ```cpp
   ItemManifest.Fragments 배열에 프래그먼트 추가:
   - GridFragment (필수): 크기 설정
   - ImageFragment: 아이콘 지정
   - TextFragment: 이름 설정
   - ConsumableFragment: 소비 효과 정의
   - etc.
   ```

3. **GameplayTag 설정**
   - 각 프래그먼트의 FragmentTag 설정
   - Project Settings → GameplayTags에서 태그 등록 확인

### 새 프래그먼트 타입 추가

1. **프래그먼트 구조체 정의**
   ```cpp
   USTRUCT(BlueprintType)
   struct FInv_MyFragment : public FInv_ItemFragment
   {
       GENERATED_BODY()
       // 프로퍼티 및 메서드
   };
   ```

2. **GameplayTag 추가**
   ```cpp
   // Inv_FragmentTags.h
   UE_DECLARE_GAMEPLAY_TAG_EXTERN(MyFragment)

   // Inv_FragmentTags.cpp
   UE_DEFINE_GAMEPLAY_TAG(MyFragment, "FragmentTags.MyFragment")
   ```

3. **UI 위젯 생성 (선택사항)**
   - `UInv_Leaf` 상속 위젯 생성
   - FragmentTag를 MyFragment로 설정
   - Assimilate 메서드에서 데이터 처리

### 테스트

**PIE (Play-in-Editor)**
```
Edit → Editor Preferences → Play → Multiplayer Options
Number of Players = 2 (멀티플레이어 테스트)
```

**테스트 항목**
- [ ] 아이템 픽업/드롭
- [ ] 그리드에 아이템 배치
- [ ] 아이템 스택
- [ ] 아이템 호버 시 설명 표시
- [ ] 소비 아이템 사용
- [ ] 멀티플레이어 복제

## 코딩 표준

이 프로젝트는 **Epic Games 공식 코딩 표준**을 준수합니다:
- 모든 인벤토리 클래스/구조체는 `Inv_` 접두사 사용
- 주석은 Doxygen 스타일
- Public/Private 헤더 분리
- `UPROPERTY`, `UFUNCTION` 매크로에 적절한 지정자 사용

## 개발 패턴

### 컴포지션 패턴
아이템은 프래그먼트의 조합으로 구성되어 상속보다 유연한 설계를 제공합니다.

### 컴포지트 패턴
UI 위젯은 트리 구조로 구성되며, 프래그먼트 데이터를 재귀적으로 처리합니다.

### Manifest 패턴
아이템 초기화 시 `Manifest()` 호출로 랜덤 값 생성 등 초기화 작업을 수행합니다.

### Fast Array Serialization
언리얼 엔진의 최적화된 복제 메커니즘을 사용하여 멀티플레이어 성능을 보장합니다.

## 주요 의존성

- **Unreal Engine**: 5.5+
- **GameplayTags**: 프래그먼트 식별
- **UMG**: UI 시스템
- **StructUtils**: `TInstancedStruct` (이종 프래그먼트 저장)

## 라이선스

이 프로젝트는 교육 및 개발 목적으로 제공됩니다.

## 기여

Pull Request 환영합니다. 주요 변경사항은 이슈를 먼저 열어 논의해 주세요.

## 관련 문서

- [CLAUDE.md](CLAUDE.md) - Claude Code를 위한 프로젝트 가이드라인
- [Unreal Engine Documentation](https://docs.unrealengine.com/)
- [Epic Games Coding Standard](https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine)

---

**Created with Unreal Engine 5.5** | **Powered by GameplayTags** | **Built with ❤️**
