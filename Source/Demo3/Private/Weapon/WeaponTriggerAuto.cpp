// Fill out your copyright notice in the Description page of Project Settings.

#include "Demo3/Public/Weapon/WeaponTriggerAuto.h"

UWeaponTriggerAuto::UWeaponTriggerAuto()
{
    FireRate = 600.0f; // 默认 600 RPM
    FireInterval = 0.0f;
    TimeSinceLastFire = 0.0f;
    bIsFiring = false;
}

void UWeaponTriggerAuto::Initialize(AWeaponBase* InWeapon)
{
    Super::Initialize(InWeapon);
    
    // 计算开火间隔
    if (FireRate > 0.0f)
    {
        FireInterval = 60.0f / FireRate; // 每分钟60秒 / 每分钟发射数 = 每发间隔（秒）
    }
    else
    {
        FireInterval = 0.1f; // 默认间隔
    }
    
    TimeSinceLastFire = FireInterval; // 初始化为可以立即开火
    bIsFiring = false;
}

void UWeaponTriggerAuto::OnFirePressed()
{
    bIsFiring = true;
    
    // 如果已经可以开火，立即开火一次
    if (TimeSinceLastFire >= FireInterval)
    {
        TriggerFire();
        TimeSinceLastFire = 0.0f;
    }
}

void UWeaponTriggerAuto::OnFireReleased()
{
    bIsFiring = false;
}

void UWeaponTriggerAuto::Tick(float DeltaTime)
{
    // 更新距离上次开火的时间
    TimeSinceLastFire += DeltaTime;
    
    if (!bIsFiring)
    {
        return;
    }

    // 如果达到开火间隔，执行开火
    if (TimeSinceLastFire >= FireInterval)
    {
        TriggerFire();
        TimeSinceLastFire = 0.0f;
    }
}

void UWeaponTriggerAuto::SetFireRate(float RoundsPerMinute)
{
    FireRate = RoundsPerMinute;
    if (FireRate > 0.0f)
    {
        FireInterval = 60.0f / FireRate;
    }
    else
    {
        FireInterval = 0.1f;
    }
}
