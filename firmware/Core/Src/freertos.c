/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "usbd_cdc_if.h"
#include "cli.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId cliTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartCLITask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityHigh, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of cliTask */
  osThreadDef(cliTask, StartCLITask, osPriorityLow, 0, 2056);
  cliTaskHandle = osThreadCreate(osThread(cliTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
	/* Infinite loop */
	for(;;)
	{
//		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//		k += 0.1;
//		x = 10*cos(k);
//		y = 10*sin(k);
		osDelay(50);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartCLITask */
/**
* @brief Function implementing the cliTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCLITask */
void StartCLITask(void const * argument)
{
  /* USER CODE BEGIN StartCLITask */

	/* USER CODE BEGIN StartLoggigTask */
	cli_init();
	/* Infinite loop */
	for(;;)
	{
		// Task should handle the following:
		// 1. Handle received characters that have been placed in Que by CDC_Receive interrupt
		// 2. Handle outgoing characters that have been placed in a Que
		// 3. CLI Process

		// Handle all received characters
		for(int i = 0; i < i_rx; i++){
			cli_receive_byte(rx_buffer[i]);
		}
		memset(rx_buffer, 0, sizeof(rx_buffer));
		i_rx = 0;


		// Handle outgoing characters
		uint8_t res = CDC_Transmit_FS(tx_buffer, i_tx);
		if (res == USBD_OK){
			memset(tx_buffer, 0, sizeof(tx_buffer));
			i_tx = 0;
		}


		cli_process();

//		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		osDelay(50);

	}
  /* USER CODE END StartCLITask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */
