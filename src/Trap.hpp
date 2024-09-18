#pragma once
#ifndef TRAP_H
#define TRAP_H

#include "Machine.hpp"

#define TRAP_MAX_NUM 4


inline void initTrap(Machine& m) {

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
