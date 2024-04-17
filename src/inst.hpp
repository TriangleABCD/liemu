#pragma once
#ifndef INST_H
#define INST_H

#include <cstdio>

#include "liemu.hpp"

inline int parse_inst(Inst inst) {
  if (inst == MAGIC) {
    return 1;
  }
  printf("inst = 0x%08x\n", inst);
  return 0;
}

#endif
