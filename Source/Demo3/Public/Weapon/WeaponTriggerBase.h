#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponTriggerBase.generated.h"

class AWeaponBase;

/**
 * @brief 武器开火模式
 *
 * 负责处理开火触发逻辑：
 * - 接收玩家按下与松开开火键的信息
 * - 自动记录开火过程中的信息
 * - 在需要的时机调用 WeaponBase 的 Fire 方法
 *
 * 组件由 WeaponBase 持有并管理生命周期，WeaponSystemComponent会记录当前活动的 WeaponTrigger \n
 * 开火模式组件允许由多个武器共有，之后可以支持类似双枪的射击方式。
 * 
 * @note WeaponSystemComponent应当检查RequiredTick()，如果它返回 true，则需要 WeaponSystemComponent由每帧调用 Tick 方法
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class DEMO3_API UWeaponTriggerBase : public UObject
{
    GENERATED_BODY()

public:
    UWeaponTriggerBase();

    /**
     * @brief 初始化触发器
     * @param InWeapon 所属武器
     */
    virtual void Initialize(AWeaponBase* InWeapon);

    /**
     * @brief 玩家按下开火键
     */
    virtual void OnFirePressed() PURE_VIRTUAL(::OnFirePressed);

    /**
     * @brief 玩家松开开火键
     */
    virtual void OnFireReleased() {};

    virtual void Tick(float DeltaTime) {};

    /**
     * @brief 检查是否需要 Tick
     * @return 如果需要 Tick 返回 true
     */
    virtual bool RequiresTick() const {return false;};

protected:
    /**
     * @brief 所属武器
     */
    UPROPERTY()
    AWeaponBase* Weapon;

    /**
     * @brief 调用武器的 Fire 方法
     */
    virtual void TriggerFire();
};
