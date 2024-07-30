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

#include "nrf24l01.h"
#include "stm32u0xx_hal_gpio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define SINGLE_TRANSMISSION 1
#define TX_DELAY_MS         200
#define NUM_MSGS            1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static uint8_t uart_msg[128] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void read_back_config(void);

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

  // TODO: use interrupts to avoid polling

  // TODO: make these static
  uint8_t tx_payload[MAX_DATA_BYTES] = {0};
  uint8_t rx_buffer[MAX_DATA_BYTES] = {0};

  uint8_t rf_payload[PAYLOAD_SZ_BYTES] = {0};

  HAL_StatusTypeDef send_status = HAL_ERROR;

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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  HAL_PWREx_EnableLowPowerRunMode();
  HAL_ADC_Start(&hadc1);

  HAL_GPIO_WritePin(P2110B_RESET_GPIO_Port, P2110B_RESET_Pin, GPIO_PIN_RESET);

  // TODO: figure out how many SPI commands can be cut out

  // sprintf((char *)uart_msg, "Starting initialization...\r\n");
  // HAL_UART_Transmit(&huart2, uart_msg, sizeof(uart_msg), UART_TIMEOUT_MS);

  // transmit a test command to bring CLK low (not sure why the clock is high to
  // begin with)
  tx_spi_cmd(R_REGISTER(CONFIG_REGISTER), NULL, 0);

  // HAL_Delay(100);

  // [REQUIRED] make sure to power on the device
  tx_payload[0] = 0b00001110;
  tx_spi_cmd(W_REGISTER(CONFIG_REGISTER), tx_payload, 1);

  // [REQUIRED] set payload size for pipe 0
  tx_payload[0] = PAYLOAD_SZ_BYTES;
  tx_spi_cmd(W_REGISTER(RX_PW_P0), tx_payload, 1);

  // [REQUIRED] set the RF configuration
  NrfRfSetup_t rf_config = {.data_power = ZERO_DBM, .data_rate = TWO_MBPS};
  nrf24l01_setup_rf(&rf_config);

  // [NOT REQUIRED] disable auto-retransmission
  tx_payload[0] = 0x0;
  tx_spi_cmd(W_REGISTER(SETUP_RETR), tx_payload, 1);

  // [REQUIRED] enable `W_TX_PAYLOAD_NOACK` command
  tx_payload[0] = 0x1;
  tx_spi_cmd(W_REGISTER(FEATURE), tx_payload, 1);

  // [NOT REQUIRED] flush the TX buffer
  tx_spi_cmd(FLUSH_TX, NULL, 0);

  // [OPTIONAL] read back configuration
  uint32_t start = HAL_GetTick();
  read_back_config();
  uint32_t end = HAL_GetTick();

  // int index = 0;

  // load pressure value into the RF payload
  // TODO: remove this
  uint32_t pressure_val = HAL_ADC_GetValue(&hadc1);

  uint16_t pressure_val_lo = pressure_val & 0xffff;
  *((uint16_t *)rf_payload) = pressure_val_lo;

  send_status = nrf24l01_send_msg_noack(rf_payload, PAYLOAD_SZ_BYTES);

  HAL_GPIO_WritePin(P2110B_RESET_GPIO_Port, P2110B_RESET_Pin, GPIO_PIN_SET);

  while (1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // index++;

    // #if SINGLE_TRANSMISSION
    //     break;
    // #endif /* if SINGLE_TRANSMISSION */

    if (send_status == HAL_OK) {
      // sprintf((char *)uart_msg, "Transmitted pressure value of %d!\r\n",
      //         pressure_val);
      // HAL_UART_Transmit(&huart2, uart_msg, sizeof(uart_msg),
      // UART_TIMEOUT_MS);
    } else {
      // sprintf((char *)uart_msg, "Failed to transmit pressure value!\r\n");
      // HAL_UART_Transmit(&huart2, uart_msg, sizeof(uart_msg),
      // UART_TIMEOUT_MS);
    }

    memset(&uart_msg, 0, sizeof(uart_msg));

    // check TX FIFO status
    tx_rx_spi_cmd(R_REGISTER(OBSERVE_TX), NULL, 0, rx_buffer, 1);
    uint8_t observe_tx = rx_buffer[0];
    memset(&rx_buffer, 0, sizeof(rx_buffer));

    // sprintf((char *)uart_msg, "Observed TX value is %d!\r\n", observe_tx);
    // HAL_UART_Transmit(&huart2, uart_msg, sizeof(uart_msg), UART_TIMEOUT_MS);

    HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
    HAL_Delay(TX_DELAY_MS);
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_8;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void read_back_config(void) {
  uint8_t rx_buffer[RX_BUF_SZ_BYTES] = {0};

  tx_rx_spi_cmd(R_REGISTER(CONFIG_REGISTER), NULL, 0, rx_buffer, 1);
  uint8_t config = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(R_REGISTER(EN_RXADDR), NULL, 0, rx_buffer, 1);
  uint8_t endp = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(R_REGISTER(SETUP_RETR), NULL, 0, rx_buffer, 1);
  uint8_t setup_retr = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(R_REGISTER(RF_CH), NULL, 0, rx_buffer, 1);
  uint8_t rfch = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(R_REGISTER(RF_SETUP), NULL, 0, rx_buffer, 1);
  uint8_t rfsetup = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(R_REGISTER(RX_ADDR_P0), NULL, 0, rx_buffer, 5);
  uint32_t rx_addr_p0_hi = *((uint32_t *)rx_buffer);
  uint32_t rx_addr_p0_lo = *((uint32_t *)rx_buffer + 1);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(R_REGISTER(TX_ADDR), NULL, 0, rx_buffer, 5);
  uint32_t tx_addr_hi = *((uint32_t *)rx_buffer);
  uint32_t tx_addr_lo = *((uint32_t *)rx_buffer + 1);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(R_REGISTER(RX_PW_P0), NULL, 0, rx_buffer, 5);
  uint8_t rx_pw_p0 = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(R_REGISTER(FEATURE), NULL, 0, rx_buffer, 1);
  uint8_t feature = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  // TODO: assert correct configuration

  // sprintf((char *)uart_msg, "CONFIG: 0x%x\r\n", config);
  // HAL_UART_Transmit(&huart2, uart_msg, strlen((const char *)uart_msg),
  //                   UART_TIMEOUT_MS);
  //
  // sprintf((char *)uart_msg, "ENABLED DATA PIPES: 0x%x\r\n", endp);
  // HAL_UART_Transmit(&huart2, uart_msg, strlen((const char *)uart_msg),
  //                   UART_TIMEOUT_MS);
  //
  // sprintf((char *)uart_msg, "RETRANSMISSION SETUP: 0x%x\r\n", setup_retr);
  // HAL_UART_Transmit(&huart2, uart_msg, strlen((const char *)uart_msg),
  //                   UART_TIMEOUT_MS);
  //
  // sprintf((char *)uart_msg, "RF CHANNEL: 0x%x\r\n", rfch);
  // HAL_UART_Transmit(&huart2, uart_msg, strlen((const char *)uart_msg),
  //                   UART_TIMEOUT_MS);
  //
  // sprintf((char *)uart_msg, "RF SETUP: 0x%x\r\n", rfsetup);
  // HAL_UART_Transmit(&huart2, uart_msg, strlen((const char *)uart_msg),
  //                   UART_TIMEOUT_MS);
  //
  // sprintf((char *)uart_msg, "RX_ADDR_P0: 0x%x%x\r\n", rx_addr_p0_hi,
  //         rx_addr_p0_lo);
  // HAL_UART_Transmit(&huart2, uart_msg, strlen((const char *)uart_msg),
  //                   UART_TIMEOUT_MS);
  //
  // sprintf((char *)uart_msg, "TX_ADDR: 0x%x%x\r\n", tx_addr_hi, tx_addr_lo);
  // HAL_UART_Transmit(&huart2, uart_msg, strlen((const char *)uart_msg),
  //                   UART_TIMEOUT_MS);
  //
  // sprintf((char *)uart_msg, "RX_PW_P0: 0x%x\r\n", rx_pw_p0);
  // HAL_UART_Transmit(&huart2, uart_msg, strlen((const char *)uart_msg),
  //                   UART_TIMEOUT_MS);
  //
  // sprintf((char *)uart_msg, "FEATURE: 0x%x\r\n", feature);
  // HAL_UART_Transmit(&huart2, uart_msg, strlen((const char *)uart_msg),
  //                   UART_TIMEOUT_MS);
  //
  // sprintf((char *)uart_msg, "Finished initialization!\r\n");
  // HAL_UART_Transmit(&huart2, uart_msg, sizeof(uart_msg), UART_TIMEOUT_MS);
}

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
  /* User can add his own implementation to report the file name and line
  number,
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
