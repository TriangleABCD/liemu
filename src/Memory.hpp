#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include <assert.h>

#include "common.hpp"

struct AbstractMemory {
  virtual u32 read(u32 addr) = 0;
  virtual void write(u32 addr, u32 data) = 0;
};

struct Memory : public AbstractMemory {
  u32 mem[MAX_MEM_IDX];

  void resetMemory() {
    for (unsigned i = 0; i < MAX_MEM_IDX; i++) {
      self.mem[i] = MAGIC;
    }
  }

  u32 read_pmem(u32 addr) {
    assert(addr >= 0 && addr < MEM_SIZE);
    return self.mem[addr >> 2];
  }

  void write_pmem(u32 addr, u32 data) {
    assert(addr >= 0 && addr < MEM_SIZE);
    self.mem[addr >> 2] = data; 
  }
  
  virtual u32 read(u32 vaddr) override {
    u32 paddr = vaddr - MEM_START;
    return self.read_pmem(paddr);
  }

  virtual void write(u32 vaddr, u32 data) override {
    u32 paddr = vaddr - MEM_START;
    self.write_pmem(paddr, data);
  }
};

#endif
