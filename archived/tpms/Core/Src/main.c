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
#include "gpio.h"
#include "spi.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include "nrf2401.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static TxPayload txpayload = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  uint8_t test_msg[128] = {0};
  uint8_t rxbuffer[RX_BUF_SZ_BYTES] = {0};
  uint8_t cur_status = 0;

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  sprintf((char*)test_msg, "Starting initialization...\r\n");
  HAL_UART_Transmit(&huart2, test_msg, sizeof(test_msg), UART_TIMEOUT_MS);

  // transmit a test command to bring CLK low (not sure why the clock is high to begin with)
  txpayload.command = R_REGISTER(CONFIG_REGISTER);
  tx_cmd(&txpayload, 1);

  HAL_Delay(200);

  // configure NRF to be a receiver
  txpayload.command = W_REGISTER(CONFIG_REGISTER);
  txpayload.data[0] = 0x0F;
  tx_cmd(&txpayload, 2);

  // configure rx data pipe 0 with a payload size of 8
  txpayload.command = W_REGISTER(RX_PW_P0);
  txpayload.data[0] = 0x08;
  tx_cmd(&txpayload, 2);

  // flush the rx buffer on the NRF2401
  txpayload.command = FLUSH_RX;
  tx_cmd(&txpayload, 1);

  sprintf((char*)test_msg, "Flushed RX FIFO!\r\n");
  HAL_UART_Transmit(&huart2, test_msg, sizeof(test_msg), UART_TIMEOUT_MS);

  // read configuration back

  txpayload.command = R_REGISTER(CONFIG_REGISTER);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
  uint8_t config = rxbuffer[0];
  memset(&rxbuffer, 0, sizeof(rxbuffer));

  txpayload.command = R_REGISTER(EN_RXADDR);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
  uint8_t endp = rxbuffer[0];
  memset(&rxbuffer, 0, sizeof(rxbuffer));

  txpayload.command = R_REGISTER(RF_CH);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
  uint8_t rfch = rxbuffer[0];
  memset(&rxbuffer, 0, sizeof(rxbuffer));

  txpayload.command = R_REGISTER(RF_SETUP);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
  uint8_t rfsetup = rxbuffer[0];
  memset(&rxbuffer, 0, sizeof(rxbuffer));

  txpayload.command = R_REGISTER(RX_ADDR_P0);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 5);
  uint32_t rx_addr_p0_hi = *((uint32_t*)rxbuffer);
  uint32_t rx_addr_p0_lo = *((uint32_t*)rxbuffer + 1);
  memset(&rxbuffer, 0, sizeof(rxbuffer));

  txpayload.command = R_REGISTER(TX_ADDR);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 5);
  uint32_t tx_addr_hi = *((uint32_t*)rxbuffer);
  uint32_t tx_addr_lo = *((uint32_t*)rxbuffer + 1);
  memset(&rxbuffer, 0, sizeof(rxbuffer));

  txpayload.command = R_REGISTER(RX_PW_P0);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 5);
  uint8_t rx_pw_p0 = rxbuffer[0];
  memset(&rxbuffer, 0, sizeof(rxbuffer));

  sprintf((char*)test_msg, "CONFIG: 0x%x\r\n", config);
  HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);

  sprintf((char*)test_msg, "ENABLED DATA PIPES: 0x%x\r\n", endp);
  HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);

  sprintf((char*)test_msg, "RF CHANNEL: 0x%x\r\n", rfch);
  HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);

  sprintf((char*)test_msg, "RF SETUP: 0x%x\r\n", rfsetup);
  HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);

  sprintf((char*)test_msg, "RX_ADDR_P0: 0x%x%x\r\n", rx_addr_p0_hi, rx_addr_p0_lo);
  HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);

  sprintf((char*)test_msg, "TX_ADDR: 0x%x%x\r\n", tx_addr_hi, tx_addr_lo);
  HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);

  sprintf((char*)test_msg, "RX_PW_P0: 0x%x\r\n", rx_pw_p0);
  HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // check status to see if a message has arrived in the RX FIFO
    cur_status = nrf2401_get_status();

    // if new data arrives in RX FIFO... 
    if (STATUS_RX_DR(cur_status)) {
      // read the RX payload
      txpayload.command = R_RX_PAYLOAD;
      tx_rx_cmd(&txpayload, 1, rxbuffer, 9);
      sprintf((char*)test_msg, "Received message! pressure = %d\r\n", *((uint32_t*)rxbuffer));
      HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);

      // clear the data ready bit
      txpayload.command = W_REGISTER(STATUS);
      txpayload.data[0] = 0b01000000;
      tx_cmd(&txpayload, 2);
    } else {
      sprintf((char*)test_msg, "No message received!\r\n");
      HAL_UART_Transmit(&huart2, test_msg, strlen(test_msg), UART_TIMEOUT_MS);
    }

    HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
    HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);

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
  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
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
  /* User can add his own implementation to report the HAL error return state */
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
