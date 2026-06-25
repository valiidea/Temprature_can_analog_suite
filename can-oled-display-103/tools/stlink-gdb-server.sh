#!/usr/bin/env bash
set -euo pipefail

GDB_SERVER="/opt/ST/STM32CubeCLT_1.21.0/STLink-gdb-server/bin/ST-LINK_gdbserver"
CUBE_PROGRAMMER="/opt/ST/STM32CubeCLT_1.21.0/STM32CubeProgrammer/bin"

exec "${GDB_SERVER}" \
  --swd \
  --initialize-reset \
  --verify \
  --frequency 4000 \
  --port-number 61234 \
  --stm32cubeprogrammer-path "${CUBE_PROGRAMMER}"
