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
#include "FreeRTOSConfig.h"


#include "cli.h"
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"


// ============ Private function declaration =================
static void s_cli_clear(EmbeddedCli *cli, char *args, void *context);
static void cli_write_char(EmbeddedCli *embeddedCli, char c);

static void get_led(EmbeddedCli *cli, char *args, void *context);
static void set_led(EmbeddedCli *cli, char *args, void *context);
static void toggle_led(EmbeddedCli *cli, char *args, void *context);


// ============= Private variables ===================
static EmbeddedCli *cli;
static CLI_UINT cliBuffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];

static bool cli_is_ready = false; // Disable usage if cli isn't initialised


//============ Private function implementation ===============
void s_cli_clear(EmbeddedCli *cli, char *args, void *context){
	cli_printf("\033[2J\033[0;0H"); // Clear screen => Set Cursor to start, will automatically add invitation character
}

static void cli_write_char(EmbeddedCli *embeddedCli, char c) {
	rtos_add_tx(c);
}


static void get_led(EmbeddedCli *cli, char *args, void *context) {
    const char *arg1 = embeddedCliGetToken(args, 1);
    const char *arg2 = embeddedCliGetToken(args, 2);
    if (arg1 == NULL || arg2 == NULL) {
        cli_printf("usage: get-led [arg1] [arg2]");
        return;
    }
    // Make sure to check if 'args' != NULL, printf's '%s' formatting does not like a null pointer.
    cli_printf("LED with args: %s and %s", arg1, arg2);
}

static void set_led(EmbeddedCli *cli, char *args, void *context) {
	const char *arg1 = embeddedCliGetToken(args, 1);
	if ( !strcmp(arg1,"1") || !strcmp(arg1,"true")){
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	} else if (!strcmp(arg1,"0") || !strcmp(arg1,"false")){
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	} else{
		cli_printf("Set LED with args: [1/0] or [true/false]");
	}
}

static void toggle_led(EmbeddedCli *cli, char *args, void *context) {
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
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
	config->enableAutoComplete =  true;


	// Create new CLI instance
	cli = embeddedCliNew(config);
	// CLI init failed. Is there not enough memory allocated to the CLI?
	// Please increase the 'CLI_BUFFER_SIZE' in header file.
	// Or decrease max binding / history size.
	// You can get required buffer size by calling
	// uint16_t requiredSize = embeddedCliRequiredSize(config);
	// Then check it's value in debugger
	if (cli == NULL){
		char error_buffer[100] = {0};
		uint16_t size = embeddedCliRequiredSize(config);
		uint16_t len = sprintf(error_buffer, "CLI could not be created, required size: %ud", size);
		uint16_t res = CDC_Transmit_FS((uint8_t *) error_buffer, len);
		if (res != USBD_OK){
			HardFault_Handler();
		}
	}

	// Assign character write function
	cli->writeChar = cli_write_char;


	// Un-comment to add a non-default reaction to unbound commands.
	//cli->onCommand = onCliCommand;

	// Add all the initial command bindings
	CliCommandBinding clear_binding = {
			.name = "clear",
			.help = "Clears the console",
			.tokenizeArgs = true,
			.context = NULL,
			.binding = s_cli_clear
	};
	CliCommandBinding led_get_binding = {
			.name = "get-led",
			.help = "Get led status",
			.tokenizeArgs = true,
			.context = NULL,
			.binding = get_led
	};
	CliCommandBinding led_set_binding = {
			.name = "set-led",
			.help = "Set led state",
			.tokenizeArgs = true,
			.context = NULL,
			.binding = set_led
	};
	CliCommandBinding led_toggle_binding = {
			.name = "toggle-led",
			.help = "Toggle led state",
			.tokenizeArgs = true,
			.context = NULL,
			.binding = toggle_led
	};
	embeddedCliAddBinding(cli, clear_binding);
	embeddedCliAddBinding(cli, led_get_binding);
	embeddedCliAddBinding(cli, led_set_binding);
	embeddedCliAddBinding(cli, led_toggle_binding);


	// Init the CLI with blank screen
	cli_clear();

	// CLI has now been initialized, set bool to true to enable usage
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
    char buffer[CLI_PRINTF_BUFFER_SIZE];

    // Format the string using snprintf
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Check if string fitted in buffer else print error to stderr
    if (length < 0) {
    	Error_Handler(); // print out error?
    }

    // Call embeddedCliPrint with the formatted string
    embeddedCliPrint(cli, buffer);
}

/**
 * Getter function, to keep only one instance of the EmbeddedCli pointer in this file.
 * @return EmbeddedCli *
 */
EmbeddedCli * cli_get_pointer() {
    return cli;
}


/**
 * Wrapper for running handle function
 */
void cli_process(void){
	embeddedCliProcess(cli);
}

/**
 * Wrapper for clearing termi~nal
 */
void cli_clear(void){
	s_cli_clear(cli, NULL, NULL);
}


