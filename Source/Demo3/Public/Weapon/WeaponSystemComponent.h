#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponSystemComponent.generated.h"


class UWeaponTriggerBase;
class URecoilBase;
class AFPSDemoPlayerController;
class AWeaponBase;
/**
 * @brief 武器系统组件（每个玩家一个） \n
 * WeaponSystemComponent 负责管理某个玩家拥有的所有 Weapon 实例，
 * 包括：
 * - 武器背包（仅服务器 + 拥有者同步）
 * - 当前手持武器（全客户端同步）
 * - 武器切换
 * - 开火按键绑定
 * - 客户端下后坐力组件的更新,切换武器时后坐力组件的改变
 *
 * 额外支持的生命周期管理：
 * - DestroyComponent，会自动销毁管理的所有武器，以及解除绑定等操作。可用于角色死亡。
 * 
 * @note 需要由持有它的Character在被PlayerController控制时调用SetPlayerController
 */
UCLASS(ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent))
class DEMO3_API UWeaponSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWeaponSystemComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void SetPlayerController(AFPSDemoPlayerController* PC);
    
    /**
     * @brief 玩家拥有的所有武器实例（背包）
     *
     * 网络语义：
     * - 该数组在服务器与“武器拥有者客户端”之间同步
     * - 背包武器本身应设置 bOnlyRelevantToOwner = true
     */
    UPROPERTY(Replicated)
    TArray<AWeaponBase*> InventoryWeapons;

    /**
     * @brief 将一把武器添加到当前玩家武器系统的库存中,这会修改weaponActor的父Actor和网络归属
     * @param weaponActor 将要添加的武器
     */
    UFUNCTION(BlueprintCallable,Server,Reliable)
    void AddWeapon(AWeaponBase* weaponActor);

    /** 
     * @brief 获取当前手持武器
     * @return 当前手持武器
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    AWeaponBase* GetCurrentWeapon();

    /**
     * @brief 当前手持武器
     *
     * 网络语义：
     * - 该指针对所有客户端复制
     * - 当前手持武器必须是一个“对所有客户端可见”的 Weapon Actor
     *
     * 副效应：
     * - OnRep_CurrentWeapon 中负责处理：
     *   - 上一把武器卸下
     *   - 新武器装备
     */
    UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
    AWeaponBase* CurrentWeapon;

    /**
     * @brief 当前手持武器发生变化时的回调
     *
     * 调用时机：
     * - 服务器修改 CurrentWeapon 后
     * - 客户端接收到复制更新时

     * 注意：
     * - 该函数必须是"幂等"的
     * - 允许被多次调用而不产生副作用叠加
     */
    UFUNCTION()
    void OnRep_CurrentWeapon();

    /* =========================
     * 武器操作接口（供 PlayerController 调用）
     * ========================= */

    /**
     * @brief 尝试开火（客户端入口）
     *
     * 调用者：
     * - 本地 PlayerController
     *
     * 行为：
     * - 本地立即执行后坐力（通过 Weapon 提供的数据）
     * - 调用 Weapon 的 ServerFire RPC
     *
     * 注意：
     * - 不在此函数中生成子弹
     * - 不在此函数中播放公共特效
     */
    void TryFire();

    /**
     * @brief 切换当前手持武器
     */
    void SwitchWeapon(AWeaponBase* NewWeapon);

    /**
     * 向后切换武器（按背包顺序）
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToNextWeapon();

    /**
     * 向前切换武器（按背包顺序）
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToPreviousWeapon();

protected:
    /**
     * @brief 服务器端切换武器的 RPC
     */
    UFUNCTION(Server, Reliable)
    void ServerSwitchWeapon(AWeaponBase* NewWeapon);

    /**
     * @brief 设置网络复制属性
     */
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

private:
    /**
     * @brief 上一把武器（用于 OnRep 回调中获取旧值）
     */
    AWeaponBase* LastWeapon;

    // ============= Player Controller ===========
    UPROPERTY()
    AFPSDemoPlayerController* PlayerController;
    UPROPERTY()
    UWeaponTriggerBase* ActiveWeaponTrigger;
    // ============= Recoil: Internal State ================
    UPROPERTY()
    URecoilBase* ActiveRecoilLogic;
    
#pragma region ActionBind
public:
    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    class UInputMappingContext* FireMappingContext;

    /** Fire Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    class UInputAction* FireAction;
    
    void EnableWeaponInput(AWeaponBase* Weapon);
    void DisableWeaponInput(AWeaponBase* Weapon);
#pragma endregion 
};
