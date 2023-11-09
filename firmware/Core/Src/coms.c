/**
 * @file coms.c
 * @author Isak Åslund (aslundisak@gmail.com)
 * @brief Communication Interface, layer between application and Virtual Com Port (CDC USB)
 * @version 0.1
 * @date 2023-11-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "usbd_cdc_if.h"

#include "cli.h"
#include "coms.h"

#define TX_SIZE 1048
#define RX_SIZE 50

static uint8_t s_rx_buffer[RX_SIZE];
static uint8_t s_i_rx;
static uint8_t s_tx_buffer[TX_SIZE];
static uint8_t s_i_tx;

static void s_handle_rx(void) {
    if (!s_i_rx) {
        return;
    }

    for (int i = 0; i < s_i_rx; i++) {
        cli_receive_byte(s_rx_buffer[i]);
    }
    memset(s_rx_buffer, 0, sizeof(s_rx_buffer));
    s_i_rx = 0;
}

static void s_handle_tx(void) {
    if (!s_i_tx) {
        return;
    }

    uint8_t res = CDC_Transmit_FS(s_tx_buffer, s_i_tx);
    if (res == USBD_OK) {
        memset(s_tx_buffer, 0, sizeof(s_tx_buffer));
        s_i_tx = 0;
    }
}

/**
 * @brief Communication RTOS task
 * 
 * @param argument Unused
 */
void coms_task(void const* argument) {
    // Task should handle the following:
    // 1. Handle received characters that have been placed in Que by CDC_Receive interrupt
    // 2. Handle outgoing characters that have been placed in a Que
    for (;;) {
        // Handle all received characters
        if (s_i_rx) {
            s_handle_rx();
        }

        // Handle outgoing characters
        if (s_i_tx) {
            s_handle_tx();
        }

        osDelay(50);
    }
}

/**
 * @brief Add received character via Virtual COM port
 * Only CDC receive should be calling this function
 * @param c 
 */
void coms_add_rx(uint8_t c) {
    s_rx_buffer[s_i_rx++] = c;
}

/**
 * @brief Add character to send via Virtual COM port
 * 
 * @param c Character to add
 */
void coms_add_tx(uint8_t c) {
    s_tx_buffer[s_i_tx++] = c;
}
