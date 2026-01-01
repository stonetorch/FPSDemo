#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "WeaponRifle.generated.h"

class ADemo3Projectile;
class USoundBase;
class UAnimMontage;

/**
 * @brief 步枪武器实现类
 *
 * 负责具体的武器实现：
 * - 弹药管理
 * - 投射物生成
 * - 网格体展示
 * - 音效播放
 */
UCLASS()
class DEMO3_API AWeaponRifle : public AWeaponBase
{
    GENERATED_BODY()

public:
    AWeaponRifle();

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

