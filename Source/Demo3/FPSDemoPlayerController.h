#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSDemoPlayerController.generated.h"

class ADemo3PlayerState;
/**
 * 
 */
UCLASS(Blueprintable)
class DEMO3_API AFPSDemoPlayerController : public APlayerController
{
	GENERATED_BODY()
	virtual void OnRep_PlayerState() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void HandlePlayerStateChange(ADemo3PlayerState* ps);
	// UPROPERTY(EditAnywhere)
	// TSubclassOf<UUserWidget> UMG_Overlay_Class;

private:
	// UPROPERTY()
	// UUserWidget* Widget;
};
