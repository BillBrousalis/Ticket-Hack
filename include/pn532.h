#ifndef _PN532_H_
#define _PN532_H_

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <assert.h>

#include "serial.h"
#include "util.h"

#define PAGELENGTH                          0x04
#define MAXPAGES                            0x29
#define FIRMWARE_VERSION_LEN                0x04
#define UID_LENGTH                          0x07

#define PN532_PREAMBLE                      0x00
#define PN532_STARTCODE1                    0x00
#define PN532_STARTCODE2                    0xFF
#define PN532_POSTAMBLE                     0x00

#define PN532_HOSTTOPN532                   0xD4
#define PN532_PN532TOHOST                   0xD5

#define PN532_COMMAND_GETFIRMWAREVERSION    0x02
#define PN532_COMMAND_GETGENERALSTATUS      0x04
#define PN532_COMMAND_READGPIO              0x0C
#define PN532_COMMAND_WRITEGPIO             0x0E
#define PN532_COMMAND_SETSERIALBAUDRATE     0x10
#define PN532_COMMAND_SETPARAMETERS         0x12
#define PN532_COMMAND_SAMCONFIGURATION      0x14
#define PN532_COMMAND_POWERDOWN             0x16
#define PN532_COMMAND_INLISTPASSIVETARGET   0x4A
#define PN532_COMMAND_INDATAEXCHANGE        0x40
#define PN532_COMMAND_INCOMMUNICATETHRU     0x42
#define PN532_COMMAND_INDESELECT            0x44
#define PN532_COMMAND_INRELEASE             0x52
#define PN532_COMMAND_INSELECT              0x54
#define PN532_COMMAND_INAUTOPOLL            0x60

#define PN532_RESPONSE_INDATAEXCHANGE       0x41
#define PN532_RESPONSE_INLISTPASSIVETARGET  0x4B

#define PN532_WAKEUP                        0x55

#define PN532_SPI_STATREAD                  0x02
#define PN532_SPI_DATAWRITE                 0x01
#define PN532_SPI_DATAREAD                  0x03
#define PN532_SPI_READY                     0x01

#define PN532_MIFARE_ISO14443A              0x00

#define MIFARE_CMD_AUTH_A                   0x60
#define MIFARE_CMD_AUTH_B                   0x61
#define MIFARE_CMD_READ                     0x30
#define MIFARE_CMD_WRITE                    0xA0
#define MIFARE_CMD_TRANSFER                 0xB0
#define MIFARE_CMD_DECREMENT                0xC0
#define MIFARE_CMD_INCREMENT                0xC1
#define MIFARE_CMD_STORE                    0xC2

/* Mifare Ultralight Commands */
#define ULTRALIGHT_CMD_REQUEST              0x26
#define ULTRALIGHT_CMD_WUP                  0x52
#define ULTRALIGHT_CMD_GETVERSION           0x60
#define ULTRALIGHT_CMD_READ                 0x30
#define ULTRALIGHT_CMD_FAST_READ            0x3a
#define ULTRALIGHT_CMD_WRITE                0xa2
#define ULTRALIGHT_CMD_COMPWRITE            0xa0
#define ULTRALIGHT_CMD_READCNT              0x39
#define ULTRALIGHT_CMD_INCRCNT              0xa5
#define ULTRALIGHT_CMD_PWDAUTH              0x1b
#define ULTRALIGHT_CMD_READSIG              0x3c

class pn532 {
public:
  Serial ser;
  pn532(std::string const dev_name, int t=1000);
  ~pn532();
  void echo_test();
  void wakeup();
  void SAM_config();
  void call_func(std::vector<uint8_t> &buf, uint8_t command, int res_length, std::vector<uint8_t> params);
  void get_firmware_version(std::vector<uint8_t> &buf);
  void read_passive_target(std::vector<uint8_t> &buf, int baud=PN532_MIFARE_ISO14443A);
  void ultralight_read_page(std::vector<uint8_t> &buf, int page);
  int ultralight_write_page(std::vector<uint8_t> dat, int page);

private:
  int DEBUG_LVL;
  int timeout;
  int write_frame(std::vector<uint8_t> dat);
  void read_frame(std::vector<uint8_t> &buf, int res_length);
  int ack_wait();
};

#endif
