#pragma once
#ifndef CPU_H
#define CPU_H

#include <vector>
#include <string>
#include <cstdio>
#include <algorithm>

#include "common.hpp"

struct CPU {
  u32 gp_regs[32];
  u32 pc;

  std::vector<std::string> reg_names {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"  
  };
  
  int reg2idx(std::string reg_name) {
    auto it = std::find(this->reg_names.begin(), this->reg_names.end(), reg_name);
    if (it == this->reg_names.end()) {
      return -1;
    }
    return std::distance(this->reg_names.begin(), it);
  }

  void write_reg(std::string reg, u32 val) {
    int idx = this->reg2idx(reg);
    if (idx == -1) {
      fprintf(stderr, RED("wrong reg name\n"));
      return;
    }
    this->gp_regs[idx] = val;
    this->gp_regs[0] = 0;
  }

  void write_reg(int idx, u32 val) {
    if (idx < 0 || idx >= 32) {
      fprintf(stderr, RED("wrong reg idx\n"));
      return;
    }
    this->gp_regs[idx] = val;
    this->gp_regs[0] = 0;
  }

  void resetCPU(u32 _start = CODE_START, u32 _end = CODE_END) {
    this->write_reg("$0", 0);
    for (int i = 1; i < 32; i++) {
      this->gp_regs[i] = MAGIC;
    }
    this->pc = _start;
    this->gp_regs[this->reg2idx("sp")] = STACK_BTM;
    this->gp_regs[this->reg2idx("ra")] = _end; 
  }

  void info_reg(std::string reg = "") {
    if ("pc" == reg) {
      printf(GREEN("%s")":\t0x%08x\n", reg.c_str(), this->pc);
      return;
    }
    if ("" != reg) {
      int idx = this->reg2idx(reg);
      if (idx == -1) {
        fprintf(stderr, RED("wrong reg name\n"));
        return;
      }
      printf(GREEN("%s")":\t0x%08x\n", reg.c_str(), this->gp_regs[idx]);
      return;
    }
    int ln = 0;
    for (int i = 0; i < 32; i++) {
      auto& name = this->reg_names[i];
      printf(GREEN("%s")":\t0x%08x", name.c_str(), this->gp_regs[i]);
      ln++;
      if (ln % 2 == 0) {
        printf("\n");
      } else {
        printf("\t");
      }
    }
    printf(GREEN("pc")":\t0x%08x\n", this->pc);
  }
};

#endif
