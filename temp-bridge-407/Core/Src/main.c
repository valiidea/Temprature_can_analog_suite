/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  APP_OK = 0,
  APP_UART_TX_ERROR,
  APP_UART_TIMEOUT,
  APP_MODBUS_BAD_CRC,
  APP_MODBUS_BAD_FRAME,
  APP_SENSOR_BAD_STATUS,
  APP_TEMPERATURE_OUT_OF_RANGE,
  APP_TEMPERATURE_NOT_STABLE,
  APP_GP8302_I2C_ERROR,
  APP_CAN_ERROR
} AppStatus_t;

typedef struct
{
  int32_t temperature_centi_c;
  uint16_t status;
} TemperatureSample_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MODBUS_SLAVE_ADDR              0xF7U
#define MODBUS_FUNC_READ_HOLDING       0x03U
#define MODBUS_FUNC_EXCEPTION          0x83U
#define MODBUS_RESPONSE_LEN            21U
#define MODBUS_EXCEPTION_LEN           5U
#define MODBUS_RESPONSE_DATA_LEN       16U
#define MODBUS_RX_STREAM_LEN           80U

#define UART_TX_TIMEOUT_MS             50U
#define UART_BYTE_TIMEOUT_MS           4U
#define MODBUS_FRAME_TIMEOUT_MS        180U
#define MODBUS_POLL_INTERVAL_MS        0U
#define ANALOG_OUTPUT_INTERVAL_MS      200U

#define SENSOR_VALID_MIN_CENTI_C       ((int32_t)-6000)
#define SENSOR_VALID_MAX_CENTI_C       ((int32_t)33000)
#define SENSOR_NORMAL_STATUS           1U

#define TEMP_MIN_CENTI_C               ((int32_t)-4000)
#define TEMP_MAX_CENTI_C               ((int32_t)30000)
#define TEMP_RANGE_CENTI_C             (TEMP_MAX_CENTI_C - TEMP_MIN_CENTI_C)
#define TEMP_STARTUP_WINDOW_CENTI_C    ((int32_t)500)
#define TEMP_STARTUP_STABLE_COUNT      3U
#define TEMP_ACCEPT_WINDOW_CENTI_C     ((int32_t)2000)
#define TEMP_RATE_LIMIT_CENTI_C        ((int32_t)200)

#define DAC8550_WRITE_REPEATS          3U
#define DAC8550_BIT_DELAY_CYCLES       120U
#define DAC8550_ZERO_CODE              0x0000U

#define GP8302_I2C_ADDR_7BIT           0x58U
#define GP8302_OUTPUT_REG              0x02U
#define GP8302_DAC_MAX_CODE            0x0FFFU
#define GP8302_CURRENT_FULL_SCALE_UA   25000U
#define GP8302_CURRENT_MIN_UA          4000U
#define GP8302_CURRENT_MAX_UA          20000U
#define GP8302_I2C_TIMEOUT_MS          20U

#define CAN_DEFAULT_BITRATE            500000U
#define CAN_HEARTBEAT_STD_ID           0x320U
#define CAN_TEMP_STD_ID                0x321U
#define CAN_DIAG_STD_ID                0x322U
#define CAN_F103_TEST_REQ_STD_ID       0x700U
#define CAN_F103_TEST_RESP_STD_ID      0x701U
#define CAN_HEARTBEAT_INTERVAL_MS      500U
#define CAN_DIAG_INTERVAL_MS           500U
#define CAN_F103_TEST_INTERVAL_MS      1000U
#define CAN_TX_TIMEOUT_MS              10U
#define CAN_RECOVERY_INTERVAL_MS       200U
#define CAN_RECOVERABLE_HAL_ERROR_MASK (HAL_CAN_ERROR_BOF | HAL_CAN_ERROR_EPV | HAL_CAN_ERROR_ACK | \
                                        HAL_CAN_ERROR_TX_TERR0 | HAL_CAN_ERROR_TX_TERR1 | HAL_CAN_ERROR_TX_TERR2)
#define CAN_RECOVERABLE_ESR_MASK       (CAN_ESR_BOFF | CAN_ESR_EPVF)
#define CAN_ESR_LEC_MASK_APP           (7UL << 4)
#define CAN_ESR_LEC_ACK_APP            (3UL << 4)
#define CAN_TSR_TX_ERROR_MASK_APP      (CAN_TSR_TERR0 | CAN_TSR_TERR1 | CAN_TSR_TERR2)

#define LED_ON_STATE                   GPIO_PIN_SET
#define LED_OFF_STATE                  GPIO_PIN_RESET
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
volatile AppStatus_t g_app_status = APP_UART_TIMEOUT;
volatile int32_t g_raw_temperature_centi_c = 0;
volatile int32_t g_output_temperature_centi_c = 0;
volatile uint16_t g_dac_code = DAC8550_ZERO_CODE;
volatile uint16_t g_current_code = 0;
volatile uint32_t g_current_uA = GP8302_CURRENT_MIN_UA;
volatile uint16_t g_sensor_status = 0;
volatile uint32_t g_i2c_error_count = 0;
volatile uint32_t g_can_error_count = 0;
volatile uint32_t g_can_tx_ok_count = 0;
volatile uint32_t g_can_heartbeat_count = 0;
volatile uint32_t g_can_temp_count = 0;
volatile uint32_t g_can_diag_count = 0;
volatile uint32_t g_can_f103_test_req_count = 0;
volatile uint32_t g_can_f103_test_resp_count = 0;
volatile uint32_t g_can_f103_test_bad_resp_count = 0;
volatile uint16_t g_can_f103_test_last_counter = 0;
volatile uint32_t g_can_last_hal_error = 0;
volatile uint32_t g_can_last_esr = 0;
volatile uint32_t g_can_last_tsr = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C2_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */
static void App_RunOnce(void);
static bool TemperatureDevice_Read(TemperatureSample_t *sample, AppStatus_t *status);
static bool Modbus_FindTemperatureFrame(const uint8_t *rx, uint16_t rx_len,
                                        TemperatureSample_t *sample, AppStatus_t *status);
static bool Modbus_ParseTemperatureFrame(const uint8_t *frame, TemperatureSample_t *sample,
                                         AppStatus_t *status);
static bool Modbus_FrameCrcOk(const uint8_t *frame, uint16_t length);
static uint16_t Modbus_Crc16(const uint8_t *data, uint16_t length);
static void UART_ClearRx(void);

static bool TemperatureFilter_Update(int32_t measured_centi_c, int32_t *filtered_centi_c);
static uint16_t TemperatureToDacCode(int32_t temp_centi_c);
static uint32_t TemperatureToCurrentMicroamps(int32_t temp_centi_c);
static uint16_t CurrentMicroampsToGp8302Code(uint32_t current_uA);
static int32_t ClampInt32(int32_t value, int32_t low, int32_t high);
static int32_t AbsDiffInt32(int32_t a, int32_t b);

static HAL_StatusTypeDef GP8302_WriteCode(uint16_t code);
static HAL_StatusTypeDef GP8302_SetCurrent(uint32_t current_uA);
static HAL_StatusTypeDef CAN_SendTemperatureFrame(int32_t temp_centi_c,
                                                  uint16_t dac_code,
                                                  uint32_t current_uA,
                                                  AppStatus_t app_status);
static HAL_StatusTypeDef CAN_SendHeartbeatFrame(void);
static HAL_StatusTypeDef CAN_SendDiagnosticFrame(void);
static HAL_StatusTypeDef CAN_SendF103TestFrame(void);
static HAL_StatusTypeDef CAN_SendDataFrame(uint32_t std_id, const uint8_t *data, uint8_t dlc);
static void CAN_ProcessReceivedFrames(void);
static void CAN_ProcessF103TestResponse(const CAN_RxHeaderTypeDef *rx_header, const uint8_t *data);
static void CAN_RecoverIfNeeded(void);
static bool CAN_TxMailboxOk(uint32_t mailbox, uint32_t tsr);

static void DAC8550_WriteCode(uint16_t code);
static void DAC8550_WriteFrame(uint16_t code);
static void DAC8550_WriteByte(uint8_t value);
static void DAC8550_BitDelay(void);

static void Led_SetVoltageOk(void);
static void Led_SetVoltageFault(void);
static void Led_SetCurrentOk(void);
static void Led_SetCurrentFault(void);
static void Led_SetAllFault(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 * ============================================================================
 * 系统概述
 * ============================================================================
 * 本固件运行于 STM32F407VETx，实现温度转模拟量输出功能，信号链路如下：
 *
 *   1. Modbus RTU 主机通过 UART1 (115200-8N1) 轮询温度传感器（从机地址 0xF7），
 *      读取从寄存器 0x1000 开始的 8 个保持寄存器。
 *
 *   2. 解析传感器响应帧：将 32 位原始温度值转换为摄氏温度（百分度），
 *      并校验传感器状态字。
 *
 *   3. 两级数字滤波器处理温度数据：
 *        - 启动锁定阶段：等待连续 N 次读数落在窄窗口内，锁定初始值。
 *        - 运行阶段：对每次采样施加变化率限制，抑制瞬态尖峰和异常值。
 *
 *   4. 滤波后的温度线性映射为 16 位 DAC 码值 (DAC8550)：
 *        TEMP_MIN..TEMP_MAX → 0x0000..0xFFFF
 *
 *   5. DAC8550 通过三个 GPIO 引脚 (SYNC, SCLK, DIN) 以软件模拟 SPI 驱动，
 *      每个码值重复写入 3 次以确保可靠性。
 *
 *   6. GP8302 4-20mA 电流模块通过 I2C2 (PB10/PB11) 输出：
 *        TEMP_MIN..TEMP_MAX → 4mA..20mA
 *
 *   7. SN65HVD230 连接 CAN1 (PD0/PD1)，每次成功采样后发送温度和最近一次模拟输出状态。
 *
 *   8. 两个 LED 分别指示输出通道状态：
 *        - PA6: 电压输出正常闪烁，故障常亮
 *        - PA7: 电流输出正常闪烁，故障常亮
 *
 *   主循环不额外限速；每次 Modbus 成功读到温度后立即发送 CAN 温度帧。
 *   模拟量输出保持独立节奏更新，避免高频重写 DAC/电流模块。
 * ============================================================================
 */

/*
 * Modbus RTU 读取温度请求帧。
 * 格式: [地址] [功能码] [寄存器高] [寄存器低] [数量高] [数量低] [CRC低] [CRC高]
 *   Addr  = 0xF7 (传感器从机地址)
 *   Func  = 0x03 (读保持寄存器)
 *   Reg   = 0x1000 (起始寄存器地址)
 *   Count = 0x0008 (8 个寄存器 = 16 字节数据)
 *   CRC   = 0x5A54 (预计算的 Modbus CRC-16)
 */
static const uint8_t kTemperatureRequest[8] = {
  0xF7U, 0x03U, 0x10U, 0x00U, 0x00U, 0x08U, 0x54U, 0x5AU
};

/*
 * 单次运行：读取传感器 → 立即发送 CAN 温度帧；模拟电压/电流输出按独立周期更新。
 * CAN 温度帧频率由 Modbus 设备实际响应速度决定。
 */
static void App_RunOnce(void)
{
  static bool analog_output_started = false;
  static uint32_t last_analog_output_tick = 0U;
  TemperatureSample_t sample;
  int32_t filtered_temp;
  AppStatus_t status = APP_UART_TIMEOUT;

  /* 每成功解析一帧 Modbus 温度，就立即转发一帧 CAN 温度。 */
  if (TemperatureDevice_Read(&sample, &status))
  {
    uint32_t now_tick = HAL_GetTick();
    bool update_analog_output = (!analog_output_started) ||
                                ((now_tick - last_analog_output_tick) >= ANALOG_OUTPUT_INTERVAL_MS);

    g_raw_temperature_centi_c = sample.temperature_centi_c;
    g_sensor_status = sample.status;

    if (update_analog_output && TemperatureFilter_Update(sample.temperature_centi_c, &filtered_temp))
    {
      uint16_t dac_code = TemperatureToDacCode(filtered_temp);
      uint32_t current_uA = TemperatureToCurrentMicroamps(filtered_temp);
      uint16_t current_code = CurrentMicroampsToGp8302Code(current_uA);

      analog_output_started = true;
      last_analog_output_tick = now_tick;
      g_app_status = APP_OK;
      g_output_temperature_centi_c = filtered_temp;
      g_dac_code = dac_code;
      g_current_uA = current_uA;
      g_current_code = current_code;

      DAC8550_WriteCode(dac_code);
      Led_SetVoltageOk();

      if (GP8302_SetCurrent(current_uA) != HAL_OK)
      {
        Led_SetCurrentFault();
        g_app_status = APP_GP8302_I2C_ERROR;
        g_i2c_error_count++;
      }
      else
      {
        Led_SetCurrentOk();
      }
    }
    else if (update_analog_output)
    {
      analog_output_started = true;
      last_analog_output_tick = now_tick;
      Led_SetAllFault();
      g_app_status = APP_TEMPERATURE_NOT_STABLE;
    }

    if (CAN_SendTemperatureFrame(sample.temperature_centi_c,
                                 g_dac_code,
                                 g_current_uA,
                                 g_app_status) != HAL_OK)
    {
      g_can_error_count++;
    }
  }
  else
  {
    /* 读取或滤波失败时进入故障状态 */
    if (status == APP_OK)
    {
      status = APP_TEMPERATURE_NOT_STABLE;
    }

    Led_SetAllFault();
    g_app_status = status;
  }
}

/*
 * 通过 Modbus RTU 从温度传感器读取一次采样。
 * 流程：清空 UART 接收缓冲 → 发送读寄存器请求 → 在超时窗口内逐字节接收并扫描有效帧。
 * 返回 true 表示成功解析到有效温度帧。
 */
static bool TemperatureDevice_Read(TemperatureSample_t *sample, AppStatus_t *status)
{
  uint8_t rx[MODBUS_RX_STREAM_LEN];   /* 接收字节流缓冲区 */
  uint16_t rx_len = 0U;               /* 当前已接收字节数 */
  uint32_t start_tick;

  /* 清空 UART 接收路径上的残留数据和错误标志 */
  UART_ClearRx();

  if (HAL_UART_Transmit(&huart1, (uint8_t *)kTemperatureRequest,
                        sizeof(kTemperatureRequest), UART_TX_TIMEOUT_MS) != HAL_OK)
  {
    *status = APP_UART_TX_ERROR;
    return false;
  }

  start_tick = HAL_GetTick();
  while ((HAL_GetTick() - start_tick) < MODBUS_FRAME_TIMEOUT_MS)
  {
    uint8_t byte;

    if (HAL_UART_Receive(&huart1, &byte, 1U, UART_BYTE_TIMEOUT_MS) == HAL_OK)
    {
      if (rx_len < MODBUS_RX_STREAM_LEN)
      {
        rx[rx_len++] = byte;
      }
      else
      {
        for (uint16_t i = 1U; i < MODBUS_RX_STREAM_LEN; i++)
        {
          rx[i - 1U] = rx[i];
        }
        rx[MODBUS_RX_STREAM_LEN - 1U] = byte;
      }

      if (Modbus_FindTemperatureFrame(rx, rx_len, sample, status))
      {
        return true;
      }
    }
  }

  *status = APP_UART_TIMEOUT;
  return false;
}

/*
 * 在接收字节流中滑动扫描有效的 Modbus 帧。
 * 优先检测异常响应帧（功能码 0x83 + 正确 CRC），若发现则提前返回失败。
 * 然后扫描正常响应帧：从机地址 + 功能码 0x03 + 数据长度匹配 + CRC 校验通过。
 * 帧长度：异常帧 5 字节，正常响应帧 21 字节。
 */
static bool Modbus_FindTemperatureFrame(const uint8_t *rx, uint16_t rx_len,
                                        TemperatureSample_t *sample, AppStatus_t *status)
{
  /* 先扫描是否存在异常响应帧（传感器返回的错误指示） */
  if (rx_len >= MODBUS_EXCEPTION_LEN)
  {
    for (uint16_t offset = 0U; offset <= (rx_len - MODBUS_EXCEPTION_LEN); offset++)
    {
      const uint8_t *frame = &rx[offset];

      if ((frame[0] == MODBUS_SLAVE_ADDR) &&
          (frame[1] == MODBUS_FUNC_EXCEPTION) &&
          Modbus_FrameCrcOk(frame, MODBUS_EXCEPTION_LEN))
      {
        *status = APP_MODBUS_BAD_FRAME;
        return false;
      }
    }
  }

  if (rx_len < MODBUS_RESPONSE_LEN)
  {
    *status = APP_UART_TIMEOUT;
    return false;
  }

  for (uint16_t offset = 0U; offset <= (rx_len - MODBUS_RESPONSE_LEN); offset++)
  {
    const uint8_t *frame = &rx[offset];

    if ((frame[0] != MODBUS_SLAVE_ADDR) ||
        (frame[1] != MODBUS_FUNC_READ_HOLDING) ||
        (frame[2] != MODBUS_RESPONSE_DATA_LEN))
    {
      continue;
    }

    if (!Modbus_FrameCrcOk(frame, MODBUS_RESPONSE_LEN))
    {
      *status = APP_MODBUS_BAD_CRC;
      continue;
    }

    return Modbus_ParseTemperatureFrame(frame, sample, status);
  }

  *status = APP_MODBUS_BAD_FRAME;
  return false;
}

/*
 * 解析 Modbus 温度响应帧的数据字段。
 * 帧格式 (字节偏移):
 *   [0]     从机地址 (0xF7)
 *   [1]     功能码 (0x03)
 *   [2]     数据字节数 (16)
 *   [3..6]  32 位原始温度值 (大端序，比例 1/65536 K)
 *   [7..16] 保留/其他寄存器
 *   [17..18] 16 位传感器状态字 (大端序)
 *   [19..20] CRC-16
 *
 * 温度转换公式：摄氏度 = (raw_temp * 100 + 32768) / 65536 - 273.15（以百分度表示）
 *   即 ℃ = raw_temp / 655.36 - 273.15，乘以 100 得到百分度。
 */
static bool Modbus_ParseTemperatureFrame(const uint8_t *frame, TemperatureSample_t *sample,
                                         AppStatus_t *status)
{
  uint32_t raw_temp;
  int32_t temp_centi_c;
  uint16_t sensor_status;

  /* 提取 32 位大端原始温度值 */
  raw_temp = ((uint32_t)frame[3] << 24) |
             ((uint32_t)frame[4] << 16) |
             ((uint32_t)frame[5] << 8) |
             ((uint32_t)frame[6]);
  /* 转换为摄氏百分度 (℃ × 100)：先乘 100 并四舍五入，再除以 65536，最后减去 273.15℃ 的偏移 */
  temp_centi_c = (int32_t)((((int64_t)raw_temp * 100) + 32768) / 65536) - 27300;

  /* 提取 16 位大端传感器状态字 */
  sensor_status = ((uint16_t)frame[17] << 8) | frame[18];

  g_raw_temperature_centi_c = temp_centi_c;
  g_sensor_status = sensor_status;

  if (sensor_status != SENSOR_NORMAL_STATUS)
  {
    *status = APP_SENSOR_BAD_STATUS;
    return false;
  }

  if ((temp_centi_c < SENSOR_VALID_MIN_CENTI_C) ||
      (temp_centi_c > SENSOR_VALID_MAX_CENTI_C))
  {
    *status = APP_TEMPERATURE_OUT_OF_RANGE;
    return false;
  }

  sample->temperature_centi_c = temp_centi_c;
  sample->status = sensor_status;
  *status = APP_OK;
  return true;
}

/*
 * 校验 Modbus 帧的 CRC-16 校验和。
 * 对帧的前 (length - 2) 字节计算 CRC，与帧末 2 字节的 CRC 值比较。
 * 帧长度至少为 3 字节（1 字节数据 + 2 字节 CRC）。
 */
static bool Modbus_FrameCrcOk(const uint8_t *frame, uint16_t length)
{
  uint16_t expected;   /* 根据数据计算的 CRC */
  uint16_t actual;     /* 帧中携带的 CRC 值 */

  if (length < 3U)
  {
    return false;
  }

  expected = Modbus_Crc16(frame, (uint16_t)(length - 2U));
  actual = (uint16_t)frame[length - 2U] | ((uint16_t)frame[length - 1U] << 8);

  return expected == actual;
}

/*
 * Modbus CRC-16 计算 (多项式 0xA001，初始值 0xFFFF，LSB 优先)。
 * 标准 Modbus RTU 校验算法。
 */
static uint16_t Modbus_Crc16(const uint8_t *data, uint16_t length)
{
  uint16_t crc = 0xFFFFU;   /* CRC 初始值 */

  for (uint16_t i = 0U; i < length; i++)
  {
    crc ^= data[i];
    for (uint8_t bit = 0U; bit < 8U; bit++)
    {
      if ((crc & 0x0001U) != 0U)
      {
        crc = (uint16_t)((crc >> 1) ^ 0xA001U);
      }
      else
      {
        crc >>= 1;
      }
    }
  }

  return crc;
}

/*
 * 清空 UART 接收缓冲区：清除所有错误标志（PE/FE/NE/ORE），
 * 排空 RXNE 数据寄存器中残留的未读字节。
 * 每次发起 Modbus 请求前调用，确保接收从干净状态开始。
 */
static void UART_ClearRx(void)
{
  volatile uint32_t dummy;   /* 用于读取并丢弃残留数据 */

  __HAL_UART_CLEAR_PEFLAG(&huart1);
  __HAL_UART_CLEAR_FEFLAG(&huart1);
  __HAL_UART_CLEAR_NEFLAG(&huart1);
  __HAL_UART_CLEAR_OREFLAG(&huart1);

  while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
  {
    dummy = __HAL_UART_FLUSH_DRREGISTER(&huart1);
    (void)dummy;
  }
}

/*
 * 温度数字滤波器 —— 两级结构：
 *
 * 阶段 1 — 启动锁定:
 *   上电后等待连续 TEMP_STARTUP_STABLE_COUNT 次采样落在
 *   TEMP_STARTUP_WINDOW_CENTI_C 窗口内，取均值锁定初始输出。
 *
 * 阶段 2 — 变化率限制:
 *   每次新值与当前输出的差值若超过 TEMP_RATE_LIMIT_CENTI_C，
 *   只允许按限制值步进（抑制尖峰）；差值在限制内则直接跟随。
 *   若偏差超过 TEMP_ACCEPT_WINDOW_CENTI_C，拒绝本次更新。
 *
 * 返回 true 表示成功产生滤波输出（已锁定且数据在合理范围内）。
 */
static bool TemperatureFilter_Update(int32_t measured_centi_c, int32_t *filtered_centi_c)
{
  static bool locked = false;                  /* 是否已完成启动锁定 */
  static int32_t startup_candidate = 0;        /* 启动阶段候选值 */
  static uint8_t startup_count = 0U;           /* 连续稳定采样计数 */
  static int32_t output_temp_centi_c = 0;      /* 当前滤波输出值 */
  int32_t delta;                               /* 新采样与当前输出的差值 */

  if (!locked)
  {
    if ((startup_count == 0U) ||
        (AbsDiffInt32(measured_centi_c, startup_candidate) > TEMP_STARTUP_WINDOW_CENTI_C))
    {
      startup_candidate = measured_centi_c;
      startup_count = 1U;
    }
    else
    {
      startup_candidate = (startup_candidate + measured_centi_c) / 2;
      startup_count++;
    }

    if (startup_count < TEMP_STARTUP_STABLE_COUNT)
    {
      return false;
    }

    locked = true;
    output_temp_centi_c = startup_candidate;
    *filtered_centi_c = output_temp_centi_c;
    return true;
  }

  if (AbsDiffInt32(measured_centi_c, output_temp_centi_c) > TEMP_ACCEPT_WINDOW_CENTI_C)
  {
    return false;
  }

  delta = measured_centi_c - output_temp_centi_c;
  if (delta > TEMP_RATE_LIMIT_CENTI_C)
  {
    output_temp_centi_c += TEMP_RATE_LIMIT_CENTI_C;
  }
  else if (delta < -TEMP_RATE_LIMIT_CENTI_C)
  {
    output_temp_centi_c -= TEMP_RATE_LIMIT_CENTI_C;
  }
  else
  {
    output_temp_centi_c = measured_centi_c;
  }

  *filtered_centi_c = output_temp_centi_c;
  return true;
}

/*
 * 温度值 → DAC 码值线性映射。
 * 映射范围：TEMP_MIN_CENTI_C..TEMP_MAX_CENTI_C → 0x0000..0xFFFF（16 位满量程）
 * 先将温度钳位到有效范围，再按线性比例缩放，最后转为有符号偏移码。
 */
static uint16_t TemperatureToDacCode(int32_t temp_centi_c)
{
  int64_t offset_code;    /* 64 位中间值，防止溢出 */
  int32_t signed_code;    /* 有符号偏移码 */

  temp_centi_c = ClampInt32(temp_centi_c, TEMP_MIN_CENTI_C, TEMP_MAX_CENTI_C);

  offset_code = ((int64_t)(temp_centi_c - TEMP_MIN_CENTI_C) * 65535) +
                (TEMP_RANGE_CENTI_C / 2);
  offset_code /= TEMP_RANGE_CENTI_C;
  signed_code = (int32_t)offset_code - 32768;

  return (uint16_t)((int16_t)signed_code);
}

static uint32_t TemperatureToCurrentMicroamps(int32_t temp_centi_c)
{
  int64_t scaled_uA;

  temp_centi_c = ClampInt32(temp_centi_c, TEMP_MIN_CENTI_C, TEMP_MAX_CENTI_C);
  scaled_uA = GP8302_CURRENT_MIN_UA +
              (((int64_t)(temp_centi_c - TEMP_MIN_CENTI_C) *
                (GP8302_CURRENT_MAX_UA - GP8302_CURRENT_MIN_UA)) +
               (TEMP_RANGE_CENTI_C / 2)) /
              TEMP_RANGE_CENTI_C;

  return (uint32_t)scaled_uA;
}

static uint16_t CurrentMicroampsToGp8302Code(uint32_t current_uA)
{
  int64_t code;

  if (current_uA >= GP8302_CURRENT_FULL_SCALE_UA)
  {
    return GP8302_DAC_MAX_CODE;
  }

  code = ((int64_t)current_uA * GP8302_DAC_MAX_CODE) +
         (GP8302_CURRENT_FULL_SCALE_UA / 2);
  code /= GP8302_CURRENT_FULL_SCALE_UA;

  return (uint16_t)code;
}

static HAL_StatusTypeDef GP8302_WriteCode(uint16_t code)
{
  uint8_t tx[3];
  uint16_t shifted_code;

  code &= GP8302_DAC_MAX_CODE;
  shifted_code = (uint16_t)(code << 4);
  tx[0] = GP8302_OUTPUT_REG;
  tx[1] = (uint8_t)(shifted_code & 0xFFU);
  tx[2] = (uint8_t)(shifted_code >> 8);

  return HAL_I2C_Master_Transmit(&hi2c2,
                                 (uint16_t)(GP8302_I2C_ADDR_7BIT << 1),
                                 tx,
                                 sizeof(tx),
                                 GP8302_I2C_TIMEOUT_MS);
}

static HAL_StatusTypeDef GP8302_SetCurrent(uint32_t current_uA)
{
  return GP8302_WriteCode(CurrentMicroampsToGp8302Code(current_uA));
}

static HAL_StatusTypeDef CAN_SendTemperatureFrame(int32_t temp_centi_c,
                                                  uint16_t dac_code,
                                                  uint32_t current_uA,
                                                  AppStatus_t app_status)
{
  uint8_t tx_data[8];
  HAL_StatusTypeDef result;

  tx_data[0] = (uint8_t)((uint32_t)temp_centi_c >> 24);
  tx_data[1] = (uint8_t)((uint32_t)temp_centi_c >> 16);
  tx_data[2] = (uint8_t)((uint32_t)temp_centi_c >> 8);
  tx_data[3] = (uint8_t)((uint32_t)temp_centi_c);
  tx_data[4] = (uint8_t)(dac_code >> 8);
  tx_data[5] = (uint8_t)(dac_code);
  tx_data[6] = (uint8_t)(current_uA / 100U);
  tx_data[7] = (uint8_t)app_status;

  result = CAN_SendDataFrame(CAN_TEMP_STD_ID, tx_data, sizeof(tx_data));
  if (result == HAL_OK)
  {
    g_can_temp_count++;
  }

  return result;
}

static HAL_StatusTypeDef CAN_SendHeartbeatFrame(void)
{
  uint32_t count = g_can_heartbeat_count + 1U;
  uint8_t tx_data[8];
  HAL_StatusTypeDef result;

  tx_data[0] = 0x56U;
  tx_data[1] = 0x45U;
  tx_data[2] = 0x54U;
  tx_data[3] = 0x32U;
  tx_data[4] = (uint8_t)(count >> 24);
  tx_data[5] = (uint8_t)(count >> 16);
  tx_data[6] = (uint8_t)(count >> 8);
  tx_data[7] = (uint8_t)count;

  result = CAN_SendDataFrame(CAN_HEARTBEAT_STD_ID, tx_data, sizeof(tx_data));
  if (result == HAL_OK)
  {
    g_can_heartbeat_count = count;
  }

  return result;
}

static HAL_StatusTypeDef CAN_SendDiagnosticFrame(void)
{
  uint8_t tx_data[8];
  HAL_StatusTypeDef result;

  tx_data[0] = (uint8_t)g_app_status;
  tx_data[1] = (uint8_t)(g_sensor_status & 0xFFU);
  tx_data[2] = (uint8_t)(g_i2c_error_count & 0xFFU);
  tx_data[3] = (uint8_t)(g_can_error_count & 0xFFU);
  tx_data[4] = (uint8_t)((uint32_t)g_raw_temperature_centi_c >> 24);
  tx_data[5] = (uint8_t)((uint32_t)g_raw_temperature_centi_c >> 16);
  tx_data[6] = (uint8_t)((uint32_t)g_raw_temperature_centi_c >> 8);
  tx_data[7] = (uint8_t)((uint32_t)g_raw_temperature_centi_c);

  result = CAN_SendDataFrame(CAN_DIAG_STD_ID, tx_data, sizeof(tx_data));
  if (result == HAL_OK)
  {
    g_can_diag_count++;
  }

  return result;
}

static HAL_StatusTypeDef CAN_SendF103TestFrame(void)
{
  uint32_t count = g_can_f103_test_req_count + 1U;
  uint8_t tx_data[8];
  HAL_StatusTypeDef result;

  tx_data[0] = 0x46U; /* F */
  tx_data[1] = 0x34U; /* 4 */
  tx_data[2] = 0x30U; /* 0 */
  tx_data[3] = 0x37U; /* 7 */
  tx_data[4] = 0x54U; /* T */
  tx_data[5] = 0x53U; /* S */
  tx_data[6] = (uint8_t)count;
  tx_data[7] = (uint8_t)(count >> 8);

  result = CAN_SendDataFrame(CAN_F103_TEST_REQ_STD_ID, tx_data, sizeof(tx_data));
  if (result == HAL_OK)
  {
    g_can_f103_test_req_count = count;
  }

  return result;
}

static HAL_StatusTypeDef CAN_SendDataFrame(uint32_t std_id, const uint8_t *data, uint8_t dlc)
{
  CAN_TxHeaderTypeDef tx_header;
  uint32_t mailbox;
  uint32_t start_tick;
  uint32_t tsr;

  tx_header.StdId = std_id;
  tx_header.ExtId = 0U;
  tx_header.IDE = CAN_ID_STD;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.DLC = dlc;
  tx_header.TransmitGlobalTime = DISABLE;

  if (HAL_CAN_AddTxMessage(&hcan1, &tx_header, (uint8_t *)data, &mailbox) != HAL_OK)
  {
    g_can_last_hal_error = HAL_CAN_GetError(&hcan1);
    g_can_last_esr = hcan1.Instance->ESR;
    g_can_last_tsr = hcan1.Instance->TSR;
    return HAL_ERROR;
  }

  start_tick = HAL_GetTick();
  while (HAL_CAN_IsTxMessagePending(&hcan1, mailbox) != 0U)
  {
    if ((HAL_GetTick() - start_tick) > CAN_TX_TIMEOUT_MS)
    {
      g_can_last_hal_error = HAL_CAN_GetError(&hcan1);
      g_can_last_esr = hcan1.Instance->ESR;
      g_can_last_tsr = hcan1.Instance->TSR;
      HAL_CAN_AbortTxRequest(&hcan1, mailbox);
      return HAL_TIMEOUT;
    }
  }

  g_can_last_hal_error = HAL_CAN_GetError(&hcan1);
  g_can_last_esr = hcan1.Instance->ESR;
  tsr = hcan1.Instance->TSR;
  g_can_last_tsr = tsr;

  if (!CAN_TxMailboxOk(mailbox, tsr))
  {
    return HAL_ERROR;
  }

  g_can_tx_ok_count++;
  return HAL_OK;
}

static void CAN_ProcessReceivedFrames(void)
{
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8];

  while (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0U)
  {
    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK)
    {
      g_can_last_hal_error = HAL_CAN_GetError(&hcan1);
      g_can_last_esr = hcan1.Instance->ESR;
      g_can_last_tsr = hcan1.Instance->TSR;
      g_can_error_count++;
      return;
    }

    CAN_ProcessF103TestResponse(&rx_header, rx_data);
  }
}

static void CAN_ProcessF103TestResponse(const CAN_RxHeaderTypeDef *rx_header, const uint8_t *data)
{
  if ((rx_header->IDE != CAN_ID_STD) ||
      (rx_header->RTR != CAN_RTR_DATA) ||
      (rx_header->StdId != CAN_F103_TEST_RESP_STD_ID))
  {
    return;
  }

  if ((rx_header->DLC == 8U) &&
      (data[0] == 0x46U) &&
      (data[1] == 0x31U) &&
      (data[2] == 0x30U) &&
      (data[3] == 0x33U) &&
      (data[4] == 0x4FU) &&
      (data[5] == 0x4BU))
  {
    g_can_f103_test_resp_count++;
    g_can_f103_test_last_counter = (uint16_t)data[6] | ((uint16_t)data[7] << 8);
  }
  else
  {
    g_can_f103_test_bad_resp_count++;
  }
}

static void CAN_RecoverIfNeeded(void)
{
  static uint32_t last_recovery_tick = 0U;
  uint32_t now = HAL_GetTick();
  uint32_t esr = hcan1.Instance->ESR;
  uint32_t tsr = hcan1.Instance->TSR;
  uint32_t error = HAL_CAN_GetError(&hcan1);

  if (((esr & CAN_RECOVERABLE_ESR_MASK) == 0U) &&
      ((esr & CAN_ESR_LEC_MASK_APP) != CAN_ESR_LEC_ACK_APP) &&
      ((tsr & CAN_TSR_TX_ERROR_MASK_APP) == 0U) &&
      ((error & CAN_RECOVERABLE_HAL_ERROR_MASK) == 0U))
  {
    return;
  }

  if ((now - last_recovery_tick) < CAN_RECOVERY_INTERVAL_MS)
  {
    return;
  }

  last_recovery_tick = now;
  g_can_last_hal_error = error;
  g_can_last_esr = esr;
  g_can_last_tsr = tsr;

  (void)HAL_CAN_AbortTxRequest(&hcan1, CAN_TX_MAILBOX0 | CAN_TX_MAILBOX1 | CAN_TX_MAILBOX2);
  (void)HAL_CAN_Stop(&hcan1);
  (void)HAL_CAN_DeInit(&hcan1);
  MX_CAN1_Init();
  (void)HAL_CAN_ResetError(&hcan1);
}

static bool CAN_TxMailboxOk(uint32_t mailbox, uint32_t tsr)
{
  if ((mailbox & CAN_TX_MAILBOX0) != 0U)
  {
    return (tsr & CAN_TSR_TXOK0) != 0U;
  }

  if ((mailbox & CAN_TX_MAILBOX1) != 0U)
  {
    return (tsr & CAN_TSR_TXOK1) != 0U;
  }

  if ((mailbox & CAN_TX_MAILBOX2) != 0U)
  {
    return (tsr & CAN_TSR_TXOK2) != 0U;
  }

  return false;
}

/* 将 int32_t 值钳位到 [low, high] 范围内 */
static int32_t ClampInt32(int32_t value, int32_t low, int32_t high)
{
  if (value < low)
  {
    return low;
  }

  if (value > high)
  {
    return high;
  }

  return value;
}

/* 计算两个 int32_t 值的绝对差值 */
static int32_t AbsDiffInt32(int32_t a, int32_t b)
{
  return (a >= b) ? (a - b) : (b - a);
}

/*
 * 向 DAC8550 写入 16 位码值，重复写入 DAC8550_WRITE_REPEATS 次以提高可靠性。
 * DAC8550 为 16 位单通道电压输出 DAC，通过 3 线 SPI 接口控制。
 */
static void DAC8550_WriteCode(uint16_t code)
{
  for (uint8_t repeat = 0U; repeat < DAC8550_WRITE_REPEATS; repeat++)
  {
    DAC8550_WriteFrame(code);
  }
}

/*
 * 通过软件模拟 SPI 发送一帧到 DAC8550。
 * 时序：拉低 SYNC → 发送 3 字节 → 拉高 SYNC。
 * 3 字节 = 8 位控制字 (0x00) + 16 位 DAC 码值（高字节在前）。
 */
static void DAC8550_WriteFrame(uint16_t code)
{
  HAL_GPIO_WritePin(DAC_SYNC_GPIO_Port, DAC_SYNC_Pin, GPIO_PIN_RESET);
  DAC8550_BitDelay();

  DAC8550_WriteByte(0x00U);
  DAC8550_WriteByte((uint8_t)(code >> 8));
  DAC8550_WriteByte((uint8_t)(code & 0xFFU));

  DAC8550_BitDelay();
  HAL_GPIO_WritePin(DAC_SYNC_GPIO_Port, DAC_SYNC_Pin, GPIO_PIN_SET);
  DAC8550_BitDelay();
}

/*
 * 软件模拟 SPI 发送单字节（MSB 优先）。
 * 时序：设置 DIN → 延时 → SCLK 上升沿 → 延时 → SCLK 下降沿 → 延时。
 */
static void DAC8550_WriteByte(uint8_t value)
{
  for (uint8_t mask = 0x80U; mask != 0U; mask >>= 1)
  {
    HAL_GPIO_WritePin(DAC_DIN_GPIO_Port, DAC_DIN_Pin,
                      ((value & mask) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    DAC8550_BitDelay();
    HAL_GPIO_WritePin(DAC_SCLK_GPIO_Port, DAC_SCLK_Pin, GPIO_PIN_SET);
    DAC8550_BitDelay();
    HAL_GPIO_WritePin(DAC_SCLK_GPIO_Port, DAC_SCLK_Pin, GPIO_PIN_RESET);
    DAC8550_BitDelay();
  }
}

/*
 * 软件模拟 SPI 位延时，通过 NOP 循环消耗 CPU 周期，
 * 满足 DAC8550 的建立/保持时间要求。
 */
static void DAC8550_BitDelay(void)
{
  for (volatile uint32_t i = 0U; i < DAC8550_BIT_DELAY_CYCLES; i++)
  {
    __NOP();
  }
}

/* PA6: 电压输出正常时翻转，形成闪烁 */
static void Led_SetVoltageOk(void)
{
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}

/* PA6: 电压输出异常时常亮 */
static void Led_SetVoltageFault(void)
{
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, LED_ON_STATE);
}

/* PA7: 电流输出正常时翻转，形成闪烁 */
static void Led_SetCurrentOk(void)
{
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
}

/* PA7: 电流输出异常时常亮 */
static void Led_SetCurrentFault(void)
{
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, LED_ON_STATE);
}

/* 两路输出都未进入正常工作时常亮 */
static void Led_SetAllFault(void)
{
  Led_SetVoltageFault();
  Led_SetCurrentFault();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
/**
  * @brief  程序入口。
  *          初始化 HAL 库 → 配置系统时钟 → 初始化 GPIO 和 UART →
  *          设置 DAC 零输出和故障 LED → 进入主循环轮询传感器。
  * @retval int
  */
int main(void)
{
  uint32_t last_can_heartbeat_tick = 0U;
  uint32_t last_can_diag_tick = 0U;
  uint32_t last_can_f103_test_tick = 0U;

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_CAN1_Init();

  DAC8550_WriteCode(DAC8550_ZERO_CODE);
  (void)GP8302_SetCurrent(GP8302_CURRENT_MIN_UA);
  Led_SetAllFault();

  while (1)
  {
    CAN_RecoverIfNeeded();
    App_RunOnce();
    CAN_ProcessReceivedFrames();

    if ((HAL_GetTick() - last_can_diag_tick) >= CAN_DIAG_INTERVAL_MS)
    {
      if (CAN_SendDiagnosticFrame() != HAL_OK)
      {
        g_can_error_count++;
      }
      last_can_diag_tick = HAL_GetTick();
    }

    if ((HAL_GetTick() - last_can_f103_test_tick) >= CAN_F103_TEST_INTERVAL_MS)
    {
      if (CAN_SendF103TestFrame() != HAL_OK)
      {
        g_can_error_count++;
      }
      last_can_f103_test_tick = HAL_GetTick();
    }

    if ((HAL_GetTick() - last_can_heartbeat_tick) >= CAN_HEARTBEAT_INTERVAL_MS)
    {
      if (CAN_SendHeartbeatFrame() != HAL_OK)
      {
        g_can_error_count++;
      }
      last_can_heartbeat_tick = HAL_GetTick();
    }
    CAN_ProcessReceivedFrames();
    if (MODBUS_POLL_INTERVAL_MS > 0U)
    {
      HAL_Delay(MODBUS_POLL_INTERVAL_MS);
    }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
/**
  * @brief  系统时钟配置：使用内部 HSI (16 MHz) 作为系统时钟源。
  *         HCLK = SYSCLK = APB1 = APB2 = 16 MHz，Flash 延迟 = 0 等待周期。
  *         不使用 PLL，以降低功耗和简化配置。
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
/**
  * @brief  USART1 初始化：115200-8N1，无流控，收发模式。
  *         用于与温度传感器的 Modbus RTU 通信。
  * @param  None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
  __HAL_RCC_USART1_CLK_ENABLE();

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C2_Init(void)
{
  __HAL_RCC_I2C2_CLK_ENABLE();

  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_CAN1_Init(void)
{
  CAN_FilterTypeDef filter;

  __HAL_RCC_CAN1_CLK_ENABLE();

  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 2;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }

  filter.FilterBank = 0;
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterIdHigh = 0x0000;
  filter.FilterIdLow = 0x0000;
  filter.FilterMaskIdHigh = 0x0000;
  filter.FilterMaskIdLow = 0x0000;
  filter.FilterFIFOAssignment = CAN_RX_FIFO0;
  filter.FilterActivation = ENABLE;
  filter.SlaveStartFilterBank = 14;
  if (HAL_CAN_ConfigFilter(&hcan1, &filter) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
/**
 * @brief  GPIO 初始化：
 *         - PA6, PA7: LED 输出（推挽）
 *         - PB12 (SYNC), PB13 (SCLK), PB15 (DIN): DAC8550 SPI 控制线（推挽）
 *         - PB10 (I2C2_SCL), PB11 (I2C2_SDA): GP8302 I2C2
 *         - PA9, PA10: USART1 TX/RX（复用推挽 + 上拉）
 *         - PD0 (CAN1_RX), PD1 (CAN1_TX): SN65HVD230 CAN1
 *         - PE3, PE4: 预留数字输入（上拉）
 * @param  None
 * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  HAL_GPIO_WritePin(LED1_GPIO_Port, LED_Pins, LED_OFF_STATE);
  HAL_GPIO_WritePin(DAC_SYNC_GPIO_Port, DAC_SYNC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DAC_SCLK_GPIO_Port, DAC_SCLK_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DAC_DIN_GPIO_Port, DAC_DIN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CAN1_TX_GPIO_Port, CAN1_TX_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = LED_Pins;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DAC_Pins;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GP8302_I2C_Pins;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CAN1_Pins;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
/**
  * @brief  错误处理函数：发生不可恢复错误时调用。
  *         禁用全局中断并进入死循环，等待看门狗复位或调试器介入。
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;
}
#endif /* USE_FULL_ASSERT */
