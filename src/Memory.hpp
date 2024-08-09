#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include <assert.h>
#include <string>
#include <fstream>

#include "common.hpp"

struct Memory {
  u32 mem[MAX_MEM_IDX];

  void resetMemory() {
    for (auto i = MAX_MEM_IDX - 1; i >= 0; i--) {
      this->write_pmem(i, MAGIC);
    }
  }

  void load_insts_into_mem(std::string path) {
    std::fstream file(path);
    if (!file.is_open()) {
      fprintf(stderr, RED("failed to open file: %s\n"), path.c_str());
      return;
    }
    std::string line;
    std::vector<u32> insts;
    while(file >> line) {
      u32 inst = strtol(line.c_str(), NULL, 16);
      insts.push_back(inst);
    }
    file.close();
    u32 addr = MEM_START;
    for (const auto& inst : insts) {
      this->write(addr, inst);
      addr += 4;
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
  
  u32 read(u32 vaddr) {
    u32 paddr = vaddr - MEM_START;
    assert(paddr >= 0 && paddr < MEM_SIZE);
    return this->mem[paddr >> 2];
  }

  void write(u32 vaddr, u32 data) {
    u32 paddr = vaddr - MEM_START;
    assert(paddr >= 0 && paddr < MEM_SIZE);
    this->mem[paddr >> 2] = data; 
  }
};

#endif
