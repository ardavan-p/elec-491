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
#include "rng.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "nrf24l01.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define TRUE  (1)
#define FALSE (0)

#define BURST_MSG_NUM 3

#define REF_VOLTAGE_MEASUREMENT    (2250)
#define VOLTAGE_PRESSURE_SLOPE_ONE (0.2679)
#define VOLTAGE_PRESSURE_SLOPE_TWO (0.5611)

#define START_DELAY_MS             0
#define STOP_DELAY_MS              0
#define CONSECUTIVE_BURST_DELAY_MS 0
#define SETUP_TO_TX_DELAY_MS       1

#define ADC_TIMEOUT_MS 500

#define POWERCAST_RESET FALSE

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

void read_back_config(void);
uint16_t process_temp_val(uint32_t raw_val);

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

  uint8_t tx_payload[MAX_DATA_BYTES] = {0};
  uint8_t rf_payload[PAYLOAD_SZ_BYTES] = {0};
  uint8_t rx_buffer[MAX_DATA_BYTES] = {0};

  HAL_StatusTypeDef send_status = HAL_ERROR;
  HAL_StatusTypeDef adc_status = HAL_ERROR;

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
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_USART3_UART_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */

  // make sure RESET signal to power harvester is low
  HAL_GPIO_WritePin(P2110B_RESET_GPIO_Port, P2110B_RESET_Pin, GPIO_PIN_RESET);

  HAL_PWREx_EnableLowPowerRunMode();

  tx_spi_cmd(&hspi1, R_REGISTER(CONFIG_REGISTER), NULL, 0);

  HAL_Delay(START_DELAY_MS);

  // [REQUIRED] make sure to power on the device
  tx_payload[0] = 0b00001110;
  tx_spi_cmd(&hspi1, W_REGISTER(CONFIG_REGISTER), tx_payload, 1);

  // [REQUIRED] set payload size for pipe 0
  tx_payload[0] = PAYLOAD_SZ_BYTES;
  tx_spi_cmd(&hspi1, W_REGISTER(RX_PW_P0), tx_payload, 1);

  // [REQUIRED] set the RF configuration
  NrfRfSetup_t rf_config = {.data_power = ZERO_DBM, .data_rate = TWO_MBPS};
  nrf24l01_setup_rf(&hspi1, &rf_config);

  // [NOT REQUIRED] disable auto-retransmission
  tx_payload[0] = 0x0;
  tx_spi_cmd(&hspi1, W_REGISTER(SETUP_RETR), tx_payload, 1);

  // tx_payload[0] = 0x3;
  // tx_spi_cmd(W_REGISTER(SETUP_RETR), tx_payload, 1);

  // [REQUIRED] enable `W_TX_PAYLOAD_NOACK` command
  tx_payload[0] = 0x1;
  tx_spi_cmd(&hspi1, W_REGISTER(FEATURE), tx_payload, 1);

  // [NOT REQUIRED] flush the TX buffer
  tx_spi_cmd(&hspi1, FLUSH_TX, NULL, 0);

  // required when MCU clock speed is more than or equal to 24MHz
  HAL_Delay(STOP_DELAY_MS);

  HAL_ADC_Start(&hadc1);

  // measure the pressure using the ADC
  adc_status = HAL_ADC_PollForConversion(&hadc1, ADC_TIMEOUT_MS);
  uint32_t pressure_val = HAL_ADC_GetValue(&hadc1);

#if (SENSOR_NODE_ID == SENSOR_NODE_ID_ONE)
  double pressure_val_proc =
      (REF_VOLTAGE_MEASUREMENT - (pressure_val & 0xffff)) *
      VOLTAGE_PRESSURE_SLOPE_ONE;
#else
  // NOTE: reference voltage looks to be around 2250.
  double pressure_val_proc =
      (REF_VOLTAGE_MEASUREMENT - (pressure_val & 0xffff)) *
      VOLTAGE_PRESSURE_SLOPE_TWO;
#endif

  uint16_t pressure_val_final = (uint16_t)(pressure_val_proc);

  HAL_ADC_Stop(&hadc1);

  // generate random number so that each burst message has an ID
  uint32_t random_id = 0;
  HAL_RNG_GenerateRandomNumber(&hrng, &random_id);

  int16_t temperature = 22;

  if (random_id > (0xffffffff >> 1)) {
    temperature++;
  }

  sensor_msg_t message = {.node_id = SENSOR_NODE_ID,
                          .pressure = pressure_val_final,
                          .temperature = temperature,
                          .msg_id = random_id};

  // copy message into buffer
  memcpy((void *)rf_payload, (void *)(&message), PAYLOAD_SZ_BYTES);

  // send burst of RF messages
  for (int i = 0; i < BURST_MSG_NUM; i++) {
    send_status = nrf24l01_send_msg_noack(&hspi1, rf_payload, PAYLOAD_SZ_BYTES);
    HAL_Delay(CONSECUTIVE_BURST_DELAY_MS);
  }

  HAL_Delay(SETUP_TO_TX_DELAY_MS);

  // reset the power harvester so we don't consume all the stored power
#if (POWERCAST_RESET)
  HAL_GPIO_WritePin(P2110B_RESET_GPIO_Port, P2110B_RESET_Pin, GPIO_PIN_SET);
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

uint16_t process_temp_val(uint32_t raw_val)
{
  uint16_t cal1_val = *TEMPSENSOR_CAL1_ADDR;
  uint16_t cal2_val = *TEMPSENSOR_CAL2_ADDR;

  double slope = (double)(cal2_val - cal1_val) / (double)(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP);
  double intercept = TEMPSENSOR_CAL1_TEMP - (slope * cal1_val);

  return (uint16_t)(slope * raw_val + intercept);
}

void read_back_config(void) {
  uint8_t rx_buffer[RX_BUF_SZ_BYTES] = {0};

  tx_rx_spi_cmd(&hspi1, R_REGISTER(CONFIG_REGISTER), NULL, 0, rx_buffer, 1);
  uint8_t config = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi1, R_REGISTER(EN_RXADDR), NULL, 0, rx_buffer, 1);
  uint8_t endp = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi1, R_REGISTER(SETUP_RETR), NULL, 0, rx_buffer, 1);
  uint8_t setup_retr = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi1, R_REGISTER(RF_CH), NULL, 0, rx_buffer, 1);
  uint8_t rfch = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi1, R_REGISTER(RF_SETUP), NULL, 0, rx_buffer, 1);
  uint8_t rfsetup = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi1, R_REGISTER(RX_ADDR_P0), NULL, 0, rx_buffer, 5);
  uint32_t rx_addr_p0_hi = *((uint32_t *)rx_buffer);
  uint32_t rx_addr_p0_lo = *((uint32_t *)rx_buffer + 1);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi1, R_REGISTER(TX_ADDR), NULL, 0, rx_buffer, 5);
  uint32_t tx_addr_hi = *((uint32_t *)rx_buffer);
  uint32_t tx_addr_lo = *((uint32_t *)rx_buffer + 1);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi1, R_REGISTER(RX_PW_P0), NULL, 0, rx_buffer, 5);
  uint8_t rx_pw_p0 = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi1, R_REGISTER(FEATURE), NULL, 0, rx_buffer, 1);
  uint8_t feature = rx_buffer[0];
  memset(&rx_buffer, 0, sizeof(rx_buffer));
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
