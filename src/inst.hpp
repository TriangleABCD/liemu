#pragma once
#ifndef INST_H
#define INST_H

#include <cstdio>

#include "liemu.hpp"

struct Machine;

extern Machine m;



inline int parse_inst(Inst inst) {
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
          printf("slti\n");
          break;
        case 0b011:
          printf("sltiu\n");
          break;
        case 0b100:
          printf("xori\n");
          break;
        case 0b110:
          printf("ori\n");
          break;
        case 0b111:
          printf("andi\n");
          break;
        case 0b001:
          printf("slli\n");
          break;
        case 0b101:
          int op = (inst >> 31);
          if (op) {
            printf("srai\n");
          } else {
            printf("srli\n");
          }
          break;
      } 
      break;
    }
    case 0x17:
      printf("auipc\n"); 
      break;
    case 0x23: {
      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000:
          printf("sb");
          break;
        case 0b001:
          printf("sh");
          break;
        case 0b010:
          printf("sw");
          break;
        default:
          return -1;
      }
      break;
    }
    case 0x33: {
      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          int op = (inst >> 31);
          if (op) {
            printf("sub\n");
          } else {
            printf("add\n");
          }
          break;
        }
        case 0b001:
          printf("sll\n");
          break;
        case 0b010:
          printf("slt\n");
          break;
        case 0b011:
          printf("sltu\n");
          break;
        case 0b100:
          printf("xor\n");
          break;
        case 0b101: {
          int op = (inst >> 31);
          if (op) {
            printf("sra\n");
          } else {
            printf("srl\n");
          }
          break;
        }
        case 0b110:
          printf("or\n");
          break;
        case 0b111:
          printf("and\n");
          break;
        default:
          return -1;
      }
      break;
    }
    case 0x37:
      printf("lui\n"); 
      break;
    case 0x63: {
      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000:
          printf("beq\n");
          break;
        case 0b001:
          printf("bne\n");
          break;
        case 0b100:
          printf("blt\n");
          break;
        case 0b101:
          printf("bge\n");
          break;
        case 0b110:
          printf("bltu\n");
          break;
        case 0b111:
          printf("bgeu\n");
          break;
        default:
          return -1;
      }
      break;
    }
    case 0x67:
      printf("jalr\n"); 
      break;
    case 0x6f:
      printf("jal\n");
      break;
    case 0x73: {
      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          int op = (inst >> 20);
          if (op) {
            printf("ebreak\n");
          } else {
            printf("ecall\n");
          }
          break;
        }
        case 0b001:
          printf("csrrw");
          break;
        case 0b010:
          printf("csrrs");
          break;
        case 0b011:
          printf("csrrc");
          break;
        case 0b101:
          printf("csrrwi");
          break;
        case 0b110:
          printf("csrrsi");
          break;
        case 0b111:
          printf("csrrci");
          break;
      }
      break;
    }
    default:
      return -1;
  }
  return 0;
}

#endif
