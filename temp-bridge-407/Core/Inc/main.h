/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/*
 * LED 引脚定义
 * LED1: PA6 — 电压输出指示，正常闪烁，异常常亮
 * LED2: PA7 — 电流输出指示，正常闪烁，异常常亮
 * LED_Pins 位掩码支持同时操作两个 LED
 */
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOA
#define LED_Pins (LED1_Pin | LED2_Pin)

/*
 * DAC8550 软件模拟 SPI 引脚定义
 * SYNC  : PB12 — 片选/帧同步（低有效）
 * SCLK  : PB13 — 串行时钟
 * DIN   : PB15 — 串行数据输入
 */
#define DAC_SYNC_Pin GPIO_PIN_12
#define DAC_SYNC_GPIO_Port GPIOB
#define DAC_SCLK_Pin GPIO_PIN_13
#define DAC_SCLK_GPIO_Port GPIOB
#define DAC_DIN_Pin GPIO_PIN_15
#define DAC_DIN_GPIO_Port GPIOB
#define DAC_Pins (DAC_SYNC_Pin | DAC_SCLK_Pin | DAC_DIN_Pin)

/*
 * GP8302 4-20mA 电流输出模块预留引脚
 * I2C2_SCL: PB10
 * I2C2_SDA: PB11
 * 当前模块未接入，固件先按安全空闲态上拉输入预留。
 */
#define GP8302_I2C_SCL_Pin GPIO_PIN_10
#define GP8302_I2C_SCL_GPIO_Port GPIOB
#define GP8302_I2C_SDA_Pin GPIO_PIN_11
#define GP8302_I2C_SDA_GPIO_Port GPIOB
#define GP8302_I2C_Pins (GP8302_I2C_SCL_Pin | GP8302_I2C_SDA_Pin)

/*
 * SN65HVD230 CAN 收发器预留引脚
 * CAN1_RX: PD0, 接 SN65HVD230 RXD
 * CAN1_TX: PD1, 接 SN65HVD230 TXD
 * 当前模块未接入，固件先让 TXD 保持高电平（CAN recessive idle）。
 */
#define CAN1_RX_Pin GPIO_PIN_0
#define CAN1_RX_GPIO_Port GPIOD
#define CAN1_TX_Pin GPIO_PIN_1
#define CAN1_TX_GPIO_Port GPIOD
#define CAN1_Pins (CAN1_RX_Pin | CAN1_TX_Pin)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
