/**
 * @file button.c
 * @author Isak Åslund (aslundisak@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdbool.h>
#include <stdint.h>

#include "cmsis_os.h"
#include "main.h"
#include "stm32f4xx_hal.h"

#include "User/button.h"
#include "User/cli.h"
#include "lwbtn.h"

// ============= Private variables ===================
static lwbtn_btn_t btns[1] = {0}; // Variable to store all information for lwbtn.

// ============ Private function declaration =================
static uint8_t s_button_get_state(struct lwbtn* lw, struct lwbtn_btn* btn);
static void    s_button_event(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt);

//============ Private function implementation ===============
static uint8_t s_button_get_state(struct lwbtn* lw, struct lwbtn_btn* btn) {
    return !HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);
}

static void s_button_event(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt) {
    // Here all events can be processed
    if (evt == LWBTN_EVT_ONPRESS) {
        cli_printf("[LWBTN] OnPress");
    } else if (evt == LWBTN_EVT_ONRELEASE) {
        cli_printf("[LWBTN] OnRelease");
    } else if (evt == LWBTN_EVT_ONCLICK) {
        cli_printf("[LWBTN] Click Count: %u", btn->click.cnt);
    }

    // TODO: Change modes by clicking X amount of times?
}

// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//     // TODO:  Use both rising and falling callback for buttonand manually edit state, use LWBTN_GET_STATE_MODE_MANUAL
// }

// ==================== Global function implementation ==========================
/**
 * @brief Initialize button handler.
 * Uses lwbtn library
 * 
 */
void button_init(void) {
    lwbtn_init_ex(NULL, btns, sizeof(btns) / sizeof(btns[0]), s_button_get_state, s_button_event);
}

/**
 * @brief Button RTOS task
 */
void button_task(void) {
    button_init();

    for (;;) {
        lwbtn_process(HAL_GetTick());
        osDelay(10);
    }
}

/**
 * @brief Get button state
 * 
 * @return button_state_e State of the button
 */
button_state_e button_get_state(void) {
    // Button is Pulled up in when not pressed
    return (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_RESET) ? eBUTTON_STATE_PRESSED
                                                                              : eBUTTON_STATE_NOT_PRESSED;
}