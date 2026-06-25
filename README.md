# TempCanSuite

这是一个温度采集、模拟量输出和 CAN 显示联调工程，仓库里包含两个 STM32 固件项目：

- `temp-bridge-407/`：`TempBridge407`，运行在 STM32F407VET6。负责通过 Modbus RTU 读取温度传感器，输出电压/4-20mA 模拟量，并通过 CAN 上报温度和状态。
- `can-oled-display-103/`：`CanOledDisplay103`，运行在 STM32F103C8T6。负责接收 F407 的 CAN 温度帧，在 OLED 上显示温度，并回复 CAN 握手测试帧。

## 系统链路

```text
温度传感器
  -> Modbus RTU / USART1
  -> STM32F407 TempBridge407
  -> DAC8550 电压输出
  -> GP8302 4-20mA 电流输出
  -> CAN1 / SN65HVD230
  -> STM32F103 CanOledDisplay103
  -> SSD1306 OLED 显示
```

## TempBridge407 外设信息

| 功能 | STM32F407 引脚 | 外设/模块 | 说明 |
| --- | --- | --- | --- |
| Modbus_TX | PA9 | USART1_TX | 115200-8N1，连接温度传感器接收端 |
| Modbus_RX | PA10 | USART1_RX | 115200-8N1，接收温度传感器响应 |
| CAN1_RX | PD0 | SN65HVD230 RXD | CAN 500 kbit/s |
| CAN1_TX | PD1 | SN65HVD230 TXD | CAN 500 kbit/s |
| DAC_SYNC | PB12 | DAC8550 SYNC | 软件 SPI，低有效片选 |
| DAC_SCLK | PB13 | DAC8550 SCLK | 软件 SPI 时钟 |
| DAC_DIN | PB15 | DAC8550 DIN | 软件 SPI 数据 |
| I2C2_SCL | PB10 | GP8302 SCL | 4-20mA 电流模块 |
| I2C2_SDA | PB11 | GP8302 SDA | 4-20mA 电流模块 |
| LED1 | PA6 | 电压输出指示 | 正常翻转，故障常亮 |
| LED2 | PA7 | 电流输出指示 | 正常翻转，故障常亮 |
| DI | PE3/PE4 | 预留输入 | 上拉输入 |

F407 使用 HSI 16 MHz。CAN1 配置为 500 kbit/s，当前使用 PD0/PD1。

## Modbus 温度采集

- 串口：USART1，`115200-8N1`
- 从机地址：`0xF7`
- 功能码：`0x03`
- 起始寄存器：`0x1000`
- 读取数量：`8` 个保持寄存器
- 响应帧长度：`21` 字节
- 温度格式：前 4 个数据字节为大端 32 位原始温度，固件换算为 `0.01℃`
- 轮询策略：无额外固定延时，传感器回复一帧就立即处理一帧

## 模拟量输出

温度工程量范围：

```text
-40.00℃ .. 300.00℃
```

### 电压输出

电压通道使用 DAC8550，F407 通过 PB12/PB13/PB15 软件 SPI 写入 24 位帧：

```text
控制字 0x00 + 16 位 DAC 码值，高字节在前
```

写入策略：

- 每次模拟量刷新时重复写入 3 次，提高可靠性
- 模拟量刷新周期为 `200 ms`
- 初始化时写入 `0x0000`
- DAC8550 的输出电压由模块参考电压、后级放大电路和接线决定
- DAC 模块、F407、传感器和测量设备必须共地；GND 接错会导致输出接近 0 或异常

温度会先按 `-40.00℃ .. 300.00℃` 线性换算到 16 位满量程，再按 DAC8550 二补码输入格式写入。

### 4-20mA 电流输出

电流通道使用 GP8302，I2C 地址为 `0x58`，输出寄存器为 `0x02`。

映射关系：

| 温度 | 输出电流 |
| --- | --- |
| -40.00℃ | 4 mA |
| 300.00℃ | 20 mA |

计算关系：

```text
current_uA = 4000 + (temperature - TEMP_MIN) / (TEMP_MAX - TEMP_MIN) * 16000
GP8302 码值 = current_uA / 25000 * 4095
```

因此常温约 28℃ 时，比例约 20%，电流目标大约是 `7.2 mA`。实际输出会受电流模块精度、负载、电源和校准影响。

## CAN 通信

两个项目都使用 CAN 500 kbit/s。

| CAN ID | 方向 | 周期/触发 | 内容 |
| --- | --- | --- | --- |
| `0x320` | F407 -> 总线 | 500 ms | 心跳帧 |
| `0x321` | F407 -> 总线 | 每次有效 Modbus 温度帧 | 温度、DAC 码、电流、状态 |
| `0x322` | F407 -> 总线 | 500 ms | 诊断状态 |
| `0x700` | F407 -> F103 | 1000 ms | F103 链路测试请求 |
| `0x701` | F103 -> F407 | 收到 `0x700` 后回复 | `F103OK` 和计数 |

`0x321` 温度帧格式：

```text
DATA[0..3]  int32，大端，温度，单位 0.01℃
DATA[4..5]  uint16，大端，最近一次电压 DAC 码
DATA[6]     current_uA / 100
DATA[7]     应用状态码
```

`0x701` 握手回复格式：

```text
46 31 30 33 4F 4B xx xx
 F  1  0  3  O  K  计数低 计数高
```

## 构建

两个子工程都可以单独用 CLion 打开。命令行构建如下：

```sh
cd temp-bridge-407
cmake --preset Debug
cmake --build --preset Debug

cd ../can-oled-display-103
cmake --preset debug
cmake --build --preset debug
```

构建产物会生成 `.elf`、`.hex`、`.bin`，这些文件不会提交到 git。

## 仓库结构

```text
temp-can-suite/
  temp-bridge-407/        STM32F407 温度桥接和模拟量输出固件
  can-oled-display-103/   STM32F103 CAN 接收和 OLED 显示固件
```
