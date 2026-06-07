# STM32U5 姿态异常监护手环

基于 STM32U5 的离线姿态监护可穿戴设备。采用六轴传感器实时捕捉佩戴者姿态数据，在 MCU 端部署轻量化 AI 模型，实现跌倒动作的毫秒级本地识别与即时震动报警。裸机开发，无需联网，低功耗可长时间佩戴。

## 硬件

| 器件 | 型号 | 说明 |
|------|------|------|
| 开发板 | NUCLEO-U575ZI-Q | STM32U575ZIT6Q, Cortex-M33 |
| 六轴 IMU | MPU6050 / LSM6DSO / ICM-42688 | I2C 400kHz |
| 震动马达 | 微型偏心轮电机 | PWM 驱动 (TIM) |
| RGB LED | 共阴三色 LED | GPIO: 绿/黄/红 |
| 按键 | 轻触开关 | 长按解除告警 |
| 电池 | 锂聚合物 3.7V | 经 LDO 稳压至 3.3V |

## 软件架构

```
STM32U5/
├── Core/
│   ├── Inc/
│   │   ├── ahrs.h          # Mahony AHRS 姿态解算
│   │   ├── fall_detect.h   # 跌倒检测状态机
│   │   └── params.h        # 可调参数（阈值、采样率）
│   └── Src/
│       ├── ahrs.c          # 四元数姿态解算实现
│       └── fall_detect.c   # 五状态检测机实现
├── src/                    # 驱动层（开发中）
│   ├── drivers/
│   │   ├── imu/            # IMU 传感器驱动
│   │   ├── vibration/      # 震动马达驱动
│   │   └── led/            # LED 指示驱动
│   ├── algorithms/
│   │   ├── attitude/       # 姿态解算（与 Core/ 中的一致）
│   │   └── detection/      # 异常检测
│   └── system/
│       └── power_mgmt.c    # 低功耗管理
├── include/
│   └── config.h            # 全局配置
├── lib/                    # CMSIS / HAL 库
├── Makefile                # 编译脚本
└── README.md
```

## 姿态解算

Mahony AHRS 滤波器，9-DOF（加速度 + 陀螺仪 + 可选磁力计）：

- **输入**: 加速度 (m/s²)、陀螺仪 (rad/s)、磁力计 (μT, 可选)
- **输出**: 四元数 → roll / pitch / yaw 欧拉角
- **采样率**: 100Hz
- **磁力计**: `AHRS_USE_MAG` 编译开关控制

## 跌倒检测

五状态状态机：

```
NORMAL → FREE_FALL → IMPACT → POST_IMPACT → FALL_CONFIRMED
```

| 阶段 | 条件 | 时间门控 |
|------|------|---------|
| 自由落体 | \|a\| < 0.6g | > 100ms |
| 冲击 | \|a\| > 3.0g | — |
| 落地后姿态 | trunk_tilt > 60° | 3s 窗口内 |
| 倾斜预警 | tilt > 45° | > 3s（独立触发） |

状态切换含 5 帧防抖。

## 编译与烧录

```bash
# 编译
make

# 烧录（STLINK-V3）
make flash

# 调试
make debug
```

工具链: `arm-none-eabi-gcc` + `openocd`

## 关键参数

| 参数 | 值 |
|------|-----|
| AHRS 采样率 | 100 Hz |
| 陀螺仪量程 | ±2000°/s |
| 加速度量程 | ±8g |
| I2C 速率 | 400kHz |
| 主频 | 160MHz (PLL) |
| 低功耗目标 | STOP 模式 μA 级 |

## 项目状态

| Issue | 标题 | 状态 |
|-------|------|------|
| WUT-10 | 工程骨架搭建 | 🔄 进行中 |
| WUT-11 | 六轴传感器驱动 | ✅ 已审查 |
| WUT-12 | 姿态解算与跌倒检测 | ✅ 已审查 |
| WUT-13 | 震动告警与低功耗 | ✅ 已审查 |
| WUT-14 | 硬件设备清单 | 📋 待办 |

## 许可证

MIT
