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
