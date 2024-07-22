#pragma once
#ifndef INST_HPP
#define INST_HPP 

#include <string>
#include <vector>
#include <functional>

#include "common.hpp"
#include "CPU.hpp"
#include "Memory.hpp"


struct Inst {
  int result = -1;
  std::string name = "nop";

  std::function<
    int(const Inst&, CPU&, Memory&)
  > doit;

  struct {
    u8 rd, rs1, rs2, shamt;
    i16 offset;
    i32 imm;
  } preValue;
};

struct Syscall {
  int syscall_num;
  std::string syscall_name;
  std::function<
    int(CPU&, Memory&)
  > doit;
};

#endif
