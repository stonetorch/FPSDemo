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

	/** 受到伤害 */
	UFUNCTION(BlueprintCallable, Category = Health)
	void TakeDamage(float DamageAmount);

protected:
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

