#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSDemoPlayerController.generated.h"

class ADemo3PlayerState;
class ADemo3Character;
class UCombatComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class DEMO3_API AFPSDemoPlayerController : public APlayerController
{
	GENERATED_BODY()
	virtual void OnRep_PlayerState() override;

	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintImplementableEvent)
	void HandlePlayerStateChange(ADemo3PlayerState* ps);
	/** 切换到下一把武器 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchToNextWeapon();

	/** 切换到上一把武器 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchToPreviousWeapon();

	// UPROPERTY(EditAnywhere)
	// TSubclassOf<UUserWidget> UMG_Overlay_Class;

private:
	// UPROPERTY()
	// UUserWidget* Widget;
};
