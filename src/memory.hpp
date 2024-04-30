#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include <assert.h>
#include <string>
#include <fstream>

#include "common.hpp"

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

  void load_insts_into_mem(std::string path) {
    std::fstream file(path);
    if (!file.is_open()) {
      fprintf(stderr, "\033[31mfailed to open file: %s\033[0m\n", path.c_str());
      return;
    }
    std::string line;
    std::vector<u32> insts;
    while(file >> line) {
      u32 inst = strtol(line.c_str(), NULL, 16);
      insts.push_back(inst);
    }
    u32 addr = START_ADDR;
    for (const auto& inst : insts) {
      this->write_vmem(addr, inst);
      addr += 4;
    }
  }
};

#endif
