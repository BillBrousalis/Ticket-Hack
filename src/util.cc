#include "util.h"

std::chrono::high_resolution_clock::time_point get_time()
{
  return std::chrono::high_resolution_clock::now();
}

int elapsed_time_ms(std::chrono::high_resolution_clock::time_point end, std::chrono::high_resolution_clock::time_point begin)
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

void sleepms(int ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

uint8_t checksum(const std::vector<uint8_t> arr, uint8_t init)
{
  uint8_t acc = init;
  for(int i=0; i<(int)arr.size(); ++i) {
    acc += arr[i];
    acc &= 0xff;
  }
  return acc;
}

void hprint(const std::vector<uint8_t> buf, std::string info, char separator, int limited_len)
{
  assert(limited_len >= 0 && limited_len <= (int)buf.size());
  int len;
  if(limited_len == 0) { len = (int)buf.size(); }
  else { len = limited_len; }
  std::cout << info << std::endl;;
  for(int i=0; i<len; ++i) {
    printf("%02x", buf[i]);
    if(i != len-1) { printf(" %c ", separator); }
  }
  std::cout << std::endl;
}

bool equal_slice(const std::vector<uint8_t> arr1, const std::vector<uint8_t> arr2, int lo, int hi)
{
  assert(lo >= 0 && lo < hi && hi <= (int)std::min(arr1.size(), arr2.size()));
  for(int i=lo; i<hi; ++i) {
    if(arr1[i] != arr2[i]) { return false; }
  }
  return true;
}

std::vector<uint8_t> slice(const std::vector<uint8_t> vec, int lo, int hi)
{
  assert(lo >= 0 && lo < hi && hi <= (int)vec.size());
  std::vector<uint8_t> sliced (hi-lo);
  std::copy(vec.begin()+lo, vec.begin()+hi, sliced.begin());
  return sliced;
}
