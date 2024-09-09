#pragma once
#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <map>

#include "common.hpp"
#include "Memory.hpp"

#define L1_CACHE_BIT_SIZE 10
#define L1_CACHE_SIZE     (1<<L1_CACHE_BIT_SIZE)

#define L2_CACHE_BIT_SIZE 14
#define L2_CACHE_SIZE     (1<<L2_CACHE_BIT_SIZE)

#define L3_CACHE_BIT_SIZE 18
#define L3_CACHE_SIZE     (1<<L3_CACHE_BIT_SIZE)

#define BLOCK_BIT_SIZE    4
#define BLOCK_SIZE        (1<<BLOCK_BIT_SIZE)

#define GROUP_BIT_SIZE    2
#define GROUP_SIZE        (1<<GROUP_BIT_SIZE)


struct BasicCache : public AbstractMemory {
  std::vector<u32> data;
  
  // if the block is valid
  std::map<u32, bool> valid;
  // if the block is dirty
  std::map<u32, bool> dirty;
  // the address of the block in the memory
  std::map<u32, u32> blk2memAddr;
  // count the frequency of the block
  std::map<u32, int> lfu_cnt;

  // if the block is in the cache
  std::map<u32, bool> inCache;
  // if so, we can get the index of the block by the address immediately
  std::map<u32, int> addr2idx;

  // the level of the cache
  int level;
  // the size of the cache in bit
  int cache_size_bit;
  // count of the access to the cache
  int cnt;
  // count of the hit to the cache
  int hit;

  AbstractMemory* next_cache;

  BasicCache(int level=1) 
  : level(level), cnt(0), hit(0) {

    assert(1 <= self.level && level <= 3);

    int data_size = 0;
    
    if (1 == self.level) {
      self.cache_size_bit = L1_CACHE_BIT_SIZE;
    } else if (2 == self.level) {
      self.cache_size_bit = L2_CACHE_BIT_SIZE;
    } else if (3 == self.level) {
      self.cache_size_bit = L3_CACHE_BIT_SIZE;
    } else if (3 == self.level) {
    }

    data_size = (1 << self.cache_size_bit);
    
    self.data.resize(data_size);
    for (auto& w : self.data) {
      w = MAGIC;
    }
  }

  void setNext(AbstractMemory* next_cache) {
    assert(nullptr != next_cache);
    self.next_cache = next_cache;
  }

  void load_blk(u32 addr, int blk_idx) {
    assert((addr & 0x3) == 0);
    // load the block from the next memory to the cache
    u32 mem_addr = addr & 0xfffffffc;
    for (int i = 0; i < BLOCK_SIZE; i++) {
      self.data[blk_idx*BLOCK_SIZE + i] = self.next_cache->read(mem_addr + i*4);
    }
  }

  void write_back(u32 addr, int blk_idx) {
    assert((addr & 0x3) == 0);
    // write the block back to the next memory
    u32 mem_addr = addr & 0xfffffffc;
    for (int i = 0; i < BLOCK_SIZE; i++) {
      self.next_cache->write(mem_addr + i*4, self.data[blk_idx*BLOCK_SIZE + i]);
    }
  }

  virtual u32 read(u32 addr) override {
    // the address should be aligned to 4 bytes (32 bits)
    assert((addr & 0x3) == 0);
    addr = ((addr >> 2) << 2);

    self.cnt++;

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
    
    /*
     
    memory address:
    -------------------------------------
    | blk_id 28 | group_id 2 | offset 4 |
    -------------------------------------
      
    cahce address:
    --------------------------------------
    | group_id 2 | blk_id m-2 | offset 4 |
    --------------------------------------
    m + 4 + 2 = slef.cache_size_bit
      
    */

    int group_id = (addr >> 6) & 0x3;

    int blk_begin = group_id *  GROUP_SIZE * BLOCK_SIZE;
    int blk_end = blk_begin + GROUP_SIZE * BLOCK_SIZE;

    int target_blk = -1;

    for (int i = blk_begin; i < blk_end; i += BLOCK_SIZE) {
      if (!self.valid[i]) {
        target_blk = i;
        break;
      }
    }

    // if there is no invalid block, we should find the block with the smallest lfu count to replace
    if (-1 == target_blk) {
      int min_lfu = self.lfu_cnt[blk_begin];
      target_blk = blk_begin;
      for (int i = blk_begin + BLOCK_SIZE; i < blk_end; i += BLOCK_SIZE) {
        if (self.lfu_cnt[i] < min_lfu) {
          min_lfu = self.lfu_cnt[i];
          target_blk = i;
        }
      }
    }

    // if the target block is dirty, we should write back to the next memory
    if (self.dirty[target_blk]) {
      u32 mem_addr = self.blk2memAddr[target_blk];
      self.write_back(mem_addr, target_blk);
    }

    // now we can finally load the block from the next memory to the target block
    self.load_blk(addr, target_blk);
    self.valid[target_blk] = true; 
    self.dirty[target_blk] = false;
    u32 first_word_in_blk_addr = addr & 0xfffffffc;
    self.blk2memAddr[target_blk] = first_word_in_blk_addr;
    for (u32 i = first_word_in_blk_addr; i < first_word_in_blk_addr + BLOCK_SIZE*4; i += 4) {
      self.addr2idx[i] = target_blk*BLOCK_SIZE + ((i - first_word_in_blk_addr) >> 2);
      self.inCache[i] = true;
    }
    self.lfu_cnt[target_blk] = 1;

    int offset = (addr >> 2) & 0xf;
    u32 res = self.data[target_blk * BLOCK_SIZE + offset];

    return res;
  }

  virtual void write(u32 addr, u32 val) override {
    // the address should be aligned to 4 bytes (32 bits)
    assert((addr & 0x3) == 0);
    addr = ((addr >> 2) << 2);

    self.cnt++;

    // if hit in cache
    if (self.inCache[addr]) {
      // update the hit count
      self.hit++;
      int idx = self.addr2idx[addr];
      // update the count of lfu
      self.lfu_cnt[idx]++;

      self.data[idx] = val;
      self.dirty[(idx >> BLOCK_BIT_SIZE)] = true;
    }

    int group_id = (addr >> 6) & 0x3;

    int blk_begin = group_id *  GROUP_SIZE * BLOCK_SIZE;
    int blk_end = blk_begin + GROUP_SIZE * BLOCK_SIZE;

    int target_blk = -1;

    for (int i = blk_begin; i < blk_end; i += BLOCK_SIZE) {
      if (!self.valid[i]) {
        target_blk = i;
        break;
      }
    }

    // if there is no invalid block, we should find the block with the smallest lfu count to replace
    if (-1 == target_blk) {
      int min_lfu = self.lfu_cnt[blk_begin];
      target_blk = blk_begin;
      for (int i = blk_begin + BLOCK_SIZE; i < blk_end; i += BLOCK_SIZE) {
        if (self.lfu_cnt[i] < min_lfu) {
          min_lfu = self.lfu_cnt[i];
          target_blk = i;
        }
      }
    }

    // if the target block is dirty, we should write back to the next memory
    if (self.dirty[target_blk]) {
      u32 mem_addr = self.blk2memAddr[target_blk];
      self.write_back(mem_addr, target_blk);
    }

    // now we can finally load the block from the next memory to the target block
    self.load_blk(addr, target_blk);
    self.valid[target_blk] = true; 
    self.dirty[target_blk] = true;
    u32 first_word_in_blk_addr = addr & 0xfffffffc;
    self.blk2memAddr[target_blk] = first_word_in_blk_addr;
    for (u32 i = first_word_in_blk_addr; i < first_word_in_blk_addr + BLOCK_SIZE*4; i += 4) {
      self.addr2idx[i] = target_blk;
      self.inCache[i] = true;
    }
    self.lfu_cnt[target_blk] = 1;

    int offset = (addr >> 2) & 0xf;
    self.data[target_blk * BLOCK_SIZE + offset] = val;
  }

  virtual ~BasicCache() {}

};


struct IBasicCache : public BasicCache {
  IBasicCache(int level=1)
  : BasicCache(level) {
  }

  void write(u32 addr, u32 val) override {
    // We usually can not rewrite the instruction memory.
    assert(false);
  }
};


struct DBasicCache : public BasicCache {
  DBasicCache(int level=1)
  : BasicCache(level) {
  }
};


struct ICache {
  IBasicCache l1;
  IBasicCache l2;
  IBasicCache l3;

  void setLevel(Memory* mem = nullptr) {
    self.l1.setNext(&self.l2);
    self.l2.setNext(&self.l3);
    self.l3.setNext(mem);
  }

  u32 read(u32 addr) {
    u32 res = self.l1.read(addr);
    return res;
  }

  void write(u32 addr, u32 val) {
    // We usually can not rewrite the instruction memory.
    assert(false);
  } 
};


struct DCache {
  DBasicCache l1;
  DBasicCache l2;
  DBasicCache l3;

  void setLevel(Memory* mem = nullptr) {
    self.l1.setNext(&self.l2);
    self.l2.setNext(&self.l3);
    self.l3.setNext(mem);
  }

  u32 read(u32 addr) {
    u32 res = self.l1.read(addr);
    return res;
  }

  void write(u32 addr, u32 val) {
    self.l1.write(addr, val);
  }
};


struct Cache {
  ICache icache;
  DCache dcache;

  void setLevel(Memory* mem = nullptr) {
    self.icache.setLevel(mem);
    self.dcache.setLevel(mem);
  }

  u32 read(u32 addr, bool is_instr) {
    if (is_instr) {
      return self.icache.read(addr);
    } else {
      return self.dcache.read(addr);
    }
  }

  void write(u32 addr, u32 val) {
    self.dcache.write(addr, val);
  }
};

#endif
