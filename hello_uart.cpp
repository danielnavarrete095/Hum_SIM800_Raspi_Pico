#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "sim800/SIM800.h"

#define DEBUG

#define BAUD_RATE 115200
#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define SIM_TX_PIN 4
#define SIM_RX_PIN 5
#define SIM_RST_PIN 8
#define SIM_DTR_PIN 9

#define BUFF_SIZE 300

// char ipAddress[] = "216.75.21.40";
// char port[] = "15033";
char ipAddress[] = "189.154.214.182";
char port[] = "15031";

char buffer[BUFF_SIZE];

SIM800 sim(uart1, (uint8_t)SIM_TX_PIN, (uint8_t)SIM_RX_PIN, (uint8_t)SIM_RST_PIN, (uint8_t)SIM_DTR_PIN);
Serial DebugSerial(uart0, UART0_TX_PIN, UART0_RX_PIN);

int main() {
    DebugSerial.begin(BAUD_RATE);
    DebugSerial.println("Init");

    while(1) {
        bool simErr = false;
        // sim.wakeUp();
        char imei[16];
        memset(imei, '\0', 16);
        // debug("Getting imei!");
        simErr = sim.getIMEI(imei, 1000);
        if (simErr) {
            debug("Sim unresponsive!");
        #ifdef RESET_IF_NO_RESPONSE
            sim.reset();
        #ifdef DEBUG
            Serial.flush();
        #endif
            resetFunc();
        #endif
        } else {
          debug("imei", imei);
          // Once you have the IMEI, copy it to the buffer
          strncpy(buffer, imei, 15);
          buffer[15] = '|';
          if (sim.UDP_Connect(ipAddress, port)) {
            debug("Connected succesfully");
            if (sim.UDP_Send(buffer)) {
              debug("Sent succesfully");
            }
          }
        }
        sleep_ms(5000);
    }
}

#ifdef DEBUG
void debug(const char *message, const char *value) {
  DebugSerial.print(message);
  DebugSerial.print(": ");
  DebugSerial.println(value);
}
void debug(const char *message, const char value) {
  char charStr[2];
  strncat(charStr, &value, 1);
  DebugSerial.print(message);
  DebugSerial.print(": ");
  DebugSerial.println(charStr);
}
void debug(const char *message, const uint16_t value) {
  char valStr[10];
  sprintf(valStr, "%u", value);
  DebugSerial.print(message);
  DebugSerial.print(": ");
  DebugSerial.println(valStr);
}
void debug(const char *message, const float value) {
  char valStr[10];
  sprintf(valStr, "%f", value);
  DebugSerial.print(message);
  DebugSerial.print(": ");
  DebugSerial.println(valStr);
}
void debug(const char *message, const bool value) {
  DebugSerial.print(message);
  DebugSerial.print(": ");
  if(value) DebugSerial.println("True");
  else DebugSerial.println("False");
}
void debug(const char *message) {
  DebugSerial.println(message);
}
#else
void debug(const char *message, const char *value) {
  return;
}
void debug(const char *message, const char value) {
  return;
}
void debug(const char *message, const uint16_t value) {
  return;
}
void debug(const char *message, const float value) {
  return;
}
void debug(const char *message, const bool value) {
  return;
}
void debug(const char *message) {
  return;
}
#endif
