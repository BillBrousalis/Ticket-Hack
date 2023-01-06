#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <libserial/SerialPort.h>

#define DEFAULT_TIMEOUT 250

class Serial {
public:
  Serial();
  ~Serial();
  void open_serial(std::string const dev_name);
  void close_serial();
  uint8_t readbyte(int timeout=DEFAULT_TIMEOUT);
  std::vector<uint8_t> read(int expected_sz, int timeout=DEFAULT_TIMEOUT);
  void write(std::vector<uint8_t> buf);

private:
  LibSerial::SerialPort serial;
};

#endif
