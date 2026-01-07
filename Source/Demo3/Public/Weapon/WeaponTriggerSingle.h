#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponTriggerBase.h"
#include "WeaponTriggerSingle.generated.h"

/**
 * @brief 单发开火模式
 *
 * 行为：
 * - 按下开火键时立即开火一次
 * - 松开开火键时无操作
 */
UCLASS()
class DEMO3_API UWeaponTriggerSingle : public UWeaponTriggerBase
{
    GENERATED_BODY()

public:
    virtual void OnFirePressed() override;
    virtual void OnFireReleased() override;
    virtual bool RequiresTick() const override;
};
