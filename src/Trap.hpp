#pragma once
#ifndef TRAP_H
#define TRAP_H

#include <vector>

#include "Machine.hpp"

#define TRAP_MAX_NUM 4


inline std::vector<std::vector<int>> trap_priority = {
  {0, 1, 2, 3},
  {1, 0, 2, 3},
  {2, 1, 0, 3},
  {3, 2, 1, 0}
};


inline void initTrap(Machine& m) {
  std::vector<u32> trampoline_code = {

  };

  u32 addr = TRAMP_BTM;
  for (const auto& inst : trampoline_code) {
    m.memory.write(addr, inst);
    addr += sizeof(u32);
  }
}


inline void trap_enable() {

}


inline void trap_disable() {

}


inline bool trap_on() {
  return true;
}


inline void trap(Machine& m, int trap_num) {
  printf(RED("trap %d\n"), trap_num);
}

#endif
