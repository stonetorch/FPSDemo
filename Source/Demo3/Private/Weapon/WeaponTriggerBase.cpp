// Fill out your copyright notice in the Description page of Project Settings.

#include "Demo3/Public/Weapon/WeaponTriggerBase.h"
#include "Demo3/Public/Weapon/WeaponBase.h"

UWeaponTriggerBase::UWeaponTriggerBase()
{
    Weapon = nullptr;
}

void UWeaponTriggerBase::Initialize(AWeaponBase* InWeapon)
{
    Weapon = InWeapon;
}

void UWeaponTriggerBase::TriggerFire()
{
    if (Weapon)
    {
        Weapon->Fire();
    }
}
