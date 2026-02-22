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
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"

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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */


/************** Create EventGroup and task handles *****************/
EventGroupHandle_t EventGroupTasks_handle;

const EventBits_t task01_bit = (1 << 0);
const EventBits_t ISR_bit = (1 << 1);

const EventBits_t combined_bits = (task01_bit | ISR_bit);

TaskHandle_t Task01_handle, WatchDogTask_handle;

//void Task01_Entry(void *arg){
//	static int count  = 1;
//	while(1){
//		// sets the task01 bits if not 3 to simulate failure
//		if(count % 3 == 0){
//			// do nothing, something failed, at multiples of 3
//			vTaskDelay(pdMS_TO_TICKS(1000));
//		}else{
//			xEventGroupSetBits(EventGroupTasks_handle, task01_bit);
//			vTaskDelay(pdMS_TO_TICKS(1000));
//		}
//		count++;
//	}
//}

void Task01_Entry(void *arg){
	while(1){
		// sets the task01 bits
		xEventGroupSetBits(EventGroupTasks_handle, task01_bit);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

//void Task03_Entry(void *arg){
//	static int count = 1;
//	while(1){
//		// sets the task01 bits, failure for every 5th bit
//		if(count % 5 == 0){
//			// do nothing
//			vTaskDelay(pdMS_TO_TICKS(1000));
//		}else{
//			xEventGroupSetBits(EventGroupTasks_handle, task03_bit);
//			vTaskDelay(pdMS_TO_TICKS(1000));
//		}
//		count++;
//	}
//}

//void WatchDogTask_Entry(void *arg){
//	while(1){
//		// wait for all the bits to be set and check until the timeout/wait time of 2000ms
//		EventBits_t uxBits = xEventGroupWaitBits(EventGroupTasks_handle, combined_bits, pdTRUE, pdTRUE, pdMS_TO_TICKS(2000));
//
//		if((uxBits & combined_bits) == combined_bits){
//			HAL_UART_Transmit(&huart2, (const uint8_t *)"All Tasks are running\r\n", 23, HAL_MAX_DELAY);
//		}else{
//			if((uxBits & task01_bit) != task01_bit){
//				HAL_UART_Transmit(&huart2, (const uint8_t *)"Task 1 is not running\r\n", 23, HAL_MAX_DELAY);
//			}
//			if((uxBits & ISR_bit) != ISR_bit){
//				HAL_UART_Transmit(&huart2, (const uint8_t *)"ISR is not trigerred\r\n", 23, HAL_MAX_DELAY);
//			}
//		}
//	}
//}

void WatchDogTask_Entry(void *arg){
	while(1){
		// wait for all the bits to be set and check until the timeout/wait time of 2000ms
		EventBits_t uxBits = xEventGroupWaitBits(EventGroupTasks_handle, combined_bits, pdTRUE, pdFALSE, pdMS_TO_TICKS(2000));

		if(uxBits){
			HAL_UART_Transmit(&huart2, (const uint8_t *)"Either of the Tasks are running\r\n", 33, HAL_MAX_DELAY);

			if((uxBits & task01_bit) == task01_bit){
				HAL_UART_Transmit(&huart2, (const uint8_t *)"Task 1 is running\r\n", 19, HAL_MAX_DELAY);
			}
			if((uxBits & ISR_bit) == ISR_bit){
				HAL_UART_Transmit(&huart2, (const uint8_t *)"ISR is trigerred\r\n", 18, HAL_MAX_DELAY);
			}
		}
	}
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

    /****** Create EventGroup ************/

    EventGroupTasks_handle = xEventGroupCreate();
    if(EventGroupTasks_handle == NULL){
  	  HAL_UART_Transmit(&huart2, (const uint8_t *)"EventGroup is not created\r\n", 27, HAL_MAX_DELAY);
    }else{
  	  HAL_UART_Transmit(&huart2, (const uint8_t *)"EventGroup is created\r\n", 23, HAL_MAX_DELAY);
    }

    /************ Create tasks **************/
    BaseType_t task01_status;
    task01_status = xTaskCreate(Task01_Entry, "Task01", 128, NULL, 1, &Task01_handle);
//    task02_status = xTaskCreate(Task02_Entry, "Task02", 128, NULL, 1, &Task02_handle);
//    task03_status = xTaskCreate(Task03_Entry, "Task03", 128, NULL, 1, &Task03_handle);

    /******** Create way=tchDog Task to monitor the eventGroupSet by tasks, with Higher Prio *********/
	BaseType_t watchdogTask_status;
	watchdogTask_status = xTaskCreate(WatchDogTask_Entry, "WdgTask", 128, NULL, 2, &WatchDogTask_handle);


    if(task01_status == pdTRUE && watchdogTask_status){
    	HAL_UART_Transmit(&huart2, (const uint8_t *)"Tasks are created\r\n", 19, HAL_MAX_DELAY);
    }else{
    	HAL_UART_Transmit(&huart2, (const uint8_t *)"Tasks are not created\r\n", 23, HAL_MAX_DELAY);
    }

    vTaskStartScheduler();
    /* We should never get here as control is now taken by the scheduler */

    /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_13){
		// toggle LED
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

		// setEventBits from ISR
		BaseType_t xHigherPriotaskWoken = pdFALSE;
		xEventGroupSetBitsFromISR(EventGroupTasks_handle, ISR_bit, &xHigherPriotaskWoken);
		// context switch if required
		portYIELD_FROM_ISR(xHigherPriotaskWoken);
	}
}
/* USER CODE END 4 */


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
