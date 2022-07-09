#pragma once
#include "../Serial.h"
#include <string.h>
#include <stdlib.h>

typedef enum {
  NO_ERR,
  CONN_ERR,
  ALREADY_CONN_ERR, // This is returned when SIM responded "ALREADY CONNECT"
  NO_RESPONSE_ERR,  // This is returned when SIM didn't repond
  SEND_ERR,         // This is returned when SIM can't send data through UDP
  UNDEF_ERR         // This is returned when SIM responds with "ERROR"
}error_t;

typedef enum {
    OK_RESP,
    SEND_OK_RESP,
    INPUT_RESP,
    CONNECT_OK_RESP,
    ALREADY_CONN_RESP,
    CLOSE_OK_RESP,
    ERROR_RESP,
    SEND_ERR_RESP,
    CALL_RDY_RESP,
    NUM_RESPONSES
}response_t;

const char responses[NUM_RESPONSES][16] = {"OK", "SEND OK", ">", "CONNECT OK", "ALREADY CONNECT", "CLOSE OK", "ERROR", "SEND FAIL", "Call Ready"};

class SIM800
{
    private:
        Serial simSerial;
        uint8_t dtrPin;
        uint8_t rstPin;
        const char atPrefix[4] = "AT+";

        error_t SendCommand(const char *command, const unsigned long timeout, const char *expectedResponse);
        error_t SendCommand(const char *command, const unsigned long timeout, const char *expectedResponse, char *res);
        // error_t verifyResponse(String expectedResp, const int timeout);
        error_t verifyResponse(const char *expectedResp, const unsigned long timeout, char *res);
        void getResponse(char *res);

    public:
        // const String ipAddress;
        // const String port;

        SIM800(uart_inst_t * _instance, const uint8_t tx, const uint8_t rx, const uint8_t rst, const uint8_t dtr);
        
        ~SIM800();

        bool SMS_Send(const char *destination, const char *message);
        bool UDP_Send(const char *message);
        bool UDP_Receive(char *res, const unsigned long timeout, const char *expectedResp);
        bool UDP_Connect(const char *address, const char *port);
        bool UDP_Disconnect();
        bool getIMEI(char *imei, const unsigned long timeout);
        bool sleep();
        bool wakeUp();
        void reset();

};

extern void string_remove(char * source, char * toRemove);

void debug(const char *message, const char *value);
void debug(const char *message, const uint16_t value);
void debug(const char *message, const float value);
void debug(const char *message, const bool value);
void debug(const char *message);