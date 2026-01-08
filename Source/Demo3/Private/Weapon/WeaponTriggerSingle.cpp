// Fill out your copyright notice in the Description page of Project Settings.

#include "Demo3/Public/Weapon/WeaponTriggerSingle.h"

void UWeaponTriggerSingle::OnFirePressed()
{
    // 按下时立即开火一次
    TriggerFire();
}

void UWeaponTriggerSingle::OnFireReleased()
{
    
}

bool UWeaponTriggerSingle::RequiresTick() const
{
    return Super::RequiresTick();
}
