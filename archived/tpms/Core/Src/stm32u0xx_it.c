/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32u0xx_it.c
 * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32u0xx_it.h"

#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "nrf2401.h"
#include "stm32u0xx_hal_gpio.h"
#include "usart.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

static TxPayload txpayload = {0};
static uint8_t rxbuffer[RX_BUF_SZ_BYTES] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1) {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1) {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
 * @brief This function handles System service call via SVC instruction.
 */
void SVC_Handler(void) {
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32U0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32u0xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles EXTI line 4 to 15 interrupts.
 */
void EXTI4_15_IRQHandler(void) {
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  static int num_rx_msgs = 0;
  static int num_uniq_msgs = 0;
  static uint32_t prev_rand_id = 0;

  uint8_t cur_status = 0;
  uint8_t rx_fifo_empty = 1;
  uint8_t fifo_status = 0;

  if (__HAL_GPIO_EXTI_GET_IT(NRF_IRQ_Pin)) {
    // interrupt fired so message exists in RX FIFO, skip check

    cur_status = nrf2401_get_status();

    do {
      // step 1: read the RX payload
      txpayload.command = R_RX_PAYLOAD;
      tx_rx_cmd(&txpayload, 1, rxbuffer, PAYLOAD_SZ_BYTES + 1);

      num_rx_msgs++;

      // unpack the message into its components
      uint32_t random_id = *(((uint32_t*)rxbuffer) + 0);
      uint32_t pressure_val = *(((uint32_t*)rxbuffer) + 1);

      if (random_id != prev_rand_id) {
        num_uniq_msgs++;
        prev_rand_id = random_id;
      }

      sprintf(
          (char*)test_msg,
          "[m: %d] [u: %d] Received message with id = 0x%x! pressure = %d\r\n",
          num_rx_msgs, num_uniq_msgs, random_id, pressure_val);
      HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);

      // clear out the RX buffer
      memset(&rxbuffer, 0, sizeof(rxbuffer));

      // step 2: clear the RX_DR IRQ bit
      txpayload.command = W_REGISTER(STATUS);
      txpayload.data[0] = 0b01000000;
      tx_cmd(&txpayload, 2);

      // step 3: check FIFO_STATUS to see if there's any more messages
      txpayload.command = R_REGISTER(FIFO_STATUS);
      tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
      fifo_status = rxbuffer[0];
      rx_fifo_empty = FIFO_STATUS_RX_EMPTY(fifo_status);

      HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);

    } while (!rx_fifo_empty);

    __HAL_GPIO_EXTI_CLEAR_IT(NRF_IRQ_Pin);
  }

  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(NRF_IRQ_Pin);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
