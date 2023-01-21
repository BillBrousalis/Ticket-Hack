#ifndef _PN532_H_
#define _PN532_H_

#include <iostream>
#include <vector>
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
#define PN532_COMMAND_SAMCONFIGURATION      0x14
#define PN532_COMMAND_POWERDOWN             0x16
#define PN532_COMMAND_INLISTPASSIVETARGET   0x4A
#define PN532_COMMAND_INDATAEXCHANGE        0x40

#define PN532_WAKEUP                        0x55

#define PN532_MIFARE_ISO14443A              0x00

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
  std::vector<uint8_t> call_func(uint8_t command, int res_length, const std::vector<uint8_t> params);
  std::vector<uint8_t> get_firmware_version();
  std::vector<uint8_t> read_passive_target(int baud=PN532_MIFARE_ISO14443A);
  std::vector<uint8_t> ultralight_read_page(int page);
  uint8_t ultralight_write_page(const std::vector<uint8_t> dat, int page);
  std::vector<uint8_t> pwd_auth(const std::vector<uint8_t> passwd);

private:
  int DEBUG_LVL;
  int timeout;
  int write_frame(const std::vector<uint8_t> dat);
  std::vector<uint8_t> read_frame(int res_length);
  int ack_wait();
};

#endif
