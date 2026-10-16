#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractPromptWidget.generated.h"

class UTextBlock;

// 화면에 항상 떠 있다가, 상호작용 가능한 물체 범위 안에 들어오면 보이는 안내 문구.
// 3D 월드에 붙이는 위젯 컴포넌트 대신, 그냥 화면 고정(HUD) 텍스트로 처리합니다.
UCLASS()
class ALPHA_API UInteractPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PromptText;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetPromptText(const FText& NewText);
};
