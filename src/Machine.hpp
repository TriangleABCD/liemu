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

  u32 readMem(u32 addr) {
    return Machine::memory.read(addr);
  }

  void writeMem(u32 addr, u32 data) {
    Machine::memory.write(addr, data);
  }
};

#endif
