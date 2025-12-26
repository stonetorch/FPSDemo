#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RecoilBase.generated.h"

/**
 * @brief 后坐力逻辑基类（纯本地）
 *
 * RecoilBase 用于描述一把武器的后坐力行为模型，
 * 例如：
 * - 每次射击的视角偏移
 * - 连射时的累积曲线
 * - 随机扰动
 *
 * 重要约束：
 * - 不参与网络复制
 * - 不依赖服务器
 * - 不直接操作 Camera
 *
 * 正确使用方式：
 * - Weapon 提供 RecoilBase
 * - PlayerController 读取数据并应用表现
 */
UCLASS(Abstract, Blueprintable)
class DEMO3_API URecoilBase : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief 在一次射击时计算后坐力
	 *
	 * @param ShotIndex 当前连续射击的次数（从 0 或 1 开始）
	 * @return 视角偏移量（Pitch / Yaw）
	 *
	 * 实现建议：
	 * - 可以使用固定数组
	 * - 可以使用曲线
	 * - 可以叠加随机扰动
	 */
	// virtual FRotator ComputeRecoil(int32 ShotIndex) const;

	/**
	 * @brief 重置后坐力状态
	 *
	 * 调用时机：
	 * - 停止射击
	 * - 换弹
	 * - 切换武器
	 */
	// virtual void ResetRecoil();

protected:
	/**
	 * @brief 最大连续射击次数（用于控制数组或曲线索引）
	 */
	UPROPERTY(EditDefaultsOnly)
	int32 MaxShotCount;
};
