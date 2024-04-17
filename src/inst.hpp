#pragma once
#ifndef INST_H
#define INST_H

#include <cstdio>

#include "liemu.hpp"

struct Machine;

inline int parse_inst(Machine& m, Inst inst) {
  int opcode = inst & 0x7f;
  switch(opcode) {
    case 0x03: {
      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000:
          printf("lb\n"); 
          break;
        case 0b001:
          printf("lh\n"); 
          break;
        case 0b010:
          printf("lw\n"); 
          break;
        case 0b100:
          printf("lbu\n"); 
          break;
        case 0b101:
          printf("lhu\n"); 
          break;
        default:
          return -1;
      }
      break;
    }
    case 0x0f: {
      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000:
          printf("fense\n");
          break;
        case 0b001:
          printf("fense.i\n");
          break;
        default:
          return -1;
      }
      break;
    }
    case 0x13: {
      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000:
          printf("addi\n");
          break;
        case 0b010:
          printf("addi\n");
          break;
        case 0b011:
          printf("addi\n");
          break;
        case 0b100:
          printf("addi\n");
          break;
        case 0b110:
          printf("addi\n");
          break;
        case 0b111:
          printf("addi\n");
          break;
        case 0b001:
          printf("addi\n");
          break;
        case 0b101:
          printf("addi\n");
          break;
      } 
      break;
    }
    case 0x17:
    
      break;
    case 0x23:
    
      break;
    case 0x33:
    
      break;
    case 0x37:
    
      break;
    case 0x63:
    
      break;
    case 0x67:
    
      break;
    case 0x6f:
    
      break;
    case 0x73:
    
      break;
    default:
      return -1;
  }
  return 0;
}

#endif
