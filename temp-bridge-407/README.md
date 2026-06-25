# TempBridge407

`TempBridge407` 是 STM32F407VET6 温度桥接固件，用于完成：

- 通过 Modbus RTU 读取温度传感器
- 将温度转换为电压模拟量输出
- 将温度转换为 4-20mA 电流输出
- 通过 CAN 上报温度、输出码值和状态
- 主动发送 F103 接收端链路测试帧

## 外设分配

| 功能 | 引脚 | 配置 | 连接 |
| --- | --- | --- | --- |
| USART1_TX | PA9 | 115200-8N1 | Modbus 温度传感器 RX |
| USART1_RX | PA10 | 115200-8N1 | Modbus 温度传感器 TX |
| CAN1_RX | PD0 | AF9 CAN1 | SN65HVD230 RXD |
| CAN1_TX | PD1 | AF9 CAN1 | SN65HVD230 TXD |
| DAC_SYNC | PB12 | GPIO 推挽输出 | DAC8550 SYNC |
| DAC_SCLK | PB13 | GPIO 推挽输出 | DAC8550 SCLK |
| DAC_DIN | PB15 | GPIO 推挽输出 | DAC8550 DIN |
| I2C2_SCL | PB10 | AF4 开漏上拉 | GP8302 SCL |
| I2C2_SDA | PB11 | AF4 开漏上拉 | GP8302 SDA |
| LED1 | PA6 | GPIO 推挽输出 | 电压输出状态 |
| LED2 | PA7 | GPIO 推挽输出 | 电流输出状态 |
| PE3/PE4 | PE3/PE4 | 上拉输入 | 预留数字输入 |

## 温度输入

Modbus 请求帧：

```text
F7 03 10 00 00 08 54 5A
```

含义：

- 从机地址：`0xF7`
- 功能码：`0x03`
- 起始寄存器：`0x1000`
- 寄存器数量：`0x0008`
- 串口：USART1，`115200-8N1`
- 单帧接收超时：`180 ms`
- 主循环无额外固定轮询延时

收到有效 Modbus 温度响应后，固件会立即发送一帧 CAN 温度帧；模拟量输出按独立的 `200 ms` 周期刷新。

## 模拟量输出

温度量程：

```text
TEMP_MIN = -40.00℃
TEMP_MAX = 300.00℃
```

### 电压输出：DAC8550

DAC8550 由软件 SPI 驱动：

- `PB12`：SYNC，低有效
- `PB13`：SCLK
- `PB15`：DIN

写入帧：

```text
0x00 + DAC_CODE[15:8] + DAC_CODE[7:0]
```

每次输出刷新重复写入 3 次。DAC 码由滤波后的温度线性计算得到，并按 DAC8550 二补码输入格式写入；实际电压范围由 DAC 参考电压和后级电压模块决定。

接线注意：

- F407、DAC 模块、传感器、电流模块、测量仪表需要共地
- DAC8550 的 VREF/供电/输出端要按模块说明连接
- 若电压输出接近 0 且电流正常，优先检查 GND、VREF 和 DAC 三根 SPI 线

### 电流输出：GP8302

GP8302 使用 I2C2：

- I2C 地址：`0x58`
- 输出寄存器：`0x02`
- DAC 分辨率：12 位，`0..4095`
- 模块满量程按 `25 mA` 计算

温度到电流的映射：

```text
-40.00℃ -> 4 mA
300.00℃ -> 20 mA
```

代码中的计算：

```text
current_uA = 4000 + (temp - TEMP_MIN) * 16000 / (TEMP_MAX - TEMP_MIN)
gp8302_code = current_uA * 4095 / 25000
```

例子：

| 温度 | 比例 | 目标电流 |
| --- | --- | --- |
| -40℃ | 0% | 4.0 mA |
| 28℃ | 约 20% | 约 7.2 mA |
| 300℃ | 100% | 20.0 mA |

实际电流精度需要结合模块误差、负载电阻、电源和标定情况确认。

## CAN 帧

| ID | 方向 | 内容 |
| --- | --- | --- |
| `0x320` | F407 -> 总线 | 心跳 |
| `0x321` | F407 -> 总线 | 温度和模拟量状态 |
| `0x322` | F407 -> 总线 | 诊断信息 |
| `0x700` | F407 -> F103 | 链路测试请求 |
| `0x701` | F103 -> F407 | 链路测试回复 |

温度帧 `0x321`：

```text
DATA[0..3]  int32，大端，温度，单位 0.01℃
DATA[4..5]  uint16，大端，DAC 码
DATA[6]     current_uA / 100
DATA[7]     AppStatus
```

## 构建

```sh
cmake --preset Debug
cmake --build --preset Debug
```

生成文件：

- `build/Debug/TempBridge407.elf`
- `build/Debug/TempBridge407.hex`
- `build/Debug/TempBridge407.bin`
