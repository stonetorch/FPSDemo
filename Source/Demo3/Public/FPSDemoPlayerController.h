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

	/**
	 * @brief 结束游戏。
	 */
	UFUNCTION(BlueprintCallable,Client,Reliable)
	void FinishGame();
	
	/**
	 * @brief 在客户端执行的结束游戏逻辑，不应该直接调用
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Gameplay")
	void ClientFinishGame();

    // UPROPERTY(EditAnywhere)
	// TSubclassOf<UUserWidget> UMG_Overlay_Class;

private:
	// UPROPERTY()
	// UUserWidget* Widget;
};
