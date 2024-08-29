#pragma once
#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <map>

#include "common.hpp"
#include "Memory.hpp"

#define L1_CACHE_BIT_SIZE 12
#define L1_CACHE_SIZE     (1<<L1_CACHE_BIT_SIZE)

#define L2_CACHE_BIT_SIZE 16
#define L2_CACHE_SIZE     (1<<L2_CACHE_BIT_SIZE)

#define L3_CACHE_BIT_SIZE 20
#define L3_CACHE_SIZE     (1<<L3_CACHE_BIT_SIZE)

#define BLOCK_BIT_SIZE    6
#define BLOCK_SIZE        (1<<BLOCK_BIT_SIZE)

#define GROUP_BIT_SIZE    2
#define GROUP_SIZE        (1<<GROUP_BIT_SIZE)

struct BasicCache {
  std::vector<u32> data;
  std::map<u32, bool> valid;
  std::map<u32, bool> dirty;
  std::map<u32, int> lfu_cnt;

  std::map<u32, bool> inCache;
  std::map<u32, int> addr2idx;

  int cnt;
  int hit;
};

struct ICache {
 
};

struct DCache {

};

struct Cache {
  ICache icache;
  DCache dcache;
};

#endif
