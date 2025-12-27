#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Curves/CurveFloat.h"
#include "RecoilBase.generated.h"

/**
 * @brief 后坐力逻辑基类（纯本地）
 *
 * RecoilBase 用于描述一把武器的后坐力行为模型，
 * 使用曲线（UCurveFloat）来定义垂直和水平后坐力模式。
 *
 * 重要约束：
 * - 不参与网络复制
 * - 不依赖服务器
 * - 不直接操作 Camera 或 PlayerController
 *
 * 正确使用方式：
 * - Weapon 提供 RecoilBase
 * - Character 或 PlayerController 调用 ComputeRecoil 获取后坐力增量并应用
 */
UCLASS(Abstract, Blueprintable)
class DEMO3_API URecoilBase : public UObject
{
	GENERATED_BODY()

public:
	URecoilBase();

	/**
	 * @brief 在一次射击时计算后坐力增量
	 *
	 * @param OutVerticalRecoilAmount 输出的垂直后坐力增量（Pitch）
	 * @param OutHorizontalRecoilAmount 输出的水平后坐力增量（Yaw）
	 *
	 * 说明：
	 * - 每次调用会递增 RecoilXCoordPerShoot
	 * - 从曲线中采样新值，计算与旧值的差值作为增量
	 * - 更新旧值，为下次计算做准备
	 */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ComputeRecoil(float& OutVerticalRecoilAmount, float& OutHorizontalRecoilAmount);

	/**
	 * @brief 重置后坐力状态
	 *
	 * 调用时机：
	 * - 停止射击
	 * - 换弹
	 * - 切换武器
	 */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void ResetRecoil();

	/**
	 * @brief 更新后坐力衰减
	 *
	 * @param DeltaTime 帧时间间隔
	 *
	 * 说明：
	 * - 在没有连续开火时减少 RecoilXCoordPerShoot
	 * - 应该每帧调用，即使没有射击
	 */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
	void Update(float DeltaTime);

	/**
	 * @brief 垂直后坐力曲线
	 * 曲线的X轴表示连续射击的次数（RecoilXCoordPerShoot），Y轴表示累积的后坐力值
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	UCurveFloat* VerticalRecoilCurve;

	/**
	 * @brief 水平后坐力曲线
	 * 曲线的X轴表示连续射击的次数（RecoilXCoordPerShoot），Y轴表示累积的后坐力值
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	UCurveFloat* HorizontalRecoilCurve;

	/**
	 * @brief 后坐力衰减速度（每秒减少的X坐标值）
	 * 当没有连续开火时，RecoilXCoordPerShoot 会以这个速度衰减
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil", meta = (ClampMin = "0.0"))
	float RecoilDecayRate;

protected:
	/**
	 * @brief 当前连续射击的X坐标（用于采样曲线）
	 * 每次射击时递增 0.1
	 */
	float RecoilXCoordPerShoot;

	/**
	 * @brief 垂直后坐力的新值（从曲线采样得到）
	 */
	float NewVerticalRecoilAmount;

	/**
	 * @brief 垂直后坐力的旧值（上一帧的值）
	 */
	float OldVerticalRecoilAmount;

	/**
	 * @brief 垂直后坐力增量（新值 - 旧值）
	 */
	float VerticalRecoilAmount;

	/**
	 * @brief 水平后坐力的新值（从曲线采样得到）
	 */
	float NewHorizontalRecoilAmount;

	/**
	 * @brief 水平后坐力的旧值（上一帧的值）
	 */
	float OldHorizontalRecoilAmount;

	/**
	 * @brief 水平后坐力增量（新值 - 旧值）
	 */
	float HorizontalRecoilAmount;
};
