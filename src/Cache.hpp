#pragma once
#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <map>

#include "common.hpp"
#include "Memory.hpp"

#define L1_CACHE_SIZE 8*KB
#define L2_CACHE_SIZE 64*KB
#define L3_CACHE_SIZE 512*KB

#define BLOCK_SIZE    64

struct CacheBlock {
  bool valid;
  bool dirty;
  u32 tag;
  u32 data[(BLOCK_SIZE>>2)];

  void resetBlock() {
    self.valid = false;
    self.dirty = false;
    self.tag = 0;
    for (int i = 0; i < (BLOCK_SIZE>>2); i++) {
      self.data[i] = MAGIC;
    }
  }
};

struct CacheSet {
  int level;

  std::vector<CacheBlock> blocks;
  std::map<u32, bool> inCache;
  std::map<u32, CacheBlock&> tag_map;

  int total_cnt;
  int hit_cnt;

  CacheSet(int level = 3, int size = L1_CACHE_SIZE) {
    self.level = level;
    self.blocks.resize(size/BLOCK_SIZE);
    for (auto& block : self.blocks) {
      block.resetBlock();
    }

    self.total_cnt = 0;
    self.hit_cnt = 0;
  }

  u32 read(Memory& mem, CacheSet& nextCache, u32 addr) {
    assert(addr % 4 == 0);
    if (self.inCache[addr]) {
      self.total_cnt++;
      self.hit_cnt++;
      return self.tag_map[addr].data[addr%BLOCK_SIZE];
    }
  }

  u32 write(Memory& mem, CacheSet& nextCache, u32 addr, u32 val) {

  }
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
