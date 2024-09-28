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

  u32 csr[4];
  std::vector<std::string> csr_names {
    "mtvec", "mepc", "mcause", "mstatus"
  };
  u32& mtvec = csr[0];
  u32& mepc = csr[1];
  u32& mcause = csr[2];
  u32& mstatus = csr[3];

  std::vector<std::string> reg_names {
    "x0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"  
  };
  
  int reg2idx(std::string reg_name) {
    auto it = std::find(self.reg_names.begin(), self.reg_names.end(), reg_name);
    if (it == self.reg_names.end()) {
      auto it = std::find(self.csr_names.begin(), self.csr_names.end(), reg_name);
      if (it == self.csr_names.end()) {
        return -1;
      }
      return std::distance(self.csr_names.begin(), it);
    }
    return std::distance(self.reg_names.begin(), it);
  }

  void write_reg(std::string reg, u32 val) {
    int idx = self.reg2idx(reg);
    if (idx == -1) {
      fprintf(stderr, RED("wrong reg name\n"));
      return;
    }
    self.gp_regs[idx] = val;
    self.gp_regs[0] = 0;
  }

  void write_reg(int idx, u32 val) {
    if (idx < 0 || idx >= 32) {
      fprintf(stderr, RED("wrong reg idx\n"));
      return;
    }
    self.gp_regs[idx] = val;
    self.gp_regs[0] = 0;
  }

  void resetCPU(u32 _start = CODE_START, u32 _end = CODE_END) {
    self.write_reg("x0", 0);
    for (int i = 1; i < 32; i++) {
      self.gp_regs[i] = MAGIC;
    }
    self.pc = _start;
    self.gp_regs[self.reg2idx("sp")] = STACK_BTM;
    self.gp_regs[self.reg2idx("ra")] = _end;

    self.mtvec = 0;
    self.mepc = 0;
    self.mcause = 0;
    self.mstatus = 0;
  }

  void info_reg(std::string reg = "") {
    if ("pc" == reg) {
      printf(GREEN("%s")":\t0x%08x\n", reg.c_str(), self.pc);
      return;
    }
    for (auto& name: self.csr_names) {
      if (name == reg) {
        int idx = self.reg2idx(reg);
        if (idx == -1) {
          fprintf(stderr, RED("wrong reg name\n"));
          return;
        }
        printf(GREEN("%s")":\t0x%08x\n", reg.c_str(), self.csr[idx]);
        return;
      }
    }

    if ("" != reg) {
      int idx = self.reg2idx(reg);
      if (idx == -1) {
        fprintf(stderr, RED("wrong reg name\n"));
        return;
      }
      printf(GREEN("%s")":\t0x%08x\n", reg.c_str(), self.gp_regs[idx]);
      return;
    }
    int ln = 0;
    for (int i = 0; i < 32; i++) {
      auto& name = self.reg_names[i];
      printf(GREEN("%s")":\t0x%08x", name.c_str(), self.gp_regs[i]);
      ln++;
      if (ln % 2 == 0) {
        printf("\n");
      } else {
        printf("\t");
      }
    }
    printf(GREEN("pc")":\t0x%08x\n", self.pc);

    for (auto& name: self.csr_names) {
      int idx = self.reg2idx(name);
      if (idx == -1) {
        fprintf(stderr, RED("wrong reg name\n"));
        return;
      }
      printf(GREEN("%s")":\n0x%08x\n", name.c_str(), self.csr[idx]);
    }
  }
};

#endif
