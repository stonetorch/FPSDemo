#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponRanged.h"
#include "WeaponRifle.generated.h"

class ADemo3Projectile;
class USoundBase;
class UAnimMontage;

/**
 * @brief 步枪武器实现类
 *
 * 使用 SkeletalMeshComponent 作为武器网格体
 */
UCLASS()
class DEMO3_API AWeaponRifle : public AWeaponRanged
{
    GENERATED_BODY()

public:
    AWeaponRifle();

protected:
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    // 重写基类的 virtual 方法
    virtual void SpawnProjectile(const FRotator& SpawnRotation) override;
};

