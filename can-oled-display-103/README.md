# CanOledDisplay103

Bare-metal CLion/CMake project for STM32F103C8T6.

The firmware receives CAN frames on bxCAN through an SN65HVD230 transceiver, prints every received frame on USART1, and shows the latest temperature value on a 0.96 inch SSD1306 I2C OLED.

## Default Settings

- MCU clock: 72 MHz from an 8 MHz HSE crystal
- CAN1 bitrate: 500 kbit/s
- CAN filter: accept all standard and extended IDs
- CAN link check: host sends standard ID `0x700`, F103 replies standard ID `0x701`
- OLED: SSD1306 I2C, address 0x3C (write address byte 0x78), software I2C open-drain on PB6/PB7
- UART: USART1, 115200 baud, 8 data bits, no parity, 1 stop bit
- Temperature decode: upper-controller CAN ID `0x321`, DATA[0..3], big-endian signed integer, unit 0.01 C
- PC13 LED: steady on until CAN link check succeeds; blinks while the last successful handshake is within 10 seconds

## Pin Assignment

| Function | STM32F103C8T6 pin | Connect to |
| --- | --- | --- |
| CAN1_RX | PB8 | SN65HVD230 R / RXD |
| CAN1_TX | PB9 | SN65HVD230 D / TXD |
| OLED_SCL | PB6 | SSD1306 OLED SCL |
| OLED_SDA | PB7 | SSD1306 OLED SDA |
| LED | PC13 | Blue Pill onboard LED, active low |
| USART1_TX | PA9 | USB-UART adapter RX |
| USART1_RX | PA10 | USB-UART adapter TX, optional |
| SWDIO | PA13 | ST-Link SWDIO |
| SWCLK | PA14 | ST-Link SWCLK |
| 3V3 | 3.3 V | SN65HVD230 VCC |
| GND | GND | SN65HVD230 GND and USB-UART GND |
| CANH | Transceiver bus side | CANH bus |
| CANL | Transceiver bus side | CANL bus |

Notes:

- CAN1 is remapped to PB8/PB9 to avoid the Blue Pill USB D-/D+ circuit on PA11/PA12.
- Even as a CAN receiver, keep CAN1_TX / PB9 connected to the transceiver TXD/D pin. The CAN controller uses it to send the ACK bit.
- Tie SN65HVD230 RS to GND for high-speed mode, or use the module's default RS setting if it already provides one.
- Add 120 ohm termination between CANH and CANL only if this node is at one end of the CAN bus.
- Use 3.3 V UART levels. Do not connect 5 V logic directly to STM32 pins.
- Power the OLED from 3.3 V and share GND with the STM32. The OLED driver now follows the provided 4-pin I2C example style: SCL/SDA are open-drain software I2C lines. Most OLED modules already include I2C pull-ups; if yours does not, add 4.7 kOhm pull-ups from PB6/PB7 to 3.3 V.

## CAN Link Check

The minimal host-controller handshake is:

```text
Host -> F103:
  CAN ID: 0x700 standard data frame
  DLC: any value from 0 to 8
  DATA: ignored

F103 -> Host:
  CAN ID: 0x701 standard data frame
  DLC: 8
  DATA: 46 31 30 33 4F 4B xx xx
        F  1  0  3  O  K  rx counter low/high
```

When the `0x701` reply is transmitted successfully and ACKed on the CAN bus, the firmware treats the link as normal. PC13 then blinks. If no successful handshake occurs for 10 seconds, PC13 returns to steady on.

## OLED Temperature Display

The OLED shows:

```text
CAN TEMP
ID:123
TEMP:25.3 C
RX:1
```

Temperature is decoded from the upper-controller temperature frame:

```text
CAN ID = 0x321
temperature_centi_c = DATA[0] << 24 | DATA[1] << 16 | DATA[2] << 8 | DATA[3]
temperature_celsius = temperature_centi_c / 100
```

For example, 25.30 C should be sent as decimal 2530, which is `00 00 09 E2` in the first four CAN data bytes. Legacy two-byte 0.1 C frames are still accepted as a fallback.

## Build

The project uses CMake presets. In CLion, open this directory and choose the `debug` or `release` preset.

From a terminal:

```sh
cmake --preset debug
cmake --build --preset debug
```

Generated files are placed in `build/debug/`:

- `CanOledDisplay103.elf`
- `CanOledDisplay103.hex`
- `CanOledDisplay103.bin`

## Example UART Output

```text
STM32F103C8T6 CAN to UART bridge
CAN receiver: 500 kbit/s, accept all IDs
Link check: host sends std id 0x700, F103 replies std id 0x701
USART1: 115200 8N1
Waiting for CAN frames...
CAN stat rx=0x00000000 tx=0x00000000 link=WAIT pong=0x00000000 pb8=H pb9=H ...
CAN id=0x123 std data dlc=8 bytes=11 22 33 44 55 66 77 88
CAN link OK: replied id=0x701 bytes=46 31 30 33 4F 4B 01 00
```
