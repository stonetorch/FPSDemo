#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponTriggerBase.h"
#include "WeaponTriggerAuto.generated.h"

/**
 * @brief 自动连续射击模式
 *
 * 行为：
 * - 按下开火键时开始连续射击
 * - 松开开火键时停止射击
 * - 自动管理两发之间的间隔时间
 */
UCLASS()
class DEMO3_API UWeaponTriggerAuto : public UWeaponTriggerBase
{
    GENERATED_BODY()

public:
    UWeaponTriggerAuto();

    virtual void Initialize(AWeaponBase* InWeapon) override;
    virtual void OnFirePressed() override;
    virtual void OnFireReleased() override;
    virtual void Tick(float DeltaTime) override;
    virtual bool RequiresTick() const override { return true; }

    /**
     * @brief 设置射速（每分钟发射数）
     * @param RoundsPerMinute 每分钟发射数
     */
    void SetFireRate(float RoundsPerMinute);

    /**
     * @brief 获取射速（每分钟发射数）
     * @return 每分钟发射数
     */
    float GetFireRate() const { return FireRate; }

protected:
    /**
     * @brief 射速（每分钟发射数）
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Rate")
    float FireRate;

    /**
     * @brief 两发之间的时间间隔（秒）
     */
    float FireInterval;

    /**
     * @brief 距离上次开火的时间
     */
    float TimeSinceLastFire;

    /**
     * @brief 是否正在开火
     */
    bool bIsFiring;
};
