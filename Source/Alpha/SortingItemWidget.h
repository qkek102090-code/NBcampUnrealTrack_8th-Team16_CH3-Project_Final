#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SortingItemWidget.generated.h"

class UTextBlock;
class UBorder;

USTRUCT(BlueprintType)
struct FSortingItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(BlueprintReadOnly)
	FText DisplayText;

	// 빨강이면 좌클릭이 정답, 파랑(false)이면 우클릭이 정답입니다.
	UPROPERTY(BlueprintReadOnly)
	bool bIsRed = true;
};

// bWasLeftClick: true=좌클릭으로 분류, false=우클릭으로 분류
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSorted, FName, ItemID, bool, bWasLeftClick);

// 순수 표시용 카드 위젯. 이 컨베이어 벨트 방식 게임에서는 클릭 판정을
// 이 위젯이 아니라 UDocumentSortingGameWidget(부모)이 화면 전체에서 받습니다.
UCLASS()
class ALPHA_API USortingItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 다른 방식의 미니게임에서 이 위젯이 직접 클릭을 받아야 할 때를 위해 남겨뒀습니다.
	// 컨베이어 벨트 방식에서는 이 델리게이트를 안 씁니다.
	UPROPERTY(BlueprintAssignable)
	FOnItemSorted OnItemSorted;

	UFUNCTION(BlueprintCallable, Category = "Minigame")
	void SetItemData(const FSortingItemData& InData);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemLabelText;

	// 빨강/파랑을 색으로도 보여주고 싶으면 Border를 하나 배치해서 이 이름으로 연결하세요.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UBorder> ColorBorder;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY()
	FName CachedItemID;
};
