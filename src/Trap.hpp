#pragma once
#ifndef TRAP_H
#define TRAP_H

#include <vector>
#include <queue>

#include "Machine.hpp"

inline auto TrapCmp = [](const int& a, const int& b) {
  return !(trap_priority[a][b] == 1);
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


inline void trap_enable(Machine& m) {
  m.cpu.mstatus = m.cpu.mstatus | 0x8;
}


inline void trap_disable(Machine& m) {
  m.cpu.mstatus = m.cpu.mstatus & 0xfffffff7;
}


inline bool trap_on(Machine& m) {
  return (m.cpu.mstatus & 0x8);
}


inline void initTrap(Machine& m) {
  std::vector<u32> trampoline_code = {
    0x34206573,
    0x00100593,
    0x00b50e63,
    0x00200593,
    0x00b50a63,
    0x00300593,
    0x00b50663,
    0x00400593,
    0x00b50263,
    0x00100893,
    0x00000073,
    0x00000033,
    0x00000073,
    0x00000033,
    0x00000073,
    0x00000033,
    0x00000073,
    0x30200073,
  };

  u32 addr = TRAMPOLINE_BTM;
  for (const auto& inst : trampoline_code) {
    m.memory.write(addr, inst);
    addr += sizeof(u32);
  }
  m.cpu.mtvec = TRAMPOLINE_BTM;
  trap_enable(m);
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

  if (!TrapCmp(nxt, cur)) {

    m.cpu.gp_regs[2] -= 136;

    m.writeMem(m.cpu.gp_regs[2], m.cpu.mepc);
    m.writeMem(m.cpu.gp_regs[2] + 4, m.cpu.mcause);
    for (int i = 0; i < 32; i++) {
      u32 addr = m.cpu.gp_regs[2] + 8 + i * 4;
      m.writeMem(addr, m.cpu.gp_regs[i]);
    }
    m.cpu.mepc = m.cpu.pc;

    m.cpu.pc = m.cpu.mtvec - 4;

    m.cpu.mcause = nxt;

    trap_queue.pop();
  }
}


inline void trap_ret(Machine& m) {
  m.cpu.pc = m.cpu.mepc;
  for (int i = 0; i < 32; i++) {
    u32 addr = m.cpu.gp_regs[2] + 8 + i * 4;
    m.cpu.gp_regs[i] = m.readMem(addr);
  }
  m.cpu.mcause = m.readMem(m.cpu.gp_regs[2] + 4);
  m.cpu.mepc = m.readMem(m.cpu.gp_regs[2]);

  m.cpu.gp_regs[2] += 136;
}

#endif
