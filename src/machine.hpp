#pragma once
#ifndef MACHINE_H
#define MACHINE_H

#include <iostream>

#include "CPU.hpp"
#include "memory.hpp"

struct Machine {
  CPU cpu;
  Memory mem;
  
  int doit(Inst inst) {
    if (inst == MAGIC) {
      printf("finish\n");
      return 1;
    }
    printf("inst = 0x%08x\n", inst);
    return 0;
  }

  int execute_one_step() {
    Inst inst = this->mem.read_vmem(cpu.pc);
    int r = this->doit(inst);
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
