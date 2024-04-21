#pragma once
#ifndef CPU_H
#define CPU_H


#include <vector>
#include <string>
#include <cstdio>
#include <algorithm>

#include "liemu.hpp"

struct CPU {
  u32 gp_regs[32];
  u32 pc;
  
  std::vector<std::string> reg_names {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
  };

  CPU() {
    this->gp_regs[0] = 0;
    for (int i = 1; i < 32; i++) {
      this->gp_regs[i] = MAGIC;
    }
    this->pc = START_ADDR;
  }
  
  int reg2idx(std::string reg_name) {
    auto it = std::find(this->reg_names.begin(), this->reg_names.end(), reg_name);
    if (it == this->reg_names.end()) {
      return -1;
    }
    return std::distance(this->reg_names.begin(), it);
  }

  void info_reg(std::string reg = "") {
    if ("pc" == reg) {
      printf("\033[32m%s\033[0m:\t0x%08x\n", reg.c_str(), this->pc);
      return;
    }
    if ("" != reg) {
      int idx = this->reg2idx(reg);
      if (idx == -1) {
        fprintf(stderr, "\033[31mwrong reg name\n");
        return;
      }
      printf("\033[32m%s\033[0m:\t0x%08x\n", reg.c_str(), this->gp_regs[idx]);
      return;
    }
    for (int i = 0; i < 32; i++) {
      auto& name = this->reg_names[i];
      printf("\033[32m%s\033[0m:\t0x%08x\n", name.c_str(), this->gp_regs[i]);
    }
    printf("\033[32mpc\033[0m:\t0x%08x\n", this->pc);
  }

};

#endif
