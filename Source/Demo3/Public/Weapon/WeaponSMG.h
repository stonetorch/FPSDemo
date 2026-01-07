#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "WeaponSMG.generated.h"

class ADemo3Projectile;
class USoundBase;
class UAnimMontage;

/**
 * @brief 冲锋枪武器实现类
 *
 * 负责具体的武器实现：
 * - 弹药管理
 * - 投射物生成
 * - 网格体展示
 * - 音效播放
 * - 使用自动连续射击模式
 */
UCLASS()
class DEMO3_API AWeaponSMG : public AWeaponBase
{
    GENERATED_BODY()

public:
    AWeaponSMG();

    /**
     * @brief 弹夹中的弹药数
     */
    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Ammo)
    int32 AmmoInClip;

    /**
     * @brief 武器的网格体组件
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
    USkeletalMeshComponent* WeaponMesh;

    /**
     * @brief 子弹类（用于生成投射物）
     */
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
    TSubclassOf<ADemo3Projectile> ProjectileClass;

    /**
     * @brief 开火音效
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    USoundBase* FireSound;

    /**
     * @brief 开火动画蒙太奇
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    UAnimMontage* FireAnimation;

    /**
     * @brief 枪口偏移（相对于角色位置）
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    FVector MuzzleOffset;

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
    virtual void SetupWeaponMesh() override;
    virtual void DetachWeaponMesh() override;
    virtual bool CanFire() const override;
    virtual void ConsumeAmmo() override;
    virtual void SpawnProjectile(const FRotator& SpawnRotation) override;
    virtual void SpawnProjectileAimingAt(const FVector& TargetLocation) override;
    virtual void PlayFireEffectsLocal() override;
    virtual void PlayFireEffectsMulticast() override;
};
