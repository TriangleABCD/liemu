#pragma once
#ifndef WATCH_H
#define WATCH_H

#include "common.hpp"
#include "CPU.hpp"
#include "Memory.hpp"

struct WatchPoint {
  int id;
  enum Type {
    REG,
    MEM
  } type;
  u32 addr;
  int reg_idx;
  u32 val, old_val;
  
  bool update_val(CPU& cpu, Memory& mem) {
    if (self.type == REG) {
      self.val = cpu.gp_regs[self.reg_idx];
    } else {
      self.val = mem.read(self.addr);
    }
    if (self.val != self.old_val) {
      show_watchpoint_value(cpu, mem);
      self.old_val = self.val;
      return true;
    }
    return false;
  }

  void show_watchpoint_value(const CPU& cpu, const Memory& mem) {
    if (self.type == REG) {
      printf("watchpoint %d: %s = 0x%08x\n", self.id, cpu.reg_names[self.reg_idx].c_str(), self.val);
    } else {
      printf("watchpoint %d: 0x%08x = 0x%08x\n", self.id, self.addr, self.val);
    }
  }

  void info_watchpoint(const CPU& cpu, const Memory& mem) {
    if (self.type == REG) {
      printf("%d\treg\t%s\n", self.id, cpu.reg_names[self.reg_idx].c_str());
    } else {
      printf("%d\tmem\t0x%08x\n", self.id, self.addr);
    }
  }
};

struct WatchList {
  std::vector<WatchPoint> watch_list;

  void info_watchpoint(const CPU& cpu, const Memory& mem) {
    printf("id\ttype\tvalue\n");
    for (auto& wp : self.watch_list) {
      wp.info_watchpoint(cpu, mem);
    }
  }

  bool update_watchpoint(CPU& cpu, Memory& mem) {
    bool r = false;
    for (auto& wp : self.watch_list) {
      r = wp.update_val(cpu, mem) || r;
    }
    return r;
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
      wp.val = mem.read(wp.addr);
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
    self.watch_list.push_back(wp);
    std::sort(self.watch_list.begin(), self.watch_list.end(), [](const WatchPoint& a, const WatchPoint& b) {
      return a.id < b.id;
    });
  }

  void del_watchpoint(int id) {
    for (auto it = self.watch_list.begin(); it != self.watch_list.end(); it++) {
      if (it->id == id) {
        self.watch_list.erase(it);
        return;
      }
    }
    fprintf(stderr, "\033[31mwatchpoint %d not found\033[0m\n", id);
  }
};

#endif
