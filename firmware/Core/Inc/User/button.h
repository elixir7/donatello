/**
 * @file button.h
 * @author Isak Åslund (aslundisak@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdint.h>

typedef enum { eBUTTON_STATE_NOT_PRESSED = 0, eBUTTON_STATE_PRESSED } button_state_e;

void           button_init(void);
void           button_task(void);
button_state_e button_get_state(void);