/**
 * @file cli.c
 * @author Isak Åslund (aslundisak@gmail.com)
 * @brief CLI for terminal style connections to Donatello
 * @version 0.1
 * @date 2023-11-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#include "stm32f4xx_it.h"

#include "User/button.h"
#include "User/cli.h"
#include "User/coms.h"
#include "main.h"

#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

// ============ Private function declaration =================
static void s_cli_clear(EmbeddedCli* cli, char* args, void* context);
static void s_cli_write_char(EmbeddedCli* cli, char c);

// Bindings
static void s_led_get(EmbeddedCli* cli, char* args, void* context);
static void s_led_set(EmbeddedCli* cli, char* args, void* context);
static void s_led_toggle(EmbeddedCli* cli, char* args, void* context);
static void s_button_get_state(EmbeddedCli* cli, char* args, void* context);

// ============= Private variables ===================
static EmbeddedCli* cli;
static CLI_UINT     cliBuffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];
static bool         cli_is_ready = false; // Disable usage if cli isn't initialised

//============ Private function implementation ===============
void s_cli_clear(EmbeddedCli* cli, char* args, void* context) {

    cli_printf("\033[2J\033[0;0H"); // Clear screen => Set Cursor to start, will automatically add invitation character
}

static void s_cli_write_char(EmbeddedCli* cli, char c) {
    coms_add_tx(c);
}

static void s_led_get(EmbeddedCli* cli, char* args, void* context) {
    cli_printf("LED state: %s", HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == GPIO_PIN_SET ? "ON" : "OFF");
}

static void s_led_set(EmbeddedCli* cli, char* args, void* context) {
    const char* arg1 = embeddedCliGetToken(args, 1);

    if (!strcmp(arg1, "1") || !strcmp(arg1, "true")) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    } else if (!strcmp(arg1, "0") || !strcmp(arg1, "false")) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    } else {
        cli_printf("Set LED with args: [1/0] or [true/false]");
    }
}

static void s_led_toggle(EmbeddedCli* cli, char* args, void* context) {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

static void s_button_get_state(EmbeddedCli* cli, char* args, void* context) {
    cli_printf("Button state: %u", button_get_state());
}

// ==================== Global function implementation ==========================
/**
 * @brief Initialize CLI
 * Will BLOCK in case not enough memory(RAM) has been given to the CLI process
 *
 */
void cli_init(void) {
    EmbeddedCliConfig* config = embeddedCliDefaultConfig();
    config->cliBuffer = cliBuffer;
    config->cliBufferSize = CLI_BUFFER_SIZE;
    config->rxBufferSize = CLI_RX_BUFFER_SIZE;
    config->cmdBufferSize = CLI_CMD_BUFFER_SIZE;
    config->historyBufferSize = CLI_HISTORY_SIZE;
    config->maxBindingCount = CLI_MAX_BINDING_COUNT;
    config->enableAutoComplete = true;

    // Create new CLI instance
    cli = embeddedCliNew(config);
    if (cli == NULL) {
        // CLI init failed. Is there not enough memory allocated to the CLI?
        // Please increase the 'CLI_BUFFER_SIZE' in header file or decrease max binding / history size.
        // You can get required buffer size by calling
        // uint16_t requiredSize = embeddedCliRequiredSize(config);
        // Then check it's value in debugger

        char     error_buffer[100] = {0};
        uint16_t size = embeddedCliRequiredSize(config);
        uint16_t len = sprintf(error_buffer, "CLI could not be created, required size: %ud", size);

        for (int i = 0; i < len; i++) {
            coms_add_tx((uint8_t)error_buffer[i]); // TODO: Use coms_trannsmit() once implemented
        }
    }

    // Assign character write function
    cli->writeChar = s_cli_write_char;

    // Un-comment to add a non-default reaction to unbound commands.
    //cli->onCommand = onCliCommand;

    // Add all the initial command bindings
    CliCommandBinding clear_binding = {
        .name = "clc", .help = "Clears the console", .tokenizeArgs = false, .context = NULL, .binding = s_cli_clear
    };
    CliCommandBinding led_get_binding = {
        .name = "led-get", .help = "Get led status", .tokenizeArgs = false, .context = NULL, .binding = s_led_get
    };
    CliCommandBinding led_set_binding = {
        .name = "led-set", .help = "Set led state", .tokenizeArgs = true, .context = NULL, .binding = s_led_set
    };
    CliCommandBinding led_toggle_binding = {
        .name = "led-toggle",
        .help = "Toggle led state",
        .tokenizeArgs = false,
        .context = NULL,
        .binding = s_led_toggle
    };
    CliCommandBinding button_get_binding = {
        .name = "button-get",
        .help = "Get button state",
        .tokenizeArgs = false,
        .context = NULL,
        .binding = s_button_get_state
    };
    embeddedCliAddBinding(cli, clear_binding);
    embeddedCliAddBinding(cli, led_get_binding);
    embeddedCliAddBinding(cli, led_set_binding);
    embeddedCliAddBinding(cli, led_toggle_binding);
    embeddedCliAddBinding(cli, button_get_binding);

    // Init the CLI with blank screen
    cli_clear();

    // CLI has now been initialized, set bool to true to enable usage
    cli_is_ready = true;
}

/**
 * @brief Send characters to the CLI to be processed
 *
 * @param c Character to be processed
 */
void cli_receive_byte(uint8_t c) {
    if (!cli_is_ready) {
        return;
    }

    embeddedCliReceiveChar(cli, c);
}

/**
 * @brief Printf in the CLI
 * Function to encapsulate the 'embeddedCliPrint()' call with print formatting arguments (act like printf(), but keeps cursor at correct location).
 * The 'embeddedCliPrint()' function does already add a linebreak ('\r\n') to the end of the print statement, so no need to add it yourself.
 * @param format
 * @param ...
 */
void cli_printf(const char* format, ...) {
    char buffer[CLI_PRINTF_BUFFER_SIZE];

    // Format the string using snprintf
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Check if string fitted in buffer else print error to stderr
    if (length < 0) {
        cli_printf("printf needs more buffer size!");
    }

    // Call embeddedCliPrint with the formatted string
    embeddedCliPrint(cli, buffer);
}

/**
 * @brief Get CLI instance
 * Only one instance of EmbeddedCLI is allowed and can be retrieved with this function
 *
 * @return EmbeddedCli*
 */
EmbeddedCli* cli_get_pointer() {
    return cli;
}

/**
 * @brief Get current CLI process
 *
 */
void cli_process(void) {
    embeddedCliProcess(cli);
}

/**
 * @brief Clear terminal and reset cursor
 *
 */
void cli_clear(void) {
    s_cli_clear(cli, NULL, NULL);
}

/**
 * @brief CLI RTOS task
 * @param argument Arugmentns unused
 */
void cli_task(void const* argument) {
    cli_init();

    for (;;) {
        cli_process();
        osDelay(50);
    }
}
