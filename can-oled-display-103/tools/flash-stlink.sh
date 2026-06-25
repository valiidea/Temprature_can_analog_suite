#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build/codex-debug"
PROGRAMMER="/opt/ST/STM32CubeCLT_1.21.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI"
TOOLCHAIN_FILE="${ROOT_DIR}/cmake/arm-none-eabi-gcc.cmake"
HEX_FILE="${BUILD_DIR}/CanOledDisplay103.hex"

if [[ ! -x "${PROGRAMMER}" ]]; then
  echo "STM32_Programmer_CLI not found: ${PROGRAMMER}" >&2
  exit 1
fi

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build "${BUILD_DIR}"

"${PROGRAMMER}" \
  -c port=SWD freq=4000 mode=UR reset=HWrst \
  -w "${HEX_FILE}" \
  -v \
  -rst
