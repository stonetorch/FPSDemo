#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponBase.generated.h"

class URecoilBase;
class ADemo3Character;
class ADemo3Projectile;
class USoundBase;
class UAnimMontage;

/**
 * @brief 武器 Actor
 *
 * Weapon 代表一把具体的、实例化存在的武器。
 *
 * 网络复制：
 * - 服务器权威:射击、弹药、状态校验
 * - 其他客户端:Mesh、动画、特效
 * - 持有者客户端逻辑:提供后坐力数据（不执行）
 *
 * 注意：
 * - Weapon 本身参与网络复制
 * - Weapon 是否对其他客户端可见，取决于 bOnlyRelevantToOwner，该状态由WeaponSystemComponent维护
 *
 * 如何使用：
 * - 由WeaponSystem维护具体逻辑，它会在武器被添加到玩家时 **修改该Actor的父Actor** 
 */
UCLASS()
class DEMO3_API AWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AWeaponBase();

    UPROPERTY(EditAnywhere)
    TSubclassOf<URecoilBase> RecoilLogicClass;
    
    
    /**
     * @brief 后坐力逻辑对象（仅本地使用）
     *
     * 说明：
     * - 不参与网络复制
     * - 只在本地持有者客户端使用
     */
    UPROPERTY(Transient)
    URecoilBase* RecoilLogic;

    /**
     * @brief 武器被装备为当前手持武器
     *
     * 调用时机：
     * - WeaponSystem 的 OnRep_CurrentWeapon
     */
    virtual void OnEquipped();

    /**
     * @brief 武器被卸下（切回背包）
     *
     * 调用时机：
     * - CurrentWeapon 被替换
     */
    virtual void OnUnequipped();
    
    /**
     * 获取后坐力组件
     * @return 武器使用的后坐力组件
     */
    URecoilBase* GetRecoil();

    /**
    * @brief 开火逻辑的入口，由PlayerController调用
    * 调用后将：
    * 1. 播放本地效果
    * 2. 调用后坐力组件
    * 3. 使用ServerRPC实现生成子弹的逻辑等
    * 4. NetMulticast开火效果
    */
    UFUNCTION(BlueprintCallable)
    void Fire();

    /**
     * @brief 由非玩家的Actor开火，一般用于AI敌人。
    * 调用后将：
    * 1. 使用ServerRPC实现生成子弹的逻辑等
    * 2. NetMulticast开火效果
     */
    UFUNCTION(BlueprintCallable)
    void FireNonPlayer(const FVector& TargetLocation);

protected:
    /**
     * @brief 设置武器网格体（在装备时调用）
     * 子类应实现具体的网格体附加逻辑
     */
    virtual void SetupWeaponMesh();

    /**
     * @brief 分离武器网格体（在卸下时调用）
     * 子类应实现具体的网格体分离逻辑
     */
    virtual void DetachWeaponMesh();

    /**
     * @brief 检查是否可以开火（弹药检查等）
     * @return 如果可以开火返回 true
     */
    virtual bool CanFire() const;

    /**
     * @brief 消耗弹药
     * 子类应实现具体的弹药消耗逻辑
     */
    virtual void ConsumeAmmo();

    /**
     * @brief 生成投射物
     * 子类应实现具体的投射物生成逻辑
     * @param SpawnRotation 发射方向
     */
    virtual void SpawnProjectile(const FRotator& SpawnRotation);

    /**
     * @brief 生成投射物，瞄准目标位置
     * 子类应实现具体的投射物生成逻辑
     * @param TargetLocation 目标位置
     */
    virtual void SpawnProjectileAimingAt(const FVector& TargetLocation);

    /**
     * @brief 播放本地开火效果（音效、动画等）
     * 在持有者客户端立即播放
     */
    virtual void PlayFireEffectsLocal();

    /**
     * @brief 播放多播开火效果（音效等）
     * 在其他客户端播放
     */
    virtual void PlayFireEffectsMulticast();

protected:
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;
    
private:
    /**
     * @brief 服务器权威的射击 RPC
     * - 校验弹药
     * - 扣除弹药
     * - 生成子弹或执行 Hitscan
     * - 触发公共特效（通过 Multicast）
     * @param ShootRotation 用于指定射击方向的Rotator
     */
    UFUNCTION(Server, Reliable)
    void ServerFire(const FRotator& ShootRotation);

    /**
     * @brief 网络多播：在所有客户端（排除开火者所在的客户端）播放开火特效
     * - 播放音效
     * - 播放动画
     */
    UFUNCTION(NetMulticast, Reliable)
    void NetMulticastFire();
    
    /**
     * @brief 网络多播：在所有客户端（包括开火者所在客户端）播放开火特效。用于 监听服务器 运行的时候同步非玩家（如AI）的开火行为
     * - 播放音效
     * - 播放动画
     */
    UFUNCTION(NetMulticast, Reliable)
    void NetMulticastAllFire();
};
