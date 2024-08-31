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

// all the above size is in byte

struct BasicCache {
  std::vector<u32> data;
  
  // if the block is valid
  std::map<u32, bool> valid;
  // if the block is dirty
  std::map<u32, bool> dirty;
  // the tag of the block
  std::map<u32, u32> tag;
  // count the frequency of the block
  std::map<u32, int> lfu_cnt;

  // if the block is in the cache
  std::map<u32, bool> inCache;
  // if so, we can get the index of the block by the address immediately
  std::map<u32, int> addr2idx;

  // the level of the cache
  int level;
  // count of the access to the cache
  int cnt;
  // count of the hit to the cache
  int hit;

  Memory* mem;
  BasicCache* next_cache;

  BasicCache(int level=1) 
  : level(level), cnt(0), hit(0) {

    assert(1 <= self.level && level <= 3);

    int data_size = 0;
    
    if (1 == self.level) {
      data_size = (L1_CACHE_SIZE >> 2);
    } else if (2 == self.level) {
      data_size = (L2_CACHE_SIZE >> 2);
    } else if (3 == self.level) {
      data_size = (L3_CACHE_SIZE >> 2);
    }
    
    self.data.resize(data_size);
    for (auto& w : self.data) {
      w = MAGIC;
    }
  }

  void setNext(Memory* mem, BasicCache* next_cache) {
    self.mem = mem;
    self.next_cache = next_cache;
  }

  virtual u32 read(u32 addr) = 0;
  virtual void write(u32 addr, u32 val) = 0;

  virtual ~BasicCache() {}

};


struct IBasicCache : public BasicCache {
  IBasicCache(int level=1)
  : BasicCache(level) {
  }

  u32 read(u32 addr) override {
    // the address should be aligned to 4 bytes (32 bits)
    assert((addr & 0x3) == 0);
    addr >>= 2;

    // if hit in cache
    if (self.inCache[addr]) {
      // update the hit count
      self.hit++;
      int idx = self.addr2idx[addr];
      // update the count of lfu
      self.lfu_cnt[idx]++;

      return self.data[idx];
    }

    // if not hit in cache, we should load the block from the next memory

    // fisrtly, we should find a position to put the loaded block in this cache
    int 


    return 0;
  }

  void write(u32 addr, u32 val) override {
    // We usually can not rewrite the instruction memory.
    assert(false);
  }
};


struct DBasicCache : public BasicCache {
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
