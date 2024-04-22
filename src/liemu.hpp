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

using Inst = u32;

#define MAGIC 0x0000abde
#define START_ADDR 0x80000000

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
