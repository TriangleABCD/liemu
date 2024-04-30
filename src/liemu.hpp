#pragma once
#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

#define MAGIC 0x0000abde
#define START_ADDR 0x80000000

#define KB 1024
#define MB 1024*KB
#define MEM_SIZE 64*MB
#define MAX_MEM_IDX MEM_SIZE/4

#define STACK_BTM START_ADDR+MEM_SIZE-4*KB
#define STACK_TOP START_ADDR+MEM_SIZE-12*KB

inline std::vector<std::string> split(std::string str) {
  std::vector<std::string> words;
  std::istringstream ss(str);

  std::string word;
  while (ss >> word) {
    words.push_back(word);
  }

  return words;
}

#endif
