#include "SIM800.h"
#include <stdio.h>
// #include "hardware/gpio.h"
#define LOW 0
#define HIGH 1

#include "pico/time.h"
static inline uint32_t board_millis(void)
{
  return to_ms_since_boot(get_absolute_time());
}

SIM800::SIM800(uart_inst_t * _instance, const uint8_t tx, const uint8_t rx, const uint8_t rst, const uint8_t dtr) : simSerial((uart_inst_t*)_instance, (uint8_t)tx, (uint8_t)rx) {
    simSerial.begin((uint32_t)9600);
    dtrPin = dtr;
    rstPin = rst;
    gpio_init(dtr);
    gpio_set_dir(dtr, GPIO_OUT);
    gpio_init(rst);
    gpio_set_dir(rst, GPIO_OUT);
    gpio_put(dtr, LOW);
    gpio_put(rst, HIGH);
}

SIM800::~SIM800() {

}

// bool SIM800::SMS_Send(const char *destination, const char *message) {

//   if (SendCommand(atPrefix + "CMGF=1", 3000, responses[OK_RESP]) == NO_ERR) {
//     String sendcmd = atPrefix + "CMGS=\"";
//     sendcmd += destination;
//     sendcmd += "\"";
//     if (SendCommand(sendcmd, 3000, responses[INPUT_RESP]) == NO_ERR) {
//       simSerial.println(message);
//       // Serial.println(message);
//       sleep_ms(5);
//       simSerial.print(char(26));
//       SendCommand("", 3000, responses[OK_RESP]);
//       sleep_ms(5);
//       return true;
//     }
//   }
//   return false;
// }

bool SIM800::UDP_Send(const char *message) {
  char command[20];
  memset(command, '\0', 20);
  strcpy(command, atPrefix);
  strcat(command, "CIPSEND=");

  // Add message length in string format
  char message_size_str[5];
  memset(message_size_str, '\0', 5);
  uint8_t message_size = strlen(message);
  sprintf(message_size_str, "%u", message_size);
  strcat(command, message_size_str);

  error_t err = SendCommand(command, 3000, responses[INPUT_RESP]);
  if (err == NO_ERR) {
    err = SendCommand(message, 3000, responses[SEND_OK_RESP]);
    if (err == NO_ERR) {
      return true;
    } else {
      return false;
    }
  } else {
      return false;
  }
}

bool SIM800::UDP_Receive(char *res, const unsigned long timeout, const char *expectedResp) {
  unsigned long time = board_millis();
  bool flag = true;
  debug("expectedResponse", expectedResp);
  memset(res, '\0', 100);
  while ( (time + timeout) > board_millis()) {
    // TODO: make sure not to overflow buffer
    // if (strlen(res) < 80) {
      getResponse(res);
    // }
    // sleep_ms(10);
    // If first thing you get is a \n disregard it
    if(flag == true) {
      if (*res == '\n' || *res == '\r') {
        memset(res, '\0', 100);
        // debug("Cleaning res");
      } else {
        flag = false;
      }
      continue;
    }
    if(strchr(res, '\n') != NULL) break;
  }
  if (strstr(res, expectedResp) != NULL) {
    debug("Response found!");
    return true;
  }
  // Workaround to work with not \n terminated responses
  // if (expectedResp == " ") {
  //   return true;
  // }
  debug("Response not found!");
  return false;
}

bool SIM800::UDP_Connect(const char *address, const char *port) {
  error_t err = NO_RESPONSE_ERR;
  // Set to UDP extended mode to SEND/RECEIVE
  // const String msg = "Setting UDP Extended mode";
  // debug(msg);
  
  char command[50];
  memset(command, '\0', 50);
  strcpy(command, atPrefix);
  strcat(command, "CIPUDPMODE=1");
  err = SendCommand(command, 3000, responses[OK_RESP]);
  // Serial.print("err returned after AT+CIPUDPMODE=1: ");
  // Serial.println(String(err));
  if (err == NO_ERR) {
    // Connect using address and port
    // const String msg = "Connecting to server";
    // debug(msg);
    memset(command, '\0', 50);
    strcpy(command, atPrefix);
    strcat(command, "CIPSTART=\"UDP\",\"");
    strcat(command, address);
    strcat(command, "\",\"");
    strcat(command, port);
    strcat(command, "\"");
    err = SendCommand(command, 3000, responses[CONNECT_OK_RESP]);
    // If it was already connected, close connection and try again
    if (err == ALREADY_CONN_ERR) {
      if (UDP_Disconnect()) {
        // We, were already connected, try again
        UDP_Connect(address, port);
      }
    }
    else if (err == NO_ERR) {
      return true;
    }
    else {
      return false;
    }
  } else if (err == NO_RESPONSE_ERR) { // If SIM didn't responded, not much we can do
    const char msg[] = "SIM unresponsive!";
    debug(msg);
    return false;
    
  } else if (err == UNDEF_ERR) {// If SIM responded with "ERROR" we're probably already connected, try disconnecting and then change UDP mode
    if (UDP_Disconnect()) {
    // We, were already connected, try again
      return UDP_Connect(address, port);
    } else { // If there's any error after this, something's really wrong, should reset sim
      return false;
    }
  }
}

bool SIM800::UDP_Disconnect() {
  char command[15];
  memset(command, '\0', 15);
  strcpy(command, atPrefix);
  strcat(command, "CIPCLOSE");

  if (SendCommand(command, 3000, responses[CLOSE_OK_RESP]) == NO_ERR) {
    return true;
  }
  else {
    return false;
  }
}

bool SIM800::getIMEI(char *imei, const unsigned long timeout) {
  error_t err;
  debug("Getting IMEI");
  // Serial.print("Calling SendCommand with: ");
  // Serial.println("AT+CGSN");
  char response[35]; // Max size 25 to be safe
  memset(response, '\0', 35);
  char command[10];
  memset(command, '\0', 10);
  strcpy(command, atPrefix);  
  strcat(command, "CGSN");

  err = SendCommand(command, 3000, responses[OK_RESP], response);

  if (err == NO_ERR) {
    // Remove any letters or characters that are not numbers
    debug("Got imei succesfully");
    string_remove(response, command);
    string_remove(response, "OK");
    string_remove(response, "\n");
    string_remove(response, "\r");
    string_remove(response, " ");
    strcpy(imei, response);
    return false;
  }
  debug("Error getting imei");
  return true;
}

error_t SIM800::SendCommand(const char *command, const unsigned long timeout, const char *expectedResponse) {
  char res[100];
  memset(res, '\0', 100);
  // strcpy(res, "empty");
  error_t err = SendCommand(command, timeout, expectedResponse, res);
  return err;
}
error_t SIM800::SendCommand(const char *command, const unsigned long timeout, const char *expectedResponse, char *res) {
  debug("Sending to SIM", command);
  simSerial.println(command);
  sleep_ms(10);
  return verifyResponse(expectedResponse, timeout, res);
}

void SIM800::getResponse(char *res) {
    if (simSerial.available()) {
      char c = simSerial.read();
      strncat(res, &c, 1);
      // debug("r", res);
    }
}

error_t SIM800::verifyResponse(const char *expectedResp, const unsigned long timeout, char *res) {
  uint8_t count = 1;
  error_t err = NO_RESPONSE_ERR;
  unsigned long time = board_millis();
  debug("expectedResponse", expectedResp);
  while ( (time + timeout) > board_millis()) {
    // TODO: Make sure you don't overflow response buffer (receive size as parameter)
    // if (strlen(res) < 20) {
    getResponse(res);
    // }
    // Check if we get the expected response
    if (strstr(res, expectedResp) != NULL) {
      debug("Response found!");
      // debug("res", res);
      return NO_ERR;
    }
    else if (strstr(res, responses[ALREADY_CONN_RESP]) != NULL) {
    debug("Already connected ERROR");
    return ALREADY_CONN_ERR;
    } else if (strstr(res, responses[ERROR_RESP]) != NULL) {
      debug("SIM responded with undefined ERROR");
      return UNDEF_ERR;
    }
  }
  // At this point we didn't get any response, return
  debug("Response not found");
  return err;
}

// Entering Sleep mode 1: 
// DTR pin is HIGH
// AT+CSCLK=1

// OK

// Once this command is entered the module directly enters sleep mode and the current consumption gets reduced to ~1mA
bool SIM800::sleep() {
  gpio_put(dtrPin, HIGH);
  error_t err = NO_RESPONSE_ERR;
  char command[15];
  memset(command, '\0', 15);
  strcpy(command, atPrefix);
  strcat(command, "CSCLK=1");
  err = SendCommand(command, 3000, responses[OK_RESP]);
  // debug("Error after AT+CSCLK=1", String(err));
  // gpio_put(dtrPin, LOW);
}
// Exiting Sleep mode 1: 

// DTR pin to LOW
// send dummy AT command 
// send AT+CSCLK=0

// Remember that this all should happen within 50ms of DTR made LOW
bool SIM800::wakeUp() {
  // gpio_put(dtrPin, HIGH);
  // sleep_ms(10);
  gpio_put(dtrPin, LOW);
  // sleep_ms(10);
  error_t err = NO_RESPONSE_ERR;
  char command[15];
  memset(command, '\0', 15);
  strcpy(command, "AT");
  err = SendCommand(command, 3000, responses[OK_RESP]);
  // debug("Error after AT", String(err));
  strcpy(command, atPrefix);
  strcat(command, "CSCLK=0");
  err = SendCommand(command, 3000, responses[OK_RESP]);
  // debug("Error after AT+CSCLK=0", String(err));
}

void SIM800::reset() {
  gpio_put(rstPin, LOW);
  sleep_ms(1000);
  gpio_put(rstPin, HIGH);
}