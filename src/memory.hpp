#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include <assert.h>

#include "liemu.hpp"

#define KB 1024
#define MB 1024*KB
#define MEM_SIZE 64*MB
#define MAX_MEM_IDX MEM_SIZE/4


struct Memory {
  u32 mem[MAX_MEM_IDX];

  Memory() {
    for (int i = 0; i < MAX_MEM_IDX; i++) {
      this->write_pmem(i, MAGIC);
    }
  }

  u32 read_pmem(u32 addr) {
    assert(addr >= 0 && addr < MEM_SIZE);
    return this->mem[addr >> 2];
  }

  void write_pmem(u32 addr, u32 data) {
    assert(addr >= 0 && addr < MEM_SIZE);
    this->mem[addr >> 2] = data; 
  }
  
  u32 read_vmem(u32 vaddr) {
    u32 paddr = vaddr - START_ADDR;
    assert(paddr >= 0 && paddr < MEM_SIZE);
    return this->mem[paddr >> 2];
  }

  void write_vmem(u32 vaddr, u32 data) {
    u32 paddr = vaddr - START_ADDR;
    assert(paddr >= 0 && paddr < MEM_SIZE);
    this->mem[paddr >> 2] = data; 
  }
};

#endif
