#ifndef _UTIL_H_
#define _UTIL_H_

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <stdint.h>
#include <assert.h>

std::chrono::high_resolution_clock::time_point get_time();
int elapsed_time_ms(std::chrono::high_resolution_clock::time_point end, std::chrono::high_resolution_clock::time_point begin);
void sleepms(int ms);
uint8_t checksum(std::vector<uint8_t> arr, uint8_t init);
void hprint(std::vector<uint8_t> buf, std::string info="", char separator=' ', int limited_len=0);
bool equal_slice(std::vector<uint8_t> arr1, std::vector<uint8_t> arr2, int lo, int hi);
std::vector<uint8_t> slice(std::vector<uint8_t> vec, int lo, int hi);

#endif
