#pragma once
#ifndef MACHINE_H
#define MACHINE_H

#include "CPU.hpp"
#include "memory.hpp"

struct Machine {
  CPU cpu;
  Memory mem;
};

#endif
