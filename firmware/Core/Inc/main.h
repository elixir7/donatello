/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_0
#define LED_GPIO_Port GPIOA
#define STEER_PWM_Pin       GPIO_PIN_1
#define STEER_PWM_GPIO_Port GPIOA
#define MOTOR_PWM_Pin       GPIO_PIN_2
#define MOTOR_PWM_GPIO_Port GPIOA
#define BUTTON_Pin GPIO_PIN_3
#define BUTTON_GPIO_Port GPIOA
#define BUTTON_EXTI_IRQn EXTI3_IRQn
#define BAT_ADC_Pin         GPIO_PIN_4
#define BAT_ADC_GPIO_Port   GPIOA
#define STEER_ADC_Pin       GPIO_PIN_5
#define STEER_ADC_GPIO_Port GPIOA
#define XSHUT1_Pin          GPIO_PIN_6
#define XSHUT1_GPIO_Port    GPIOA
#define XSHUT2_Pin          GPIO_PIN_7
#define XSHUT2_GPIO_Port    GPIOA
#define XSHUT3_Pin          GPIO_PIN_0
#define XSHUT3_GPIO_Port    GPIOB
#define XSHUT4_Pin          GPIO_PIN_1
#define XSHUT4_GPIO_Port    GPIOB
#define XSHUT5_Pin          GPIO_PIN_2
#define XSHUT5_GPIO_Port    GPIOB
#define INT1_Pin            GPIO_PIN_10
#define INT1_GPIO_Port      GPIOB
#define INT2_Pin            GPIO_PIN_12
#define INT2_GPIO_Port      GPIOB
#define INT3_Pin            GPIO_PIN_13
#define INT3_GPIO_Port      GPIOB
#define INT4_Pin            GPIO_PIN_14
#define INT4_GPIO_Port      GPIOB
#define INT5_Pin            GPIO_PIN_15
#define INT5_GPIO_Port      GPIOB
#define KILL_Pin            GPIO_PIN_9
#define KILL_GPIO_Port      GPIOA
#define ENC_A_Pin           GPIO_PIN_4
#define ENC_A_GPIO_Port     GPIOB
#define ENC_B_Pin           GPIO_PIN_5
#define ENC_B_GPIO_Port     GPIOB
#define SCL_Pin             GPIO_PIN_6
#define SCL_GPIO_Port       GPIOB
#define SDA_Pin             GPIO_PIN_7
#define SDA_GPIO_Port       GPIOB
#define STEER_DIR_Pin       GPIO_PIN_8
#define STEER_DIR_GPIO_Port GPIOB
#define MOTOR_DIR_Pin       GPIO_PIN_9
#define MOTOR_DIR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
