/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @author         : Ardavan Pourkeramati
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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adf4351.h"
#include "nrf24l01.h"
#include <stdio.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FHSS_NUM_FREQS (100)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const uint64_t fhss_freqs[FHSS_NUM_FREQS] = {
    915564235, 904440541, 903148600, 904227128, 917506829, 
    909397973, 908812571, 915449869, 919254899, 926507321, 
    925822447, 923067571, 903557369, 916691446, 912408403, 
    915808206, 903846361, 915402520, 906336386, 902037364, 
    909321452, 907231759, 918789091, 905750371, 923042615, 
    906372704, 912755720, 904459906, 906370088, 923731407, 
    903472249, 927657559, 912651500, 918619844, 925694767, 
    927554893, 919457550, 916626225, 906220683, 920235783, 
    921967008, 902299182, 911944043, 906843950, 926112279, 
    910403184, 923919218, 918063135, 924158523, 921292666, 
    904810996, 905714947, 913249837, 914091121, 925623877, 
    925355688, 906284260, 920917239, 917981303, 905593593, 
    917042242, 902399063, 923820062, 926307788, 909535172, 
    907539445, 925176093, 904984611, 909284218, 919224644, 
    905772047, 920586961, 916456393, 907262194, 916956907, 
    909592341, 924544342, 926980836, 922475339, 924512210, 
    926230896, 917236004, 912573921, 903178029, 924828246, 
    923296754, 916947099, 910847903, 926148369, 914389275, 
    915863526, 911243652, 904026491, 904906841, 912758865, 
    903310541, 907514837, 926261994, 927272190, 903953924
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void cmd_sm_init(void);
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

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  // Disable buffering for stdout:
  setvbuf(stdout, NULL, _IONBF, 0);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_SPI2_Init();
  MX_CAN_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  // Set up initial state for SPI IO pins:
  HAL_GPIO_WritePin(SPI1_CSn_GPIO_Port, SPI1_CSn_Pin, 1);
  HAL_GPIO_WritePin(SPI1_LD_GPIO_Port, SPI1_LD_Pin, 1);

  // PTN - SN pair status:
  bool ptn_sn_paired = false;
  uint8_t paired_sn_id = SN_ID_UNPAIRED;

  // PTN ID Detection:
  uint8_t ptn_id = 0;
  if (HAL_GPIO_ReadPin(PTN_ID_Port, PTN_ID_Pin) == 1)
  {
    ptn_id = PTN_1;
  }
  else
  {
    ptn_id = PTN_2;
  }
  
  // Initialize the ADF4351 and keep it off:
  cmd_sm_init();
  adf4350_out_altvoltage0_powerdown(1);
  HAL_GPIO_WritePin(PTN_RF_ON_LED_Port, PTN_RF_ON_LED_Pin, 0);

  // CAN Setup:
  CAN_FilterTypeDef can_filter = {
      .FilterIdHigh = (uint32_t)(PTN_REQUEST_ID << 5 | 0x0000),
      .FilterIdLow = 0,
      .FilterMaskIdHigh = (uint32_t)(PTN_RESET << 5 | 0x0000),
      .FilterMaskIdLow = 0,
      .FilterFIFOAssignment = CAN_FILTER_FIFO0,
      .FilterBank = 0,
      .FilterMode = CAN_FILTERMODE_IDLIST,
      .FilterScale = CAN_FILTERSCALE_32BIT,
      .FilterActivation = CAN_FILTER_ENABLE,
      .SlaveStartFilterBank = 0,
  };

  HAL_CAN_ConfigFilter(&hcan, &can_filter);

  HAL_CAN_Start(&hcan);

  CAN_TxHeaderTypeDef tx_header = {
      .StdId = PTN_RESPONSE_ID,
      .ExtId = 0,
      .IDE = CAN_ID_STD,
      .RTR = CAN_RTR_DATA,
      .DLC = CAN_MSG_FRAME_LEN_BYTES,
      .TransmitGlobalTime = DISABLE,
  };

  uint8_t can_tx_payload[CAN_MSG_FRAME_LEN_BYTES] = {0};
  uint8_t can_rx_payload[CAN_MSG_FRAME_LEN_BYTES] = {0};

  HAL_StatusTypeDef can_tx_status = HAL_OK;
  HAL_StatusTypeDef can_rx_status = HAL_OK;

  CAN_RxHeaderTypeDef rx_header = {0};
  uint32_t can_tx_mailbox = 0;


  // NRF24L01 Setup:
  uint8_t tx_payload[MAX_DATA_BYTES] = {0};
  uint8_t rf_payload[PAYLOAD_SZ_BYTES] = {0};

  // [REQUIRED] Make sure to power on the device
  tx_payload[0] = 0b00111111;
  tx_spi_cmd(&hspi2, W_REGISTER(CONFIG_REGISTER), tx_payload, 1);
  HAL_Delay(500);
  
  // [REQUIRED] Make sure to power on the device
  tx_payload[0] = 0b00111111;
  tx_spi_cmd(&hspi2, W_REGISTER(CONFIG_REGISTER), tx_payload, 1);

  // [REQUIRED] Set payload size for pipe 0
  tx_payload[0] = PAYLOAD_SZ_BYTES;
  tx_spi_cmd(&hspi2, W_REGISTER(RX_PW_P0), tx_payload, 1);

  // [REQUIRED] Set the RF configuration
  NrfRfSetup_t rf_config = {.data_power = ZERO_DBM, .data_rate = TWO_MBPS};
  nrf24l01_setup_rf(&hspi2, &rf_config);

  tx_spi_cmd(&hspi2, FLUSH_RX, NULL, 0);

  // Read back the configs (for debugging):
  read_back_config();

  // Initialize PTN status variable:
  uint8_t ptn_status = PTN_OK;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // Ensure that RF output is off:
    adf4350_out_altvoltage0_powerdown(1);
    HAL_GPIO_WritePin(PTN_RF_ON_LED_Port, PTN_RF_ON_LED_Pin, 0);

    // Wait for CAN message:
    printf("\n\rWaiting for CAN");
    while(HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) <= 0);
    printf("\n\rCAN received");
    can_rx_status = HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rx_header, can_rx_payload);

    // Decode the CAN message:
    const uint16_t ptn_can_id = rx_header.StdId;
    const uint16_t received_ptn_id = can_rx_payload[0];
    switch (ptn_can_id)
    {
      case (PTN_REQUEST_ID):
      {
        if (received_ptn_id == ptn_id)
        {
          goto request_handle;
        }
        else
        {
          goto end_while;
        }
      }
      break;

      case (PTN_RESET):
      {
        ptn_sn_paired = false;
        paired_sn_id = SN_ID_UNPAIRED;
        goto end_while;
      }
      break;

      default:
      {
        goto end_while;
      }
      break;
    }

request_handle:
    // NRF24L01 chip enable (CE):
    HAL_GPIO_WritePin(CHIP_ENABLE_GPIO_Port, CHIP_ENABLE_Pin, 1);
    tx_spi_cmd(&hspi2, FLUSH_RX, NULL, 0);

    // When CAN message received, turn on RF power and frequency hopping:
    printf("\n\rRF power ON");
    HAL_GPIO_WritePin(PTN_RF_ON_LED_Port, PTN_RF_ON_LED_Pin, 1);
    adf4350_out_altvoltage0_powerdown(0);
    HAL_TIM_Base_Start_IT(&htim2);

    // Prepare to receive data from NRF24L01:
    int num_rx_msgs = 0;
    int num_uniq_msgs = 0;
    uint32_t prev_rand_id = 0;

    uint8_t cur_status = 0;
    uint8_t rx_fifo_empty = 1;
    uint8_t fifo_status = 0;
    uint8_t rxbuffer[RX_BUF_SZ_BYTES] = {0};

    uint32_t ref_time = HAL_GetTick();
    uint32_t millis_time = HAL_GetTick() - ref_time;

    
    // Poll the NRF24L01 until data is received. Start a timeout timer so RF output is not on indefinitely:
    printf("\n\rWaiting for NRF");
    while((!STATUS_RX_DR(nrf24l01_get_status(&hspi2))) && millis_time < NRF24L01_RX_TIMEOUT)
    {
      millis_time = HAL_GetTick() - ref_time;
    }
  
    // Power down the RF output:
    printf("\n\rRF power OFF");
    HAL_TIM_Base_Stop_IT(&htim2);
    adf4350_out_altvoltage0_powerdown(1);
    HAL_GPIO_WritePin(PTN_RF_ON_LED_Port, PTN_RF_ON_LED_Pin, 0);

    // If timeout timer did not expire (i.e., NRF24L01 received data), then go ahead and store the payload:
    sensor_msg_t rx_msg = {0};
    if (millis_time < NRF24L01_RX_TIMEOUT)
    {
      printf("\n\rNRF received data");

      ptn_status = PTN_OK;

      do
      {
        // Step 1 - Read the RX payload:
        tx_rx_spi_cmd(&hspi2, R_RX_PAYLOAD, NULL, 0, rxbuffer, PAYLOAD_SZ_BYTES + 1);

        num_rx_msgs++;

        // Unpack the message into its components:
        memcpy((void*)(&rx_msg), (void*)(&rxbuffer), PAYLOAD_SZ_BYTES);

        if (rx_msg.msg_id != prev_rand_id)
        {
          num_uniq_msgs++;
          prev_rand_id = rx_msg.msg_id;
        }

        printf("[m: %d] [u: %d] Received message with id = 0x%04x, node_id = 0x%02x! pressure = %d, temp = %d\r\n",
            num_rx_msgs, num_uniq_msgs, rx_msg.msg_id, rx_msg.node_id, rx_msg.pressure, rx_msg.temperature);

        // Clear out the RX buffer:
        memset(&rxbuffer, 0, sizeof(rxbuffer));

        // Step 2 - Clear the RX_DR IRQ bit:
        tx_payload[0] = 0b01000000;
        tx_spi_cmd(&hspi2, W_REGISTER(STATUS), tx_payload, 1);

        // Step 3 - Check FIFO_STATUS to see if there's any more messages:
        tx_rx_spi_cmd(&hspi2, R_REGISTER(FIFO_STATUS), NULL, 0, rxbuffer, 1);
        fifo_status = rxbuffer[0];
        rx_fifo_empty = FIFO_STATUS_RX_EMPTY(fifo_status);

      } while (!rx_fifo_empty);

      // If the PTN is not paired to any SN, then pair the received SN to this PTN
      // The bool 'ptn_sn_paired' is only false after a reset command from the master ECU
      // 'ptn_sn_paired' will remain false unless a sensor node responds to this PTN 
      if (ptn_sn_paired == false)
      {
        paired_sn_id = rx_msg.node_id;
        ptn_sn_paired = true;
      }

      // Detect if there is a mismatch by comparing to the sensor node ID stored during pairing:
      if (paired_sn_id != rx_msg.node_id)
      {
        ptn_status = PTN_ERROR_MISMATCH;
      }
    }
    else  // Else, timeout timer expired, so set an error:
    {
      ptn_status = PTN_ERROR_TIMEOUT;
      printf("\n\rERROR: Timeout");
    }

    // NRF24L01 chip enable (CE):
    HAL_GPIO_WritePin(CHIP_ENABLE_GPIO_Port, CHIP_ENABLE_Pin, 0);

    // Pack and transmit CAN message:
    printf("\n\rSending back PTN RESPONSE");
    *((uint16_t*)can_tx_payload) = rx_msg.pressure;
    *((uint16_t*)can_tx_payload + 1) = rx_msg.temperature;
    *((uint8_t*)can_tx_payload + 4) = ptn_id;
    *((uint8_t*)can_tx_payload + 5) = rx_msg.node_id;
    *((uint8_t*)can_tx_payload + 6) = ptn_status;
    *((uint8_t*)can_tx_payload + 7) = (uint8_t)ptn_sn_paired;

    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) <= 0);
    can_tx_status = HAL_CAN_AddTxMessage(&hcan, &tx_header, can_tx_payload, &can_tx_mailbox);


end_while:
    // Ensure the RF output is off:
    adf4350_out_altvoltage0_powerdown(1);
    HAL_GPIO_WritePin(PTN_RF_ON_LED_Port, PTN_RF_ON_LED_Pin, 0);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
  static uint8_t i = 0;
  adf4350_out_altvoltage0_frequency(fhss_freqs[i]);
  i = (i + 1) % FHSS_NUM_FREQS;
}


adf4350_init_param pll_config;

void cmd_sm_init()
{
  // initialize pll_config structure
  pll_config.clkin = 25e6;
  pll_config.channel_spacing = 100;
  pll_config.power_up_frequency = 146e6;
  pll_config.reference_div_factor = 1;
  pll_config.reference_doubler_enable = 0;
  pll_config.reference_div2_enable = 0;
  pll_config.phase_detector_polarity_positive_enable = 1;
  pll_config.lock_detect_precision_6ns_enable = 0; // 10 ns
  pll_config.lock_detect_function_integer_n_enable = 0; // Fractional pll
  pll_config.charge_pump_current = 7; //2.50
  pll_config.muxout_select = 6; // Digital Lock Detect Out
  pll_config.low_spur_mode_enable = 1; // higher noise, lower spurs.
  pll_config.cycle_slip_reduction_enable = 0;
  pll_config.charge_cancellation_enable = 0;
  pll_config.anti_backlash_3ns_enable = 0;
  pll_config.band_select_clock_mode_high_enable = 0; // low
  pll_config.clk_divider_12bit = 0;
  pll_config.clk_divider_mode = 0;
  pll_config.aux_output_enable = 0;
  pll_config.aux_output_fundamental_enable = 0;
  pll_config.mute_till_lock_enable = 1;
  pll_config.output_power = 2; // +2 dBm
  pll_config.aux_output_power = ADF4541_POWER_N4_DBM;
  adf4350_out_altvoltage0_powerdown(0); // power down PLL
  adf4350_setup(pll_config);
}

void read_back_config(void)
{
  uint8_t rx_buffer[RX_BUF_SZ_BYTES] = {0};

  tx_rx_spi_cmd(&hspi2, R_REGISTER(CONFIG_REGISTER), NULL, 0, rx_buffer, 1);
  uint8_t config = rx_buffer[0];
  printf("\n\rConfig: %x", config);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi2, R_REGISTER(EN_RXADDR), NULL, 0, rx_buffer, 1);
  uint8_t endp = rx_buffer[0];
  printf("\n\rEnabled Data Pipes: %x", endp);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi2, R_REGISTER(SETUP_RETR), NULL, 0, rx_buffer, 1);
  uint8_t setup_retr = rx_buffer[0];
  printf("\n\rSetup Retry: %x", setup_retr);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi2, R_REGISTER(RF_CH), NULL, 0, rx_buffer, 1);
  uint8_t rfch = rx_buffer[0];
  printf("\n\rRF Channel: %x", rfch);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi2, R_REGISTER(RF_SETUP), NULL, 0, rx_buffer, 1);
  uint8_t rfsetup = rx_buffer[0];
  printf("\n\rRF Setup: %x", rfsetup);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi2, R_REGISTER(RX_ADDR_P0), NULL, 0, rx_buffer, 5);
  uint32_t rx_addr_p0_hi = *((uint32_t*)rx_buffer);
  uint32_t rx_addr_p0_lo = *((uint32_t*)rx_buffer + 1);
  printf("\n\rRX Address High: %lx\n\rRX Address Low: %lx", rx_addr_p0_hi, rx_addr_p0_lo);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi2, R_REGISTER(RX_PW_P0), NULL, 0, rx_buffer, 5);
  uint8_t rx_pw_p0 = rx_buffer[0];
  printf("\n\rRX_PW_P0: %x", rx_pw_p0);
  memset(&rx_buffer, 0, sizeof(rx_buffer));

  tx_rx_spi_cmd(&hspi2, R_REGISTER(FEATURE), NULL, 0, rx_buffer, 1);
  uint8_t feature = rx_buffer[0];
  printf("\n\rFeature: %x", feature);
  memset(&rx_buffer, 0, sizeof(rx_buffer));
}

#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the UART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
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
  while (1)
  {
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
