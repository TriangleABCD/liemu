#pragma once
#ifndef TRAP_H
#define TRAP_H

#include <vector>
#include <queue>

#include "Machine.hpp"

#define TRAP_MAX_NUM 4

inline auto TrapCmp = [](const int& a, const int& b) {
  return (trap_priority[a][b] == 1);
};

inline std::priority_queue<int, std::vector<int>, decltype(TrapCmp)> trap_queue(TrapCmp);

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
    // todo
  };

  u32 addr = TRAMPOLINE_BTM;
  for (const auto& inst : trampoline_code) {
    m.memory.write(addr, inst);
    addr += sizeof(u32);
  }

  m.cpu.mtvec = TRAMPOLINE_BTM;
}


inline void trap_enable(Machine& m) {
  m.cpu.mstatus = m.cpu.mstatus | 0x8;
}


inline void trap_disable(Machine& m) {
  m.cpu.mstatus = m.cpu.mstatus & 0xfffffff7;
}


inline bool trap_on(Machine& m) {
  return (m.cpu.mstatus & 0x8) == 1;
}


inline void trap_comming(int trap_num) {
  printf(RED("trap %d comming !\n"), trap_num);
  trap_queue.push(trap_num);
}


inline void trap(Machine& m) {
  if (!trap_on(m)) {
    return;
  }

  if (trap_queue.empty()) {
    return;
  }

  int cur = m.cpu.mcause & 0x7fffffff;

  int nxt = trap_queue.top();

  if (TrapCmp(nxt, cur)) {
    // todo  
    trap_queue.pop();
  }
}

#endif
