/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

/// \tag::hello_uart[]

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

int main() {
    // Set up our UART with the required speed.
    uart_init(uart0, BAUD_RATE);
    uart_init(uart1, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);

    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    while(1) {
        // Send out a character without any conversions
        uart_putc_raw(uart0, '-');
        uart_putc_raw(uart1, '-');

        // Send out a character but do CR/LF conversions
        uart_putc(uart0, '0');
        uart_putc(uart1, '1');

        // Send out a string, with CR/LF conversions
        uart_puts(uart0, " UART0!\n");
        uart_puts(uart1, " UART1!\n");
        sleep_ms(5000);
        while(uart_is_readable(uart0)) {
            char c = uart_getc(uart0);
            uart_putc(uart0, c);
        }
        while(uart_is_readable(uart1)) {
            char c = uart_getc(uart1);
            uart_putc(uart1, c);
        }
    }
}

/// \end::hello_uart[]
