/* Dump Memory of Mifare Ultralight EV1 Card */

#include <iostream>
#include "pn532.h"

int main()
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

  std::cout << "[-Dumping Memory-]" << std::endl;
  std::vector<uint8_t> data;
  for(int page=0; page<MAXPAGES; ++page) {
    data = dev.ultralight_read_page(page);
    printf("Page [%02x] -- ", page);
    for(int ii=0; ii<PAGELENGTH; ++ii) {
      printf("%02x", data[ii]);
      if(ii != 3) printf(" : ");
    }
    std::cout << std::endl;
  }

  std::cout << std::endl << "[-DONE-]" << std::endl;
  return 0;
}
