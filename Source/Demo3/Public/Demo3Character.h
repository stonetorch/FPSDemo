// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Weapon/WeaponSystemComponent.h"
#include "Demo3Character.generated.h"

class ADemo3PlayerState;
class UCombatComponent;

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ADemo3Character : public ACharacter
{
	GENERATED_BODY()

public:
	/** 获取PlayerState */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player State")
	ADemo3PlayerState* GetDemo3PlayerState() const;
	
	UFUNCTION(BlueprintPure)
	UWeaponSystemComponent* GetWeaponSystemComponent();

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;
	/** 3rd person view (body; seen by others) */
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Mesh)
	USkeletalMeshComponent* Mesh3P;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	
public:
	ADemo3Character();
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	/** 最大血量 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health, Replicated)
	float MaxHealth;

	/** 当前血量 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	/** 是否已死亡 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, ReplicatedUsing = OnRep_IsDead)
	bool bIsDead;

	/** 获取最大血量 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Health)
	float GetMaxHealth() const { return MaxHealth; }

	/** 获取当前血量 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Health)
	float GetCurrentHealth() const { return CurrentHealth; }

	/** 获取是否已死亡 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Health)
	bool GetIsDead() const { return bIsDead; }

	/** 获取血量百分比 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Health)
	float GetHealthPercent() const;

	/** 
	 * @brief TakeDamage 方法
	 * @param DamageAmount 伤害值
	 * @param DamageEvent 伤害事件
	 * @param EventInstigator 伤害施加者
	 * @param DamageCauser 伤害来源
	 * @return 实际受到的伤害
	 * @note 伤害处理逻辑在客户端和服务器都会进行。这允许客户端进行预测
	 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** 生命值更新事件分发器 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth, float, HealthPercent);

	/** 生命值更新事件
	 * @param CurrentHealth 当前血量
	 * @param MaxHealth 最大血量
	 * @param HealthPercent 血量百分比
	 */
	UPROPERTY(BlueprintAssignable, Category = Health)
	FOnHealthChanged OnHealthChanged;

	/** 受到伤害事件分发器*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamaged, float, DamageAmount, FVector, DamageDirection);

	/** 受到伤害事件
	 * @param DamageAmount 伤害值
	 * @param DamageDirection 伤害方向，为一个单位向量
	 * @note 这个事件在客户端也会发生。
	 */
	UPROPERTY(BlueprintAssignable, Category = Health)
	FOnDamaged OnDamaged;

protected:
	/** 当血量变化时调用（用于通知客户端） */
	UFUNCTION()
	virtual void OnRep_CurrentHealth();

	/** 当死亡状态变化时调用（用于通知客户端） */
	UFUNCTION()
	virtual void OnRep_IsDead();
	
	/** 死亡处理 - 服务器端 */
	UFUNCTION()
	virtual void OnDeath();

	/** 死亡处理 - 客户端RPC，在拥有该角色的客户端执行 */
	UFUNCTION(Client, Reliable)
	void ClientOnDeath();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns Mesh3P subobject **/
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	UPROPERTY(EditAnywhere)
	UWeaponSystemComponent* WeaponSystemComponent;

};

