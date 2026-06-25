# TempCanSuite

Two STM32 firmware projects for the temperature CAN system.

## Projects

- `temp-bridge-407/` - `TempBridge407`, STM32F407 temperature bridge. It polls the Modbus temperature sensor, outputs voltage/current analog signals, sends CAN temperature frames, and performs the F103 link check.
- `can-oled-display-103/` - `CanOledDisplay103`, STM32F103 CAN receiver. It receives the F407 temperature frame, replies to the CAN link-check frame, prints diagnostics on UART, and displays the latest temperature on an SSD1306 OLED.

## CAN Link

- F407 sends temperature on standard ID `0x321`.
- F407 sends link-test request on standard ID `0x700`.
- F103 replies standard ID `0x701`, DLC 8, data `46 31 30 33 4F 4B xx xx` (`F103OK` plus counter).
- Both projects use CAN 500 kbit/s.

## Build

Open either subdirectory in CLion and select its CMake preset.

Terminal builds:

```sh
cd temp-bridge-407
cmake --preset Debug
cmake --build --preset Debug

cd ../can-oled-display-103
cmake --preset debug
cmake --build --preset debug
```

Generated `.elf`, `.hex`, and `.bin` files are ignored by git.
