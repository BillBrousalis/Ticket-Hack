/* Dump & Save Memory of Mifare Ultralight EV1 Card */

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "pn532.h"

int main()
{
  const char *fname = getenv("FNAME");
  std::ofstream savefile;
  if(fname == NULL) {
    savefile.open("memory_dumps/latest_memdump.txt");
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
  return 0;
}
