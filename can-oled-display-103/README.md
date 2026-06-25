# CanOledDisplay103

`CanOledDisplay103` 是 STM32F103C8T6 CAN 接收和 OLED 显示固件，用作 `TempBridge407` 的下位接收端。

主要功能：

- 使用 bxCAN 接收 F407 发出的温度帧
- 在 SSD1306 OLED 上显示最新温度
- 通过 USART1 输出 CAN 调试日志
- 收到 `0x700` 标准帧后回复 `0x701`，用于 F407 链路检测

## 默认配置

- MCU：STM32F103C8T6
- 时钟：8 MHz HSE，系统时钟 72 MHz
- CAN：500 kbit/s
- CAN 过滤器：接收所有标准帧和扩展帧
- OLED：SSD1306 I2C，默认地址 `0x3C`，软件 I2C
- UART：USART1，`115200-8N1`
- 状态灯：PC13，低电平点亮

## 外设分配

| 功能 | STM32F103 引脚 | 连接 |
| --- | --- | --- |
| CAN1_RX | PB8 | SN65HVD230 R / RXD |
| CAN1_TX | PB9 | SN65HVD230 D / TXD |
| OLED_SCL | PB6 | SSD1306 SCL |
| OLED_SDA | PB7 | SSD1306 SDA |
| USART1_TX | PA9 | USB-UART RX |
| USART1_RX | PA10 | USB-UART TX，可选 |
| LED | PC13 | Blue Pill 板载 LED，低电平亮 |
| SWDIO | PA13 | ST-Link SWDIO |
| SWCLK | PA14 | ST-Link SWCLK |
| 3V3 | 3.3 V | CAN 收发器和 OLED 供电 |
| GND | GND | 与 F407、CAN 模块、OLED 共地 |
| CANH | 总线侧 | CANH |
| CANL | 总线侧 | CANL |

注意事项：

- CAN1 已重映射到 PB8/PB9，避开 Blue Pill 的 PA11/PA12 USB 电路
- 即使只做接收端，也要连接 PB9 到 CAN 收发器 TXD/D；CAN ACK 需要 TX 引脚参与
- SN65HVD230 的 RS 建议接 GND，或使用模块默认高速模式
- 只有在总线端点处才加 120 欧终端电阻
- OLED 和 F103 必须共地，SCL/SDA 需要上拉；多数 OLED 模块已自带上拉

## CAN 温度显示

F103 主要解析 F407 的 `0x321` 温度帧：

```text
CAN ID = 0x321
DATA[0..3] = int32，大端，温度，单位 0.01℃
```

例如 `25.30℃`：

```text
temperature_centi_c = 2530 = 0x000009E2
DATA[0..3] = 00 00 09 E2
```

OLED 显示内容类似：

```text
CAN TEMP
ID:321
TEMP:25.3 C
RX:1
```

## CAN 链路检测

F407 每秒发送一次测试帧：

```text
F407 -> F103
标准帧 ID: 0x700
DLC: 任意
DATA: 任意
```

F103 收到后回复：

```text
F103 -> F407
标准帧 ID: 0x701
DLC: 8
DATA: 46 31 30 33 4F 4B xx xx
      F  1  0  3  O  K  计数低 计数高
```

当 `0x701` 回复被 CAN 总线 ACK 后，F103 判定链路正常，PC13 进入闪烁状态；如果 10 秒内没有成功握手，PC13 回到常亮。

## 构建

```sh
cmake --preset debug
cmake --build --preset debug
```

生成文件：

- `build/debug/CanOledDisplay103.elf`
- `build/debug/CanOledDisplay103.hex`
- `build/debug/CanOledDisplay103.bin`

## 串口日志示例

```text
CanOledDisplay103 CAN receiver
CAN receiver: 500 kbit/s, accept all IDs
Link check: host sends std id 0x700, F103 replies std id 0x701
USART1: 115200 8N1
Waiting for CAN frames...
CAN id=0x321 std data dlc=8 bytes=00 00 09 E2 B3 33 48 00
CAN link OK: replied id=0x701 bytes=46 31 30 33 4F 4B 01 00
```
