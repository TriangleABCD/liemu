#pragma once
#ifndef MACHINE_H
#define MACHINE_H

#include "common.hpp"
#include "CPU.hpp"
#include "Memory.hpp"
#include "Cache.hpp"

struct Machine {
  CPU cpu;
  Memory memory;
  Cache cache;

  Machine() {
    self.cache.setLevel(&self.memory);
  }

  u32 readMem(u32 addr) {
    if (addr >= CODE_START && addr < CODE_END) {
      return self.cache.read(addr, true);
    }
    return self.cache.read(addr, false);
  }

  void writeMem(u32 addr, u32 data) {
    if (addr >= CODE_START && addr < CODE_END) {
      assert(false);
    }
    self.cache.write(addr, data);
  }

};

#endif
