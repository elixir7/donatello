/*
 * cli.c
 *
 *  Created on: Oct 30, 2023
 *      Author: Isak
 */

#include "stm32f4xx_it.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include "usbd_cdc_if.h"


#include "cli.h"
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"


// ============ Private function declaration =================
void cli_clear(EmbeddedCli *cli, char *args, void *context);
void onLed(EmbeddedCli *cli, char *args, void *context);
static void cli_write_char(EmbeddedCli *embeddedCli, char c);


// ============= Private variables ===================
// CLI buffer
static EmbeddedCli *cli;
static CLI_UINT cliBuffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];

// Bool to disable the interrupts, if CLI is not yet ready.
static bool cli_is_ready = false;


//============ Private function implementation ===============
// Bindings
void cli_clear(EmbeddedCli *cli, char *args, void *context){
	// Unused arguments
	cli_printf("\033[2J\033[0;0H");
}


void onLed(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);
    if (arg1 == NULL || arg2 == NULL) {
        cli_printf("usage: get-led [arg1] [arg2]");
        return;
    }
    // Make sure to check if 'args' != NULL, printf's '%s' formatting does not like a null pointer.
    cli_printf("LED with args: %s and %s", arg1, arg2);
}

static void cli_write_char(EmbeddedCli *embeddedCli, char c) {
	uint8_t char_to_send = c;

	while(CDC_Transmit_FS(&char_to_send, 1) == USBD_BUSY){}; //This fails often
}


// ==================== Global function implementation ==========================


void cli_init(void){
	EmbeddedCliConfig *config = embeddedCliDefaultConfig();
	config->cliBuffer 			= cliBuffer;
	config->cliBufferSize 		= CLI_BUFFER_SIZE;
	config->rxBufferSize 		= CLI_RX_BUFFER_SIZE;
	config->cmdBufferSize 		= CLI_CMD_BUFFER_SIZE;
	config->historyBufferSize 	= CLI_HISTORY_SIZE;
	config->maxBindingCount 	= CLI_MAX_BINDING_COUNT;
	config->enableAutoComplete =  false;


	// Create new CLI instance
	cli = embeddedCliNew(config);

	// Assign character write function
	cli->writeChar = cli_write_char;

	// CLI init failed. Is there not enough memory allocated to the CLI?
	// Please increase the 'CLI_BUFFER_SIZE' in header file.
	// Or decrease max binding / history size.
	// You can get required buffer size by calling
	// uint16_t requiredSize = embeddedCliRequiredSize(config);
	// Then check it's value in debugger
	while (cli == NULL) {
		HardFault_Handler();
	}


	// Un-comment to add a non-default reaction to unbound commands.
	//cli->onCommand = onCliCommand;

	// Add all the initial command bindings
	CliCommandBinding clear_binding = {
			.name = "clear",
			.help = "Clears the console",
			.tokenizeArgs = true,
			.context = NULL,
			.binding = cli_clear
	};
	CliCommandBinding led_binding = {
			.name = "get-led",
			.help = "Get led status",
			.tokenizeArgs = true,
			.context = NULL,
			.binding = onLed
	};
	embeddedCliAddBinding(cli, clear_binding);
	embeddedCliAddBinding(cli, led_binding);

	// Init the CLI with blank screen
	cli_clear(cli, NULL, NULL);

	// CLI has now been initialized, set bool to true to enable interrupts.
	cli_is_ready = true;
}

void cli_receive_byte(uint8_t c){
	if (!cli_is_ready){
		return;
	}

	embeddedCliReceiveChar(cli, c);
}


// Function to encapsulate the 'embeddedCliPrint()' call with print formatting arguments (act like printf(), but keeps cursor at correct location).
// The 'embeddedCliPrint()' function does already add a linebreak ('\r\n') to the end of the print statement, so no need to add it yourself.
void cli_printf(const char *format, ...) {
    // Create a buffer to store the formatted string
    char buffer[CLI_PRINT_BUFFER_SIZE];

    // Format the string using snprintf
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Check if string fitted in buffer else print error to stderr
    if (length < 0) {
        fprintf(stderr, "Error formatting the string\r\n");
        return;
    }

    // Call embeddedCliPrint with the formatted string
    embeddedCliPrint(cli, buffer);
}

/**
 * Getter function, to keep only one instance of the EmbeddedCli pointer in this file.
 * @return
 */
EmbeddedCli * cli_get_pointer() {
    return cli;
}

void cli_process(void){
	embeddedCliProcess(cli);
}


