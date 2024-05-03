#pragma once
#ifndef MACHINE_H
#define MACHINE_H

#include <map>
#include <algorithm>

#include "CPU.hpp"
#include "memory.hpp"
#include "inst.hpp"

struct WatchPoint {
  int id;
  enum Type {
    REG,
    MEM
  } type;
  u32 addr;
  int reg_idx;
  u32 val, old_val;
  
  void update_val(CPU& cpu, Memory& mem) {
    if (this->type == REG) {
      this->val = cpu.gp_regs[this->reg_idx];
    } else {
      this->val = mem.read_vmem(this->addr);
    }
    if (this->val != this->old_val) {
      show_watchpoint(cpu, mem);
      this->old_val = this->val;
    }
  }

  void show_watchpoint(const CPU& cpu, const Memory& mem) {
    if (this->type == REG) {
      printf("watchpoint %d: %s = 0x%08x\n", this->id, cpu.reg_names[this->reg_idx].c_str(), this->val);
    } else {
      printf("watchpoint %d: 0x%08x = 0x%08x\n", this->id, this->addr, this->val);
    }
  }

  void info_watchpoint(const CPU& cpu, const Memory& mem) {
    if (this->type == REG) {
      printf("%d\treg\t%s\n", this->id, cpu.reg_names[this->reg_idx].c_str());
    } else {
      printf("%d\tmem\t0x%08x\n", this->id, this->addr);
    }
  }
};

struct WatchList {
  std::vector<WatchPoint> watch_list;

  void info_watchpoint(const CPU& cpu, const Memory& mem) {
    printf("id\ttype\tvalue\n");
    for (auto& wp : this->watch_list) {
      wp.info_watchpoint(cpu, mem);
    }
  }

  void update_watchpoint(CPU& cpu, Memory& mem) {
    for (auto& wp : this->watch_list) {
      wp.update_val(cpu, mem);
    }
  }
  
  void add_watchpoint(CPU& cpu, Memory& mem, std::string type, std::string w) {
    WatchPoint wp;
    if (type == "r") {
      wp.type = WatchPoint::REG;
      wp.reg_idx = cpu.reg2idx(w);
      wp.val = cpu.gp_regs[wp.reg_idx];
      wp.old_val = wp.val;
    } else {
      wp.type = WatchPoint::MEM;
      wp.addr = strtol(w.c_str()+2, NULL, 16);
      wp.val = mem.read_vmem(wp.addr);
      wp.old_val = wp.val;
    }
    int id = 0;
    for (auto &e : watch_list) {
      if (e.id == id) {
        id++;
      } else {
        break;
      }
    }
    wp.id = id;
    this->watch_list.push_back(wp);
    std::sort(this->watch_list.begin(), this->watch_list.end(), [](const WatchPoint& a, const WatchPoint& b) {
      return a.id < b.id;
    });
  }

  void del_watchpoint(int id) {
    for (auto it = this->watch_list.begin(); it != this->watch_list.end(); it++) {
      if (it->id == id) {
        this->watch_list.erase(it);
        return;
      }
    }
    fprintf(stderr, "\033[31mwatchpoint %d not found\033[0m\n", id);
  }
};

struct Machine {
  CPU cpu;
  Memory mem;
  std::map<u32, Inst> getInst;
  WatchList watchList;

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
    if (inst.result == -1) {
      fprintf(stderr, "\033[32mfinish\033[0m\n");
      return -1;
    }
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
