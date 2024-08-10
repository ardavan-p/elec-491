/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32u0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef struct {
  // byte [0]
  uint8_t node_id;
  // bytes [1:2]
  uint16_t pressure;
  // bytes [3:4]
  int16_t temperature;
  // bytes [5:6]
  uint16_t msg_id;
  uint8_t rsvd;
} __attribute__((packed, aligned(1))) sensor_msg_t;

_Static_assert(sizeof(sensor_msg_t) == 8, "struct not correct size");

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
#define USER_LED_Pin GPIO_PIN_15
#define USER_LED_GPIO_Port GPIOC
#define PSENSE_OUT_Pin GPIO_PIN_0
#define PSENSE_OUT_GPIO_Port GPIOA
#define PSENSE_VREF_Pin GPIO_PIN_1
#define PSENSE_VREF_GPIO_Port GPIOA
#define PSENSE_MINUS_Pin GPIO_PIN_3
#define PSENSE_MINUS_GPIO_Port GPIOA
#define PSENSE_PLUS_Pin GPIO_PIN_4
#define PSENSE_PLUS_GPIO_Port GPIOA
#define P2110B_DOUT_Pin GPIO_PIN_6
#define P2110B_DOUT_GPIO_Port GPIOA
#define P2110B_DSET_Pin GPIO_PIN_9
#define P2110B_DSET_GPIO_Port GPIOA
#define P2110B_INT_Pin GPIO_PIN_10
#define P2110B_INT_GPIO_Port GPIOA
#define P2110B_RESET_Pin GPIO_PIN_11
#define P2110B_RESET_GPIO_Port GPIOA
#define NRF_IRQ_Pin GPIO_PIN_15
#define NRF_IRQ_GPIO_Port GPIOA
#define NRF_SCK_Pin GPIO_PIN_3
#define NRF_SCK_GPIO_Port GPIOB
#define NRF_MISO_Pin GPIO_PIN_4
#define NRF_MISO_GPIO_Port GPIOB
#define NRF_MOSI_Pin GPIO_PIN_5
#define NRF_MOSI_GPIO_Port GPIOB
#define NRF_CSN_Pin GPIO_PIN_6
#define NRF_CSN_GPIO_Port GPIOB
#define NRF_CE_Pin GPIO_PIN_7
#define NRF_CE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#ifndef SENSOR_NODE_ID_ONE
#ifndef SENSOR_NODE_ID_TWO
#error "No sensor node IDs defined!"
#endif  // !SENSOR_NODE_ID_TWO
#endif  // !SENSOR_NODE_ID_ONE

#ifdef SENSOR_NODE_ID_ONE
#ifdef SENSOR_NODE_ID_TWO
#error "Both sensor node IDs defined"
#endif  // SENSOR_NODE_ID_TWO
#endif  // SENSOR_NODE_ID_ONE

#ifdef SENSOR_NODE_ID_ONE
#define SENSOR_NODE_ID SENSOR_NODE_ID_ONE
#endif  // SENSOR_NODE_ID_ONE

#ifdef SENSOR_NODE_ID_TWO
#define SENSOR_NODE_ID SENSOR_NODE_ID_TWO
#endif  // SENSOR_NODE_ID_TWO

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
