/*
 * cli.h
 *
 *  Created on: Oct 30, 2023
 *      Author: Isak
 */

#ifndef INC_CLI_H_
#define INC_CLI_H_

#include "embedded_cli.h"

// Definitions for CLI sizes
#define CLI_BUFFER_SIZE 2048
#define CLI_RX_BUFFER_SIZE 16
#define CLI_CMD_BUFFER_SIZE 32
#define CLI_HISTORY_SIZE 32
#define CLI_MAX_BINDING_COUNT 32


/**
 * Definition of the cli_printf() buffer size.
 * Can make smaller to decrease RAM usage,
 * make larger to be able to print longer strings.
 */
#define CLI_PRINTF_BUFFER_SIZE 1048


void cli_init(void);
void cli_process(void);
void cli_clear(void);
void cli_printf(const char *format, ...);
void cli_receive_byte(uint8_t c);
EmbeddedCli * cli_get_pointer();



#endif /* INC_CLI_H_ */
