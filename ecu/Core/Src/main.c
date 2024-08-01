/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "gpio.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define UART_TIMEOUT_MS (200)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

const CAN_TxHeaderTypeDef tx_header = {
    .StdId = PTN_REQUEST_ID,
    .ExtId = 0x0000,
    .IDE = CAN_ID_STD,
    .RTR = CAN_RTR_DATA,
    .DLC = CAN_MSG_FRAME_LEN_BYTES,
    .TransmitGlobalTime = DISABLE,
};

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  HAL_StatusTypeDef status = HAL_OK;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  CAN_FilterTypeDef can_filter = {
      .FilterIdHigh = (uint32_t)(PTN_RESPONSE_ID << 5 | 0x0000),
      .FilterIdLow = 0,
      .FilterMaskIdHigh = 0,
      .FilterMaskIdLow = 0,
      .FilterFIFOAssignment = CAN_FILTER_FIFO0,
      .FilterBank = 0,
      .FilterMode = CAN_FILTERMODE_IDLIST,
      .FilterScale = CAN_FILTERSCALE_32BIT,
      .FilterActivation = CAN_FILTER_ENABLE,
      .SlaveStartFilterBank = 0,
  };

  status |= HAL_CAN_ConfigFilter(&hcan, &can_filter);
  status |= HAL_CAN_Start(&hcan);

  uint8_t can_tx_payload[CAN_MSG_FRAME_LEN_BYTES] = {0};
  uint8_t can_rx_payload[CAN_MSG_FRAME_LEN_BYTES] = {0};

  HAL_StatusTypeDef can_tx_status = HAL_OK;
  HAL_StatusTypeDef can_rx_status = HAL_OK;

  uint32_t mailbox = 0;

  CAN_RxHeaderTypeDef rx_header = {0};

  uint8_t uart_msg[256] = {0};

  sprintf((char *)uart_msg, "Finished initialization!\r\n");
  HAL_UART_Transmit(&huart1, uart_msg, sizeof(uart_msg), UART_TIMEOUT_MS);
  memset((void *)uart_msg, 0, 256);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) > 0) {
      can_tx_status =
          HAL_CAN_AddTxMessage(&hcan, &tx_header, can_tx_payload, &mailbox);
      if (can_tx_status == HAL_OK) {
        sprintf((char *)uart_msg, "Successfully transmitted CAN message!\r\n");
        HAL_UART_Transmit(&huart1, uart_msg, sizeof(uart_msg), UART_TIMEOUT_MS);
        memset((void *)uart_msg, 0, 256);
        break;
      } else {
        sprintf((char *)uart_msg, "Failed to transmit CAN message\r\n");
        HAL_UART_Transmit(&huart1, uart_msg, sizeof(uart_msg), UART_TIMEOUT_MS);
        memset((void *)uart_msg, 0, 256);
      }
    }
  }

  HAL_Delay(1000);
  sprintf((char *)uart_msg, "Waiting to receive message...\r\n");
  HAL_UART_Transmit(&huart1, uart_msg, sizeof(uart_msg), UART_TIMEOUT_MS);
  memset((void *)uart_msg, 0, 256);

  while (1) {
    if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) > 0) {
      can_rx_status =
          HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rx_header, can_rx_payload);
      sprintf((char *)uart_msg, "Received CAN message with ID = 0x%x!\r\n",
              rx_header.StdId);
      HAL_UART_Transmit(&huart1, uart_msg, sizeof(uart_msg), UART_TIMEOUT_MS);
      memset((void *)uart_msg, 0, 256);
      break;
    }
  }

  while (1)
    ;
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state
   */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
  number,
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
