#include "Serial.h"

Serial::Serial(uart_inst_t* _instance, uint8_t txPin, uint8_t rxPin) {
    instance = _instance;

    gpio_set_function(txPin, GPIO_FUNC_UART);
    gpio_set_function(rxPin, GPIO_FUNC_UART);
    
}
        
Serial::~Serial() {

}

void Serial::begin(uint32_t baudRate) {
    uart_init(instance, baudRate);
}

void Serial::print(const char *message) {
    uart_puts(instance, message);
}

void Serial::println(const char *message) {
    uart_puts(instance, message);
    uart_putc(instance, '\n');
}

bool Serial::available() {
    return uart_is_readable(instance);
}

char Serial::read() {
    return uart_getc(instance);
}
