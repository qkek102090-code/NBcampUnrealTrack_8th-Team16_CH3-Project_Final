#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PriceAxisWidget.generated.h"

class UVerticalBox;

UCLASS()
class ALPHA_API UPriceAxisWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Chart")
	void SetPriceRange(float MinPrice, float MaxPrice, float CurrentPrice);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> AxisList;

	UPROPERTY(EditAnywhere, Category = "Chart")
	int32 NumGridlines = 6;

	UPROPERTY(EditAnywhere, Category = "Chart")
	FLinearColor CurrentPriceBadgeColor = FLinearColor(0.15f, 0.55f, 0.95f);

	UPROPERTY(EditAnywhere, Category = "Chart")
	FLinearColor DividerColor = FLinearColor(1.f, 1.f, 1.f, 0.15f);

	// 구분선의 가로 길이(px)를 직접 정합니다. 이 위젯이 배치되는 컨테이너 크기와
	// 상관없이 항상 이 길이로 고정됩니다.
	UPROPERTY(EditAnywhere, Category = "Chart")
	float DividerLength = 60.f;

	// 구분선을 왼쪽/오른쪽/가운데 중 어디에 붙일지. PriceChart 위에 겹쳐서 쓸 때는
	// 보통 가격 숫자가 있는 오른쪽(Right)에 맞추면 자연스럽습니다.
	UPROPERTY(EditAnywhere, Category = "Chart")
	TEnumAsByte<EHorizontalAlignment> DividerAlignment = HAlign_Left;
};