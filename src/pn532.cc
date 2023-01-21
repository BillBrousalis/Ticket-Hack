#include "pn532.h"

/* Wakeup - ACK Sequences */
std::vector<uint8_t> wakeup_seq {0x55, 0x55, 0x00, 0x00, 0x00};
std::vector<uint8_t> pn532_ack {0x00, 0x00, 0xff, 0x00, 0xff, 0x00};

pn532::pn532(std::string const dev_name, int t)
{
  /* Set DEBUG env var (values 0 - 5) for debugging messages */
  const char *env = getenv("DEBUG");
  if(env == NULL) {
    DEBUG_LVL = 0;
  }
  else {
    DEBUG_LVL = std::stoi(env);
  }
  std::cout << "[ DEBUG LEVEL = " << DEBUG_LVL << " ]" << std::endl;

  timeout = t;
  std::cout << "[+] Using device " << dev_name << std::endl;
  ser.open_serial(dev_name);
}

pn532::~pn532()
{
  std::cout << "[-] Closing PN532..." << std::endl;
}

int pn532::write_frame(const std::vector<uint8_t> dat)
{
  if(DEBUG_LVL >= 3) { std::cout << std::endl << "( pn532::write_frame )" << std::endl; }
  int sz = (int)dat.size();
  assert(0 < sz  && sz < 0xff);
  int frame_sz = sz + 7;
  std::vector<uint8_t> frame(sz+7, 0x00);
  frame[0] = PN532_PREAMBLE;
  frame[1] = PN532_STARTCODE1;
  frame[2] = PN532_STARTCODE2;
  frame[3] = sz;
  frame[4] = (~sz + 0x01) & 0xff;
  for(int i=0; i<sz; ++i) {
    frame[5+i] = dat[i];
  }
  uint8_t chksum = checksum(dat, 0xff);
  frame[frame_sz-2] = ~chksum & 0xff;
  frame[frame_sz-1] = PN532_POSTAMBLE;
  int ack = 0;
  while(!ack) {
    if(DEBUG_LVL >= 1) { hprint(frame, "( write_frame ) frame:"); }
    ser.write(frame);
    ack = ack_wait();
    //sleepms(1);
  }
  return ack;
}

/* echo test (loopback cable) */
void pn532::echo_test()
{
  std::vector<uint8_t> echo_bytes {0x0a, 0x0b, 0xa0, 0xb0, 0x10, 0xff, 0x00, 0x01, 0x02, 0x03};
  hprint(echo_bytes, "[+] ECHO: Sending");
  ser.write(echo_bytes);
  std::vector<uint8_t> bufrecv = ser.read((int)echo_bytes.size());
  hprint(bufrecv, "[+] ECHO: Received");
  std::cout << "[+] Echo test complete." << std::endl;
}

std::vector<uint8_t> pn532::read_frame(int res_length)
{
  if(DEBUG_LVL >= 3) { std::cout << std::endl << "( pn532::read_frame )" << std::endl; }

  std::vector<uint8_t> response = ser.read(res_length+8);

  if(DEBUG_LVL >= 2) { hprint(response, "( pn532::read_frame ) response:"); }

  assert(pn532_ack.size() <= response.size());
  if(equal_slice(pn532_ack, response, 0, (int)pn532_ack.size())) {
    std::cerr << "[-] !!! NO CARD !!!" << std::endl;
    throw std::runtime_error("[-] !!! NO CARD !!!");
  }
  if(response[0] != 0x00) {
    throw std::runtime_error("[-] !!! response[0] != 0x00 !!!");
  }
  int off = 1;
  while(response[off] == 0x00) {
    off++;
    if(off >= (int)response.size()) {
      throw std::runtime_error("[-] !!! Response frame preamble does not contain 00:FF (1) !!!");
    }
  }
  if(response[off] != 0xff) {
    throw std::runtime_error("[-] !!! Response frame preamble does not contain 00:FF (2) !!!");
  }
  off++;
  if(off >= (int)response.size()) {
    throw std::runtime_error("[-] !!! Response does not contain data !!!");
  }
  uint8_t frame_len = response[off];
  if(((frame_len + response[off+1]) & 0xff) != 0x00) {
    throw std::runtime_error("[-] !!! Response length checksum does not match length !!!");
  }
  std::vector<uint8_t> frame = slice(response, off+2, off+2+frame_len+1);//(frame_len+1, 0x00);

  if(DEBUG_LVL >= 2) {
    printf("frame_len = 0x%02x\n", frame_len);
    hprint(frame, "( pn532::read_frame ) frame:");
  }

  if(checksum(frame, 0x00) != 0x00) {
    throw std::runtime_error("[-] !!! Response checksum is not 0x00 !!!");
  }
  return frame;
}

int pn532::ack_wait()
{
  if(DEBUG_LVL >= 3) { std::cout << std::endl << "( pn532::ack_wait )" << std::endl; }

  int ack = 0;
  int rx_size = pn532_ack.size();
  std::vector<uint8_t> rx (rx_size, 0x00);

  auto start = get_time();
  auto now = get_time();
  while(elapsed_time_ms(now, start) < timeout && ack != 1) {
    //sleepms(1);
    rx.erase(rx.begin());
    rx.push_back(ser.readbyte(timeout=timeout));
    if(pn532_ack == rx) {
      ack = 1;
    }
    now = get_time();
    if(DEBUG_LVL >= 4) { hprint(rx, "( pn532::ack_wait ) RX:"); }
  }
  if(DEBUG_LVL >= 4) { std::cout << "( ACK = " << ack << " )" << std::endl; }
  return ack;
}

void pn532::wakeup()
{
  if(DEBUG_LVL >= 3) { std::cout << std::endl << "( pn532::wakeup )" << std::endl; }
  ser.write(wakeup_seq);
}

void pn532::SAM_config()
{
  if(DEBUG_LVL >= 3) { std::cout << std::endl << "( pn532::SAM_config )" << std::endl; }
  std::vector<uint8_t> params {0x01, 0x14, 0x01};
  call_func(PN532_COMMAND_SAMCONFIGURATION, 0, params);
}

std::vector<uint8_t> pn532::call_func(uint8_t command, int res_length, const std::vector<uint8_t> params)
{
  if(DEBUG_LVL >= 3) { std::cout << std::endl << "( pn532::call_func )" << std::endl; }

  std::vector<uint8_t> data {PN532_HOSTTOPN532, command};
  for(int i=0; i<(int)params.size(); ++i) {
    data.push_back(params[i]);
  }

  if(DEBUG_LVL >= 1) { hprint(data, "( pn532::call_func ) Sending data:"); }

  if(!write_frame(data)) {
    return std::vector<uint8_t>();
  }

  std::vector<uint8_t> response = read_frame(res_length+3);

  if(DEBUG_LVL >= 2) { hprint(response, "( pn532::call_func ) read_frame response:"); }

  if(response[0] != PN532_PN532TOHOST || response[1] != (command+0x01)) {
    throw std::runtime_error("!!! Unexpected command response !!!");
  }

  if(res_length == 0) {
    return std::vector<uint8_t>();
  }

  return slice(response, 2, (int)response.size());
}

std::vector<uint8_t> pn532::get_firmware_version()
{
  if(DEBUG_LVL >= 3) { std::cout << std::endl << "( pn532::get_firmware_version )" << std::endl; }
  return call_func(PN532_COMMAND_GETFIRMWAREVERSION, FIRMWARE_VERSION_LEN, std::vector<uint8_t>());
}

std::vector<uint8_t> pn532::read_passive_target(int baud)
{
  std::vector<uint8_t> params {0x01, (uint8_t)baud};
  std::vector<uint8_t> response = call_func(PN532_COMMAND_INLISTPASSIVETARGET, 17, params);
  assert(response[0] == 0x01);
  assert(response[5] <= 0x07);
  return slice(response, 6, 6+response[5]);
}

std::vector<uint8_t> pn532::ultralight_read_page(int page)
{
  assert(0 <= page && page < MAXPAGES);
  if(DEBUG_LVL >= 3) { std::cout << std::endl << "( pn532::ultralight_read_page )" << std::endl; }
  std::vector<uint8_t> params {0x01, ULTRALIGHT_CMD_READ, (uint8_t)page};
  std::vector<uint8_t> response = call_func(PN532_COMMAND_INDATAEXCHANGE, 17, params);
  assert(response[0] == 0x00);
  return slice(response, 1, PAGELENGTH+1);
}

uint8_t pn532::ultralight_write_page(const std::vector<uint8_t> dat, int page)
{
  assert(dat.size() == PAGELENGTH);
  assert(0 <= page && page < MAXPAGES);
  if(DEBUG_LVL >= 3) { std::cout << std::endl << "( pn532::ultralight_write_page )" << std::endl; }
  //std::vector<uint8_t> params {0x01, ULTRALIGHT_CMD_COMPWRITE, (uint8_t)page}; // compatibility-write
  std::vector<uint8_t> params {0x01, ULTRALIGHT_CMD_WRITE, (uint8_t)page};
  for(int i=0; i<(int)dat.size(); ++i) {
    params.push_back(dat[i]);
  }
  std::vector<uint8_t> response = call_func(PN532_COMMAND_INDATAEXCHANGE, 1, params);
  assert(response.size() > 0);
  return response[0];
}

std::vector<uint8_t> pn532::pwd_auth(const std::vector<uint8_t> passwd)
{
  assert(passwd.size() == 4);
  std::vector<uint8_t> params {0x01, ULTRALIGHT_CMD_PWDAUTH};
  for(int i=0; i<4; ++i) {
    params.push_back(passwd[i]);
  }
  std::vector<uint8_t> response = call_func(PN532_COMMAND_INDATAEXCHANGE, 17, params);
  return response;
}
