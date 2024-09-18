#pragma once
#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>

#define GREEN(x) "\033[32m" x "\033[0m"
#define RED(x) "\033[31m" x "\033[0m"

#define self (*this)

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
#define MAX_MEM_IDX MEM_SIZE/sizeof(u32)
#define MAGIC       0xabde

#define MEM_START   0x80000000
#define MEM_END     MEM_START+MEM_SIZE

#define TRAMP_TOP   MEM_END
#define TRAPFRAME   TRAMP_TOP-4*KB
#define TRAMP_BTM   TRAPFRAME

#define STACK_BTM   TRAPFRAME-5*KB
#define STACK_TOP   STACK_BTM-8*KB

#define CODE_START  MEM_START
#define CODE_END    CODE_START+8*KB

#define HEAP_TOP    STACK_TOP-1*KB
#define HEAP_BTM    CODE_END+1*KB

/*
 
 ---------------- <- memory end = 0x84000000
 |              |
 |trampoline 4KB|
 |              |
 ---------------- <- trapframe btm = trampline btm
 |              |
 |trapframe 4KB |
 |              |
 ----------------
 |  padding 1KB |
 ---------------- <- stack btm = 0x83ffdc00
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
 |  padding 1KB |
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
 |  padding 1KB |
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
 ---------------- <- memory start = code start = 0x80000000

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
