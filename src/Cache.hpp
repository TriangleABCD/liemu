#pragma once
#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <map>

#include "common.hpp"
#include "Memory.hpp"

#define L1_CACHE_BIN_SIZE 6
#define L1_CACHE_SIZE     (1<<L1_CACHE_BIN_SIZE)

#define L2_CACHE_BIN_SIZE 8
#define L2_CACHE_SIZE     (1<<L2_CACHE_BIN_SIZE)

#define L3_CACHE_BIN_SIZE 10
#define L3_CACHE_SIZE     (1<<L3_CACHE_BIN_SIZE)

#define LINE_BIN_SIZE    4
#define LINE_SIZE        (1<<LINE_BIN_SIZE)

#define GROUP_BIN_SIZE    2
#define GROUP_SIZE        (1<<GROUP_BIN_SIZE)


struct BasicCache : public AbstractMemory {

  std::vector<u32> data;
  int level;
  int cache_bin_size;
  int cnt;
  int hit;

  std::map<u32, bool> valid;
  std::map<u32, bool> dirty;
  std::map<u32, u32> line2memAddr;
  std::map<u32, int> lfu_cnt;

  std::map<u32, bool> inCache;
  std::map<u32, int> addr2line;

  AbstractMemory* next_cache;

  BasicCache(int level=1) 
  : level(level), cnt(0), hit(0), next_cache(nullptr) {

    assert(1 <= self.level && level <= 3);

    int data_size = 0;
    
    if (1 == self.level) {
      self.cache_bin_size = L1_CACHE_BIN_SIZE;
    } else if (2 == self.level) {
      self.cache_bin_size = L2_CACHE_BIN_SIZE;
    } else if (3 == self.level) {
      self.cache_bin_size = L3_CACHE_BIN_SIZE;
    } else {
    }

    data_size = (1 << self.cache_bin_size);
    
    self.data.resize(data_size);
    for (auto& w : self.data) {
      w = MAGIC;
    }
  }

  void showHit() {
    printf("L%d Cache Hit Rate: %d / %d\n", self.level, self.hit, self.cnt); 
  }

  void setNext(AbstractMemory* next_cache) {
    assert(nullptr != next_cache);
    self.next_cache = next_cache;
  }

  void load_line(u32 addr, int idx) {
    addr = (addr & 0xffffffc0);
    for (int i = 0; i < LINE_SIZE; i++) {
      self.data[idx + i] = self.next_cache->read(addr + i*4);
    }
  }

  void write_back(u32 addr, int idx) {
    addr = (addr & 0xffffffc0);
    for (int i = 0; i < LINE_SIZE; i++) {
      self.next_cache->write(addr + i*4, self.data[idx + i]);
    }
  }

  virtual u32 read(u32 addr) override {

    assert((addr & 0x3) == 0);

    self.cnt++;

    /*
    memory address:
    ----------------------------------
    | 24 | group_id 2 | offset 4 | 2 |
    ----------------------------------
    */

    int offset = (addr >> 2) & 0xf;
    int group_id = (addr >> 6) & 0x3;

    if (self.inCache[(addr & 0xffffffc0)]) {
      self.hit++;
      int idx = self.addr2line[(addr & 0xffffffc0)];
      self.lfu_cnt[idx]++;
      return self.data[idx + offset];
    }

    int idx_begin = group_id *  GROUP_SIZE * LINE_SIZE;
    int idx_end = idx_begin + GROUP_SIZE * LINE_SIZE;

    int target_line = -1;

    for (int i = idx_begin; i < idx_end; i += LINE_SIZE) {
      if (!self.valid[i]) {
        target_line = i;
        break;
      }
    }

    if (-1 == target_line) {
      int min_lfu = self.lfu_cnt[idx_begin];
      target_line = idx_begin;
      for (int i = idx_begin; i < idx_end; i += LINE_SIZE) {
        if (self.lfu_cnt[i] < min_lfu) {
          min_lfu = self.lfu_cnt[i];
          target_line = i;
        }
      }
    }

    if (self.dirty[target_line]) {
      u32 mem_addr = self.line2memAddr[target_line];
      self.write_back(mem_addr, target_line);
    }

    self.load_line(addr, target_line);
    self.valid[target_line] = true; 
    self.dirty[target_line] = false;
    u32 first_word_addr = (addr & 0xffffffc0);
    self.line2memAddr[target_line] = first_word_addr;
    for (u32 i = first_word_addr; i < first_word_addr + LINE_SIZE*4; i += 4) {
      self.addr2line[i] = target_line;
      self.inCache[i] = true;
    }
    self.lfu_cnt[target_line] = 1;

    u32 res = self.data[target_line + offset];

    return res;
  }

  virtual void write(u32 addr, u32 val) override {

    assert((addr & 0x3) == 0);

    self.cnt++;

    int offset = (addr >> 2) & 0xf;
    int group_id = (addr >> 6) & 0x3;

    if (self.inCache[(addr & 0xffffffc0)]) {
      self.hit++;
      int idx = self.addr2line[(addr & 0xffffffc0)];
      self.lfu_cnt[idx]++;
      self.data[idx + offset] = val;
      return;
    }

    int idx_begin = group_id *  GROUP_SIZE * LINE_SIZE;
    int idx_end = idx_begin + GROUP_SIZE * LINE_SIZE;

    int target_line = -1;

    for (int i = idx_begin; i < idx_end; i += LINE_SIZE) {
      if (!self.valid[i]) {
        target_line = i;
        break;
      }
    }

    if (-1 == target_line) {
      int min_lfu = self.lfu_cnt[idx_begin];
      target_line = idx_begin;
      for (int i = idx_begin; i < idx_end; i += LINE_SIZE) {
        if (self.lfu_cnt[i] < min_lfu) {
          min_lfu = self.lfu_cnt[i];
          target_line = i;
        }
      }
    }

    if (self.dirty[target_line]) {
      u32 mem_addr = self.line2memAddr[target_line];
      self.write_back(mem_addr, target_line);
    }

    self.load_line(addr, target_line);
    self.valid[target_line] = true; 
    self.dirty[target_line] = false;
    u32 first_word_addr = (addr & 0xffffffc0);
    self.line2memAddr[target_line] = first_word_addr;
    for (u32 i = first_word_addr; i < first_word_addr + LINE_SIZE*4; i += 4) {
      self.addr2line[i] = target_line;
      self.inCache[i] = true;
    }
    self.lfu_cnt[target_line] = 1;

    self.data[target_line + offset] = val;
  }

  virtual ~BasicCache() {}

};


struct CacheMod {
  BasicCache l1;
  BasicCache l2;
  BasicCache l3;

  CacheMod(int a = 1, int b = 2, int c = 3) : l1(a), l2(b), l3(c) {

  }

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

  void showHit() {
    self.l1.showHit();
    self.l2.showHit();
    self.l3.showHit();
  }
};


struct Cache {
  CacheMod icache;
  CacheMod dcache;

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

  void showHit() {
    printf("I Cache:\n");
    self.icache.showHit();
    printf("D Cache:\n");
    self.dcache.showHit();
  }
};

#endif
