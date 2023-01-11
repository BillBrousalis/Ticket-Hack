/*
* AUTHOR: Bill Brousalis
* DATE: 11 Jan. 2023
*/

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "pn532.h"

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
    std::cout << std::endl;
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

  std::cout << "[-Memory Overwriting-]\n" << std::endl;
  std::vector<uint8_t> data {0x0, 0x0, 0x0, 0x0};
  for(int i=4; i<30; ++i) {
    uint8_t out = dev.ultralight_write_page(data, i);
    printf("out = 0x%02x\n", out);
  }
}

void menu()
{
  std::cout << "\n[ Ticket-Hack ]\n";
  std::cout << "1. Dump card memory for cloning\n";
  std::cout << "2. Overwrite card memory\n> ";
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
  else
    std::cout << "Invalid option..." << std::endl;

  return 0;
}
