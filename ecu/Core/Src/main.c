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
#include "stm32f1xx_hal_gpio.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// Amount of time to keep trying to send a PTN_REQUEST message.
#define PTN_REQUEST_TIMEOUT_MS (1000)

// Amount of time to poll for a CAN message response from a PTN.
#define PTN_RESPONSE_TIMEOUT_MS (10000)

// Amount of time to keep trying to send a PTN_RESET message.
#define PTN_RESET_TIMEOUT_MS (3000)

// Amount of time between consecutive PTN_RESET messages to different PTNs.
#define CONSECUTIVE_RESET_MSG_DELAY_MS (50)

#define UART_TIMEOUT_MS (200)

#define CAN_TX_TO_RX_DELAY_MS (1000)

// Amount of time between attempting consecutive PTN read.
#define PTN_READ_DELAY_MS (5000)

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

void print_ecu_state(EcuState_e state);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

const CAN_TxHeaderTypeDef req_header = {
    .StdId = PTN_REQUEST_ID,
    .ExtId = 0x0000,
    .IDE = CAN_ID_STD,
    .RTR = CAN_RTR_DATA,
    .DLC = CAN_MSG_FRAME_LEN_BYTES,
    .TransmitGlobalTime = DISABLE,
};

const CAN_TxHeaderTypeDef reset_header = {
    .StdId = PTN_RESET_ID,
    .ExtId = 0x0000,
    .IDE = CAN_ID_STD,
    .RTR = CAN_RTR_DATA,
    .DLC = CAN_MSG_FRAME_LEN_BYTES,
    .TransmitGlobalTime = DISABLE,
};

const uint8_t ptn_list[NUM_PTNS] = {0x1, 0x2};

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  HAL_StatusTypeDef status = HAL_OK;
  EcuState_e ecu_state = ECU_RESET;

  AllPtnState_t all_ptns_state = {0};

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

  // send the RESET message to the GUI
  printf("reset,reset,reset\r\n");

  print_ecu_state(ecu_state);

  // delay between the reset message and actually starting to send CAN messages
  // to PTNs
  HAL_Delay(5000);

  setvbuf(stdout, NULL, _IONBF, 0);

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
  uint32_t mailbox = 0;
  CAN_RxHeaderTypeDef rx_header = {0};

  uint8_t uart_msg[256] = {0};

  ecu_state = ECU_AUTOLOCALIZATION;
  print_ecu_state(ecu_state);

  for (uint8_t idx = 0; idx < NUM_PTNS; idx++) {
    uint8_t cur_ptn_id = ptn_list[idx];

    printf("Attempting to send PTN_RESET CAN message (ID=0x%x) to PTN ID = "
           "0x%x...\r\n",
           reset_header.StdId, cur_ptn_id);

    status = HAL_CAN_AddTxMessagePolling(&hcan, &reset_header, can_tx_payload,
                                         &mailbox, PTN_RESET_TIMEOUT_MS);

    switch (status) {
    case HAL_OK: {
      printf("[PTN: 0x%x]: HAL_OK, successfully transmitted CAN message "
             "(ID=0x%x) to PTN ID = 0x%x\r\n",
             cur_ptn_id, reset_header.StdId, cur_ptn_id);
      break;
    }
    case HAL_TIMEOUT: {
      printf("[PTN: 0x%x]: status HAL_TIMEOUT (0x%x) returned when trying to "
             "send CAN message (ID=0x%x)!\r\n",
             cur_ptn_id, status, reset_header.StdId);
      break;
    }

    case HAL_ERROR: {
      printf("[PTN: 0x%x]: status HAL_ERROR (0x%x) returned when trying to "
             "send CAN message (ID=0x%x)!\r\n",
             cur_ptn_id, status, reset_header.StdId);
      break;
    }

    case HAL_BUSY: {
      printf("[PTN: 0x%x]: status HAL_BUSY (0x%x) returned when trying to "
             "send CAN message (ID=0x%x)!\r\n",
             cur_ptn_id, status, reset_header.StdId);
      break;
    }

    default: {
      printf("[PTN: 0x%x]: unexpected HAL status (0x%x) returned when trying "
             "to send CAN message (ID=0x%x)!\r\n",
             cur_ptn_id, status, reset_header.StdId);
      break;
    }
    }

    HAL_Delay(CONSECUTIVE_RESET_MSG_DELAY_MS);
  }

  printf("log,info,Finished initialization!\r\n");

  // On reset, ECU must:
  // 1) Send a RESET message to the PTNs
  // 2) Periodically send a request message.
  // 2) Wait for a timeout or a response message.
  // 3) If timeout move on.

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    for (uint8_t idx = 0; idx < NUM_PTNS; idx++) {
      uint8_t cur_ptn_id = ptn_list[idx];

      printf("Attempting to send PTN_REQUEST CAN message (ID=0x%x) to PTN ID = "
             "0x%x...\r\n",
             req_header.StdId, cur_ptn_id);

      // payload must contain the PTN ID it is targeting
      can_tx_payload[0] = cur_ptn_id;

      // TODO: add a CAN Rx Fifo flush here

      status = HAL_CAN_AddTxMessagePolling(&hcan, &req_header, can_tx_payload,
                                           &mailbox, PTN_REQUEST_TIMEOUT_MS);

      // if the CAN message doesn't transmit correctly, continue to sending
      // message for next PTN...
      if (status == HAL_OK) {
        printf("[PTN: 0x%x] Successfully transmitted CAN message with ID = "
               "0x%x!\r\n",
               cur_ptn_id, req_header.StdId);
      } else if (status == HAL_TIMEOUT) {
        printf("[PTN: 0x%x] Failed to transmit CAN message with ID = 0x%x due "
               "to timeout!\r\n",
               cur_ptn_id, req_header.StdId);
        continue;
      } else {
        printf("[PTN: 0x%x] Failed to transmit CAN message with ID = 0x%x, "
               "HAL_STATUS = "
               "%d!\r\n",
               cur_ptn_id, req_header.StdId, status);
        continue;
      }

      printf("[PTN: 0x%x] Waiting to receive PTN_RESPONSE CAN message with "
             "ID=0x%x...\r\n",
             cur_ptn_id, PTN_RESPONSE_ID);

      // CAN message transmitted correctly so now poll for CAN message
      // reception

      // NOTE: we need to do this in a while loop since we may have gotten
      // a response message from another PTN
      uint32_t start_time = HAL_GetTick();
      do {
        status = HAL_CAN_GetRxMessagePolling(&hcan, CAN_RX_FIFO0, &rx_header,
                                             can_rx_payload,
                                             PTN_RESPONSE_TIMEOUT_MS);

        // check if the PTN ID in the payload matches the one we're expecting
        PtnResponseMsg_t *response = (PtnResponseMsg_t *)(can_rx_payload);
        printf("[PTN: 0x%x] Received PTN_RESPONSE CAN message (ID=0x%x) from "
               "PTN=0x%x\r\n",
               cur_ptn_id, PTN_RESPONSE_ID, response->ptn_id);
        if (response->ptn_id == cur_ptn_id) {
          break;
        }
      } while (HAL_GetTick() - start_time <= PTN_RESPONSE_TIMEOUT_MS);

      switch (status) {
      case HAL_OK: {
        // parse out the CAN message
        PtnResponseMsg_t *response = (PtnResponseMsg_t *)(can_rx_payload);

        printf("[PTN: 0x%x]: status HAL_OK, response: PRESSURE = %d, TEMP = "
               "%d, PTN_ID = 0x%x, SN_ID = 0x%x, STATUS_CODE = 0x%x, PAIRED = "
               "%d\r\n",
               cur_ptn_id, response->pressure, response->temp, response->ptn_id,
               response->sn_id, response->status_code, response->paired);
        printf("ptn,%d,%d,%d,%d,%d,%d\r\n", cur_ptn_id, response->sn_id,
               response->pressure, response->temp, response->status_code,
               response->paired);

        // put information in the PTN state struct so we can check if
        // auto-localization is complete or not
        all_ptns_state.ptn_states[cur_ptn_id].paired = response->paired;
        all_ptns_state.ptn_states[cur_ptn_id].status_code = response->status_code;
        break;
      }
      case HAL_TIMEOUT: {
        printf("[PTN: 0x%x]: status HAL_TIMEOUT (0x%x) returned when trying to "
               "receive CAN message!\r\n",
               cur_ptn_id, status);
        break;
      }

      case HAL_ERROR: {
        printf("[PTN: 0x%x]: status HAL_ERROR (0x%x) returned when trying to "
               "receive CAN message!\r\n",
               cur_ptn_id, status);
        break;
      }

      case HAL_BUSY: {
        printf("[PTN: 0x%x]: status HAL_BUSY (0x%x) returned when trying to "
               "receive CAN message!\r\n",
               cur_ptn_id, status);
        break;
      }

      default: {
        printf("[PTN: 0x%x]: unexpected HAL STATUS (0x%x) returned when trying "
               "to receive CAN message!\r\n",
               cur_ptn_id, status);
      }
      }

      // check whether both tires are paired or not
      // if they are paired, autolocalization is complete...
      if (all_ptns_state.ptn_states[0].paired && all_ptns_state.ptn_states[1].paired)
      {
        ecu_state = ECU_NORMAL_OPERATION;
        print_ecu_state(ecu_state);
      }

      HAL_Delay(PTN_READ_DELAY_MS);
    }
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

void print_ecu_state(EcuState_e state) {
  printf("ecu,%d,null\r\n", state);
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
  /* e.g. write a character to the UART3 and Loop until the end of transmission
   */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

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
