#pragma once

#include "CoreMinimal.h"
#include "WeaponRifle.h"
#include "Weapon/WeaponBase.h"
#include "WeaponSMG.generated.h"

class ADemo3Projectile;
class USoundBase;
class UAnimMontage;

/**
 * @brief 冲锋枪武器实现类
 */
UCLASS()
class DEMO3_API AWeaponSMG : public AWeaponRifle
{
    GENERATED_BODY()

public:
    AWeaponSMG();

    /**
     * @brief 伤害值
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
    float DamageAmount;

    /**
     * @brief 射线效果粒子系统（用于显示可见的射线）
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    class UNiagaraSystem* BeamEffect;

    /**
     * @brief Tracer 频率：每 N 发子弹显示一次光束效果（例如：3 表示每 3 发显示一次）
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects, meta = (ClampMin = "1"))
    int32 TracerFrequency;

    /** 
     * @brief 最大射程
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    float MaxRange;

private:
    /**
     * @brief 发射计数器（用于 Tracer 频率控制）
     */
    int32 FireCount;

protected:
    /**
     * @brief 网络多播：在所有客户端显示射线效果
     * @param StartLocation 射线起始位置
     * @param EndLocation 射线结束位置
     */
    UFUNCTION(NetMulticast, Reliable)
    void NetMulticastSpawnBeamEffect(const FVector& StartLocation, const FVector& EndLocation);

protected:
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    // 重写基类的 virtual 方法
    virtual void SpawnProjectile(const FRotator& SpawnRotation) override;
    virtual void SpawnProjectileAimingAt(const FVector& TargetLocation) override;
    virtual void PlayFireEffectsLocal() override;
    virtual void PlayFireEffectsMulticast() override;
};
