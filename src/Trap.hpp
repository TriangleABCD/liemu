#pragma once
#ifndef TRAP_H
#define TRAP_H

#include <vector>
#include <queue>

#include "Machine.hpp"

#define TRAP_MAX_NUM 4


inline std::vector<std::vector<int>> trap_priority = {
  {1, 1, 1, 1},
  {0, 1, 1, 1},
  {0, 0, 1, 1},
  {0, 0, 0, 1}
};

inline auto cmp = [](const int& a, const int& b) {
  return (trap_priority[a][b] == 1);
};

inline std::priority_queue<int, std::vector<int>, decltype(cmp)> trap_queue(cmp);

inline void show_trap_queue() {
  printf("trap queue: ");
  auto q = trap_queue;
  while (!q.empty()) {
    printf("%d ", q.top());
    q.pop();
  }
  printf("\n");
}


inline void initTrap(Machine& m) {
  std::vector<u32> trampoline_code = {

  };

  u32 addr = TRAMPOLINE_BTM;
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


inline void trap_comming(int trap_num) {
  printf(RED("trap %d comming !\n"), trap_num);
  trap_queue.push(trap_num);
}


inline void trap(Machine& m) {
}

#endif
