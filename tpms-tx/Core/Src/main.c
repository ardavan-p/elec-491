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
#include "adc.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
int main(void)
{

  /* USER CODE BEGIN 1 */

  // SPI payloads
  static TxPayload txpayload = {0};
  uint8_t rxbuffer[RX_BUF_SZ_BYTES] = {0};

  // RF payloads
  uint8_t rf_payload[PAYLOAD_SZ_BYTES] = {0};

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */

  // transmit a test command to bring CLK low (not sure why the clock is high to
  // begin with)
  txpayload.command = R_REGISTER(CONFIG_REGISTER);
  tx_cmd(&txpayload, 1);

  HAL_Delay(1000);

  // make sure to power on the device
  txpayload.command = W_REGISTER(CONFIG_REGISTER);
  txpayload.data[0] = 0b00001110;
  tx_cmd(&txpayload, 2);

  // turn off auto re-transmission
  txpayload.command = W_REGISTER(SETUP_RETR);
  // txpayload.data[0] = 0x0; /* disables auto re-transmission */
  txpayload.data[0] = 0x3; /* default value */
  tx_cmd(&txpayload, 2);

  // flush the TX buffer
  txpayload.command = FLUSH_TX;
  tx_cmd(&txpayload, 1);

  // read back configuration
  
  txpayload.command = R_REGISTER(CONFIG_REGISTER);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
  uint8_t config = rxbuffer[0];

  txpayload.command = R_REGISTER(EN_RXADDR);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
  uint8_t en_rxaddr = rxbuffer[0];

  txpayload.command = R_REGISTER(RF_CH);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
  uint8_t rfch = rxbuffer[0];

  txpayload.command = R_REGISTER(RF_SETUP);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
  uint8_t rfsetup = rxbuffer[0];

  txpayload.command = R_REGISTER(SETUP_RETR);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 1);
  uint8_t retr_setup = rxbuffer[0];

  txpayload.command = R_REGISTER(RX_ADDR_P0);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 5);
  uint64_t rx_addr_p0 = *((uint64_t*)rxbuffer);
  
  txpayload.command = R_REGISTER(TX_ADDR);
  tx_rx_cmd(&txpayload, 1, rxbuffer, 5);
  uint64_t tx_addr = *((uint64_t*)rxbuffer);

  // assert correct configuration

  assert_param(config == 0b00001110);
  assert_param(en_rxaddr == 0b00000011);
  assert_param(rfch == 0b00000010);
  assert_param(rfsetup == 0b00001111);
  assert_param(rx_addr_p0 == 0xe7e7e7e7e7);
  assert_param(tx_addr == 0xe7e7e7e7e7);
  assert_param(retr_setup == 0x3);

  // setup the RF transmission payload
  // *((uint64_t*)rf_payload) = (uint64_t)(0xdeadbeef);

  HAL_ADC_Start(&hadc2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    // read the pressure sensor
    HAL_ADC_PollForConversion(&hadc2, 1000);
    uint32_t pressure_val = HAL_ADC_GetValue(&hadc2);
    *((uint32_t*)rf_payload) = pressure_val;
    
    // transmit a test payload of 8 bytes
    txpayload.command = W_TX_PAYLOAD;
    memcpy(txpayload.data, rf_payload, PAYLOAD_SZ_BYTES);
    tx_cmd(&txpayload, PAYLOAD_SZ_BYTES + 1);

    // check the TX FIFO status
    txpayload.command = R_REGISTER(FIFO_STATUS);
    tx_rx_cmd(&txpayload, 1, rxbuffer , 1);
    uint8_t fifo_status = rxbuffer[0];
    uint8_t tx_full = (fifo_status >> 5) & 0x1;

    // if (tx_full) {
    //   HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
    // }
    
    HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);

    HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */

  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);

  while (1);

  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
