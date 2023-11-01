/*
 * coms.c
 *
 *	Interface to for sending receiving characters
 *
 *  Created on: Nov 1, 2023
 *      Author: Isak
 */

#include <stdint.h>
#include <string.h>

#include "usbd_cdc_if.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

#include "coms.h"
#include "cli.h"


#define TX_SIZE 1048
#define RX_SIZE 50


static uint8_t s_rx_buffer[RX_SIZE];
static uint8_t s_i_rx;
static uint8_t s_tx_buffer[TX_SIZE];
static uint8_t s_i_tx;



void coms_task(void const * argument){
	// Task should handle the following:
	// 1. Handle received characters that have been placed in Que by CDC_Receive interrupt
	// 2. Handle outgoing characters that have been placed in a Que
	for(;;){
		// Handle all received characters
		for(int i = 0; i < s_i_rx; i++){
			cli_receive_byte(s_rx_buffer[i]);
		}
		memset(s_rx_buffer, 0, sizeof(s_rx_buffer));
		s_i_rx = 0;


		// Handle outgoing characters
		uint8_t res = CDC_Transmit_FS(s_tx_buffer, s_i_tx);
		if (res == USBD_OK){
			memset(s_tx_buffer, 0, sizeof(s_tx_buffer));
			s_i_tx = 0;
		}

		osDelay(50);
	}
}


/**
 * Add received character via Virtual COM port
 * Only CDC receive should be calling this function
 */
void coms_add_rx(uint8_t c){
	s_rx_buffer[s_i_rx++] = c;
}

/**
 * Add character to send via Virtual COM port
 */
void coms_add_tx(uint8_t c){
	s_tx_buffer[s_i_tx++] = c;
}
