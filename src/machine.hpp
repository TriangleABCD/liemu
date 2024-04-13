#pragma once
#ifndef MACHINE_H
#define MACHINE_H

#include <cstdio>
#include <assert.h>
#include <string>
#include <vector>
#include <algorithm>

#include "types.hpp"

#define KB 1024
#define MB 1024*KB
#define MEM_SIZE 64*MB
#define MAX_MEM_IDX MEM_SIZE/4

#define MAGIC 0xee
#define START 0x80000000

struct Machine {
  struct {
    u32 gp_regs[32];
    u32 pc;
  } cpu;

  std::vector<std::string> reg_names {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
  };
  
  u32 mem[MAX_MEM_IDX];

  Machine() {
    Machine::cpu.gp_regs[0] = 0;
    for (int i = 1; i < 32; i++) {
      Machine::cpu.gp_regs[i] = MAGIC;
    }
    Machine::cpu.pc = START;

    for (int i = 0; i < MAX_MEM_IDX; i++) {
      Machine::write_pmem(i, MAGIC);
    }
  }

  u32 read_pmem(u32 addr) {
    assert(addr >= 0 && addr < MAX_MEM_IDX);
    return Machine::mem[addr];
  }

  void write_pmem(u32 addr, u32 data) {
    assert(addr >= 0 && addr < MAX_MEM_IDX);
    Machine::mem[addr] = data; 
  }
  
  u32 read_vmem(u32 vaddr) {
    u32 paddr = vaddr - START;
    assert(paddr >= 0 && paddr < MAX_MEM_IDX);
    return Machine::mem[paddr];
  }

  void write_vmem(u32 vaddr, u32 data) {
    u32 paddr = vaddr - START;
    assert(paddr >= 0 && paddr < MAX_MEM_IDX);
    Machine::mem[paddr] = data; 
  }

  int reg2idx(std::string reg_name) {
    auto it = std::find(Machine::reg_names.begin(), Machine::reg_names.end(), reg_name);
    if (it == Machine::reg_names.end()) {
      return -1;
    }
    return std::distance(Machine::reg_names.begin(), it);
  }

  void info_reg(std::string reg = "") {
    if ("pc" == reg) {
      printf("%s:\t0x%08x\n", reg.c_str(), Machine::cpu.pc);
      return;
    }
    if ("" != reg) {
      printf("%s:\t0x%08x\n", reg.c_str(), Machine::cpu.gp_regs[Machine::reg2idx(reg)]);
      return;
    }
    for (int i = 0; i < 32; i++) {
      auto& name = Machine::reg_names[i];
      printf("%s:\t0x%08x\n", name.c_str(), Machine::cpu.gp_regs[i]);
    }
    printf("pc:\t0x%08x\n", Machine::cpu.pc);
  }
};

#endif
