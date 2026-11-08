/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "stdio.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MODE_ONE   1
#define MODE_TWO   0
#define MODE_THREE 0
#define MODE_FOUR  0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t  RoboMaster[]        = "RoboMaster 2027\r\n";

#if MODE_TWO

uint8_t  rx_byte_2;

#endif

#if MODE_THREE

uint8_t  tx_buffer[2] = {'Q', 'Y'};
uint8_t  rx_buffer[2] = {0};
uint64_t rx_timeout     =  0 ;
bool     rx_flag        = false;

#endif

#if MODE_FOUR

uint64_t tick            = 0;
float    vofa_channel[2] = {0.0f, 0.0f};

#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if MODE_TWO

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    if(rx_byte_2 == 'r'){
      HAL_UART_Transmit_IT(&huart1, RoboMaster, sizeof(RoboMaster));
      HAL_GPIO_TogglePin(Sign_GPIO_Port, Sign_Pin);
    }

    HAL_UART_Receive_IT(&huart1, &rx_byte_2, sizeof(rx_byte_2));
  }
}

#endif

#if MODE_THREE

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    rx_flag    = true;
    rx_timeout = 0;

    HAL_UART_Transmit_IT(&huart1, rx_buffer, sizeof(rx_buffer));

    HAL_UART_Receive_IT(&huart1, rx_buffer, sizeof(rx_buffer));
  }
}

#endif

#if MODE_FOUR

int _write(int file, char *ch, int len)
{
    (void)file;
    if(HAL_UART_GetState(&huart3) == HAL_UART_STATE_READY) {
        HAL_UART_Transmit(&huart3, (uint8_t *)ch, (uint16_t)len, 0xFFFF);
    }
    return len;
}

#endif

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  #if MODE_TWO

  HAL_UART_Receive_IT(&huart1, &rx_byte_2, sizeof(rx_byte_2));

  #endif

  #if MODE_THREE

  HAL_UART_Receive_IT(&huart1, rx_buffer, sizeof(rx_buffer));

  #endif

  #if MODE_FOUR

  setvbuf(stdout, NULL, _IONBF, 0);

  #endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    #if MODE_ONE

    uint8_t rx_byte;
    if(HAL_UART_Receive(&huart1, &rx_byte, 1, 0xFFFF) == HAL_OK){
      if(rx_byte == 'r'){
        HAL_UART_Transmit(&huart1, RoboMaster, sizeof(RoboMaster), 0xFFFF);
        HAL_GPIO_TogglePin(Sign_GPIO_Port, Sign_Pin);
      }
    }

    #endif

    #if MODE_THREE

    HAL_UART_Transmit_IT(&huart2, tx_buffer, sizeof(tx_buffer));

    if(rx_flag){
      HAL_GPIO_TogglePin(Sign_GPIO_Port, Sign_Pin);
    }

    if(rx_timeout++ > 10){
      rx_flag = false; 
      rx_timeout = 10;
    }

    HAL_Delay(100);

    #endif

    #if MODE_FOUR

    float x_axis = 0.0f;

    tick++;

    vofa_channel[0] = sin(2 * 3.1415926 * 0.5 * tick / 1000);
    vofa_channel[1] = cos(2 * 3.1415926 * 0.5 * tick / 1000);

    printf("%.6f, %.6f, %.6f\r\n",x_axis, vofa_channel[0], vofa_channel[1]);

    if(tick % 100 == 0){
      HAL_GPIO_TogglePin(Sign_GPIO_Port, Sign_Pin);
    }
    HAL_Delay(1);

    #endif

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
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
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
#ifdef USE_FULL_ASSERT
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
