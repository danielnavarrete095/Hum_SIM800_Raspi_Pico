#pragma once

#include "hardware/uart.h"
#include "pico/stdlib.h"

class Serial
{
    public:
        Serial(uart_inst_t* _instance, uint8_t txPin, uint8_t rxPin);
        ~Serial();
        void begin(uint32_t baudRate);
        void print(const char *message);
        void println(const char *message);
        bool available();
        char read();
        uart_inst_t* instance;
};
