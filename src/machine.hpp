#pragma once
#ifndef MACHINE_H
#define MACHINE_H

#include "CPU.hpp"
#include "memory.hpp"
#include "inst.hpp"

struct Machine {
  CPU cpu;
  Memory mem;

  int execute_one_step() {
    Inst inst = this->mem.read_vmem(cpu.pc);
    int r = parse_inst(inst);
    this->cpu.pc += 4;
    return r != 0;
  }

  void execute() {
    while (1) {
      int r = execute_one_step();
      if (1 == r) {
        break;
      } 
      if (-1 == r) {
        fprintf(stderr, "exec error\n");
        break;
      }
    }
  }
};

#endif
