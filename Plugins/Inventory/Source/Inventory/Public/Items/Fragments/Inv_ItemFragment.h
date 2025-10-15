#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Inv_ItemFragment.generated.h"

USTRUCT(BlueprintType)
struct FInv_ItemFragment
{
	GENERATED_BODY()

	FInv_ItemFragment() {};
	FInv_ItemFragment(const FInv_ItemFragment& Other) = default;
	FInv_ItemFragment& operator=(const FInv_ItemFragment& Other) = default;
	FInv_ItemFragment(FInv_ItemFragment&&) = default;
	FInv_ItemFragment& operator=(FInv_ItemFragment&&) = default;
	virtual ~FInv_ItemFragment() {};

	FGameplayTag GetFragmentTag() const { return FrgmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FrgmentTag = Tag; }
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FrgmentTag = FGameplayTag::EmptyTag;
};

USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(const FIntPoint& Size) { GridSize = Size; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(float Padding) { GridPadding = Padding; }
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize {1, 1};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding {0.f};
	
};
