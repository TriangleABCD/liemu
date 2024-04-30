#pragma once
#ifndef MACHINE_H
#define MACHINE_H

#include <map>

#include "CPU.hpp"
#include "memory.hpp"
#include "inst.hpp"

struct Machine {
  CPU cpu;
  Memory mem;
  std::map<u32, Inst> getInst;
  
  void read_insts() {
    auto pc = cpu.pc;
    while (1) {
      u32 inst = this->mem.read_vmem(pc);
      if (inst == MAGIC) {
        break;
      }
      auto ins = parse_inst(inst, this->cpu, this->mem);
      if (ins.result == -1) {
        fprintf(stderr, "\033[31mparse error\033[0m\n");
        break;
      }
      this->getInst[pc] = ins;
      pc += 4;
    }
  }

  int execute_one_step() {
    if (cpu.pc < START_ADDR) {
      return 1;
    }
    Inst inst = this->getInst[this->cpu.pc];
    int r = inst.doit(inst, this->cpu, this->mem);
    this->cpu.pc += 4;
    if (-1 == r) {
      fprintf(stderr, "\033[31mexec error\033[0m\n");
      return r;
    }
    return r != 0;
  }

  int execute() {
    int r = 0;
    while (1) {
      r = execute_one_step();
      if (-1 == r || 1 == r) {
        break;
      }
    }
    return r;
  }
};

#endif
