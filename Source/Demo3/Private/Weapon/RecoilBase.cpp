// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/RecoilBase.h"

URecoilBase::URecoilBase()
{
	VerticalRecoilCurve = nullptr;
	HorizontalRecoilCurve = nullptr;
	RecoilDecayRate = 1.0f;
	RecoilXCoordPerShoot = 0.0f;
	NewVerticalRecoilAmount = 0.0f;
	OldVerticalRecoilAmount = 0.0f;
	VerticalRecoilAmount = 0.0f;
	NewHorizontalRecoilAmount = 0.0f;
	OldHorizontalRecoilAmount = 0.0f;
	HorizontalRecoilAmount = 0.0f;
}

void URecoilBase::ComputeRecoil(float& OutVerticalRecoilAmount, float& OutHorizontalRecoilAmount)
{
	// 递增X坐标（每次射击增加1）
	RecoilXCoordPerShoot += 1.0f;

	// 计算垂直后坐力
	if (VerticalRecoilCurve)
	{
		// 从曲线中采样新值（需要传x轴进去）
		NewVerticalRecoilAmount = VerticalRecoilCurve->GetFloatValue(RecoilXCoordPerShoot);
		if (RecoilXCoordPerShoot >= 0.99f)
			OldVerticalRecoilAmount = VerticalRecoilCurve->GetFloatValue(RecoilXCoordPerShoot - 1.0f);
		else OldHorizontalRecoilAmount = 0;
		// 计算增量（新值 - 旧值）
		VerticalRecoilAmount = NewVerticalRecoilAmount - OldVerticalRecoilAmount;
	}
	else
	{
		NewVerticalRecoilAmount = 0.0f;
		VerticalRecoilAmount = 0.0f;
	}

	// 计算水平后坐力
	if (HorizontalRecoilCurve)
	{
		// 从曲线中采样新值
		NewHorizontalRecoilAmount = HorizontalRecoilCurve->GetFloatValue(RecoilXCoordPerShoot);
		if (RecoilXCoordPerShoot >= 0.99f)
			OldHorizontalRecoilAmount = HorizontalRecoilCurve->GetFloatValue(RecoilXCoordPerShoot - 1.0f);
		else OldHorizontalRecoilAmount = 0;
		// 计算增量（新值 - 旧值）
		HorizontalRecoilAmount = NewHorizontalRecoilAmount - OldHorizontalRecoilAmount;
	}
	else
	{
		NewHorizontalRecoilAmount = 0.0f;
		HorizontalRecoilAmount = 0.0f;
	}

	// 输出增量值
	OutVerticalRecoilAmount = VerticalRecoilAmount;
	OutHorizontalRecoilAmount = HorizontalRecoilAmount;
}

void URecoilBase::ResetRecoil()
{
	RecoilXCoordPerShoot = 0.0f;
	NewVerticalRecoilAmount = 0.0f;
	OldVerticalRecoilAmount = 0.0f;
	VerticalRecoilAmount = 0.0f;
	NewHorizontalRecoilAmount = 0.0f;
	OldHorizontalRecoilAmount = 0.0f;
	HorizontalRecoilAmount = 0.0f;
}

void URecoilBase::Update(float DeltaTime)
{
	// 在没有连续开火时减少 RecoilXCoordPerShoot
	if (RecoilXCoordPerShoot > 0.0f)
	{
		RecoilXCoordPerShoot = FMath::Max(0.0f, RecoilXCoordPerShoot - RecoilDecayRate * DeltaTime);
	}
}
