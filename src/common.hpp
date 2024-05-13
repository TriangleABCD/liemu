#pragma once
#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>

// type aliases for arch specific types
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// define memory struct
#define KB          1024
#define MB          1024*KB
#define MEM_SIZE    64*MB
#define MAX_MEM_IDX MEM_SIZE/4
#define MAGIC       0x0000abde

#define CODE_START  0x80000000
#define CODE_END    0x80000000+8*KB

#define STACK_BTM   CODE_START+MEM_SIZE-4*KB
#define STACK_TOP   CODE_END+MEM_SIZE-12*KB

#define HEAP_BTM  CODE_END+4*KB
#define HEAP_TOP    STACK_TOP-4*KB

/*
 
 ---------------- <- memory end
 |  padding 4KB |
 ---------------- <- stack btm
 |              |
 |              |
 |              |
 |              |
 |  stack 8KB   |
 |              |
 |              |
 |              |
 |              |
 ---------------- <- stack top
 |  padding 4KB |
 ---------------- <- heap top
 |              |
 |              |
 |              |
 |              |
 |    heap      |
 |              |
 |              |
 |              |
 |              |
 ---------------- <- heap btm
 |  padding 4KB |
 ---------------- <- code end
 |              |
 |              |
 |              |
 |              |
 |    code 8KB  |
 |              |
 |              |
 |              |
 |              |
 ---------------- <- 0x80000000 code start

 */


// split a string into words
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
