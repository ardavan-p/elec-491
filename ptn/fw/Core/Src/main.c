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
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adf4351.h"
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

  // Set up initial state for SPI IO pins:
  HAL_GPIO_WritePin(SPI1_CSn_GPIO_Port, SPI1_CSn_Pin, 1);
  HAL_GPIO_WritePin(SPI1_LD_GPIO_Port, SPI1_LD_Pin, 1);
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  cmd_sm_init();
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  // Power up the PLL:
  adf4350_out_altvoltage0_powerdown(0);
  
  while (1)
  {
    HAL_Delay(100);
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
