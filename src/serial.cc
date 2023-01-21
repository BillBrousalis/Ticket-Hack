#include "serial.h"

Serial::Serial(){}

Serial::~Serial()
{
  close_serial();
}

void Serial::open_serial(std::string const dev_name)
{
  serial.Open(dev_name);
  serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
  serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  serial.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  serial.SetParity(LibSerial::Parity::PARITY_NONE);
  serial.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
}

void Serial::close_serial()
{
  serial.Close();
}

uint8_t Serial::readbyte(int timeout)
{
  char c = 0x00;
  serial.ReadByte(c, timeout);
  return (uint8_t)c;
}

std::vector<uint8_t> Serial::read(int expected_sz, int timeout)
{
  std::vector<uint8_t> buf;
  try {
    for(int i=0; i<expected_sz; ++i) {
      buf.push_back(readbyte());
    }
  }
  catch(const LibSerial::ReadTimeout&) {}
  return buf;
}

void Serial::write(const std::vector<uint8_t> buf)
{
  for(int i=0; i<(int)buf.size(); ++i) {
    serial.WriteByte(buf[i]);
  }
}
