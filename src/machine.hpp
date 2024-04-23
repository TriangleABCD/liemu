#pragma once
#ifndef MACHINE_H
#define MACHINE_H

#include "CPU.hpp"
#include "memory.hpp"

struct Machine {
  CPU cpu;
  Memory mem;

  int execute_one_step() {
    if (cpu.pc < START_ADDR) {
      return 1;
    }
    Inst inst = this->mem.read_vmem(cpu.pc);
    int r = this->parse_inst(inst);
    this->cpu.pc += 4;
    if (-1 == r) {
      fprintf(stderr, "\033[31mexec error\033[0m\n");
      return r;
    }
    return r != 0;
  }

  int execute() {
    int r = 0;
    while (1) {
      r = execute_one_step();
      if (-1 == r || 1 == r) {
        break;
      }
    }
    return r;
  }
  int parse_inst(Inst inst) {
    int opcode = inst & 0x7f;
    switch(opcode) {
      case 0x03: {
        int funct3 = (inst >> 12) & 0x7;
        switch(funct3) {
          case 0b000: {
            printf("lb ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            i16 imm = ((i32)inst >> 20);
            printf("0x%x(%s)\n", (i16)imm, cpu.reg_names[rs1].c_str());
            u32 addr = (i32)this->cpu.gp_regs[rs1] + imm;
            u8 op = addr % 4;
            if (0 == op) {
              i8 hw = this->mem.read_vmem(addr);
              this->mem.write_vmem(addr, i32(hw));
            } else if (2 == op) {
              i8 hw = this->mem.read_vmem(addr) >> 16;
              this->mem.write_vmem(addr, i32(hw));
            } else if (1 == op) {
              i8 hw = this->mem.read_vmem(addr) >> 8;
              this->mem.write_vmem(addr, i32(hw));
            } else {
              i8 hw = this->mem.read_vmem(addr) >> 24;
              this->mem.write_vmem(addr, i32(hw));
            }
            break;
          }
          case 0b001: {
            printf("lh ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            i16 imm = ((i32)inst >> 20);
            printf("0x%x(%s)\n", (i16)imm, cpu.reg_names[rs1].c_str());
            u32 addr = (i32)this->cpu.gp_regs[rs1] + imm;
            u8 op = addr % 4;
            if (0 == op) {
              i16 hw = this->mem.read_vmem(addr);
              this->mem.write_vmem(addr, i32(hw));
            } else if (2 == op) {
              i16 hw = this->mem.read_vmem(addr) >> 16;
              this->mem.write_vmem(addr, i32(hw));
            } else if (1 == op) {
              i16 hw = this->mem.read_vmem(addr) >> 8;
              this->mem.write_vmem(addr, i32(hw));
            } else {
              i8 hw1 = this->mem.read_vmem(addr) >> 24;
              i8 hw2 = this->mem.read_vmem(addr+1);
              i16 hw = (hw2 << 8) | hw1;
              this->mem.write_vmem(addr, i32(hw));
            }
            break;
          }
          case 0b010: {
            printf("lw ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            i16 imm = ((i32)inst >> 20);
            printf("%d(%s)\n", (i16)imm, cpu.reg_names[rs1].c_str());
            u32 addr = (i32)this->cpu.gp_regs[rs1] + imm;
            this->cpu.gp_regs[rd] = this->mem.read_vmem(addr);
            break;
          }
          case 0b100: {
            printf("lbu\n");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            i16 imm = ((i32)inst >> 20);
            printf("0x%x(%s)\n", (i16)imm, cpu.reg_names[rs1].c_str());
            u32 addr = (i32)this->cpu.gp_regs[rs1] + imm;
            u8 op = addr % 4;
            if (0 == op) {
              u8 hw = this->mem.read_vmem(addr);
              this->mem.write_vmem(addr, u32(hw));
            } else if (2 == op) {
              u8 hw = this->mem.read_vmem(addr) >> 16;
              this->mem.write_vmem(addr, u32(hw));
            } else if (1 == op) {
              u8 hw = this->mem.read_vmem(addr) >> 8;
              this->mem.write_vmem(addr, u32(hw));
            } else {
              u8 hw = this->mem.read_vmem(addr) >> 24;
              this->mem.write_vmem(addr, u32(hw));
            }
            break;
          }
          case 0b101: {
            printf("lhu\n");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            i16 imm = ((i32)inst >> 20);
            printf("0x%x(%s)\n", (i16)imm, cpu.reg_names[rs1].c_str());
            u32 addr = (i32)this->cpu.gp_regs[rs1] + imm;
            u8 op = addr % 4;
            if (0 == op) {
              u16 hw = this->mem.read_vmem(addr);
              this->mem.write_vmem(addr, u32(hw));
            } else if (2 == op) {
              u16 hw = this->mem.read_vmem(addr) >> 16;
              this->mem.write_vmem(addr, u32(hw));
            } else if (1 == op) {
              u16 hw = this->mem.read_vmem(addr) >> 8;
              this->mem.write_vmem(addr, u32(hw));
            } else {
              u8 hw1 = this->mem.read_vmem(addr) >> 24;
              u8 hw2 = this->mem.read_vmem(addr+1);
              u16 hw = (hw2 << 8) | hw1;
              this->mem.write_vmem(addr, u32(hw));
            }
            break;
          }
          default:
            return -1;
        }
        break;
      }
      case 0x0f: {
        int funct3 = (inst >> 12) & 0x7;
        switch(funct3) {
          case 0b000:
            printf("(skip)fense\n");
            break;
          case 0b001:
            printf("(skip)fense.i\n");
            break;
          default:
            return -1;
        }
        break;
      }
      case 0x13: {
        int funct3 = (inst >> 12) & 0x7;
        switch(funct3) {
          case 0b000: {
            printf("addi ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            printf("%s, ", cpu.reg_names[rs1].c_str());
            i16 imm = ((i32)inst >> 20);
            printf("%d\n", imm);
            this->cpu.gp_regs[rd] = this->cpu.gp_regs[rs1] + imm;
            break;
          }
          case 0b010: {
            printf("slti ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            printf("%s, ", cpu.reg_names[rs1].c_str());
            i16 imm = ((i32)inst >> 20);
            printf("%d\n", imm);
            i32 val = this->cpu.gp_regs[rs1];
            if (val < imm) {
              this->cpu.gp_regs[rd] = 1;
            } else {
              this->cpu.gp_regs[rd] = 0;
            }
            break;
          }
          case 0b011: {
            printf("sltiu ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            printf("%s, ", cpu.reg_names[rs1].c_str());
            u16 imm = (inst >> 20);
            printf("%d\n", imm);
            u32 val = this->cpu.gp_regs[rs1];
            if (val < imm) {
              this->cpu.gp_regs[rd] = 1;
            } else {
              this->cpu.gp_regs[rd] = 0;
            }
            break;
          }
          case 0b100: {
            printf("xori ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            printf("%s, ", cpu.reg_names[rs1].c_str());
            i16 imm = ((i32)inst >> 20);
            printf("%d\n", imm);
            this->cpu.gp_regs[rd] = this->cpu.gp_regs[rs1] ^ imm;
            break;
          }
          case 0b110: {
            printf("ori ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            printf("%s, ", cpu.reg_names[rs1].c_str());
            i16 imm = ((i32)inst >> 20);
            printf("%d\n", imm);
            this->cpu.gp_regs[rd] = this->cpu.gp_regs[rs1] | imm;
            break;
          }
          case 0b111: {
            printf("andi ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            printf("%s, ", cpu.reg_names[rs1].c_str());
            i16 imm = ((i32)inst >> 20);
            printf("%d\n", imm);
            this->cpu.gp_regs[rd] = this->cpu.gp_regs[rs1] & imm;
            break;
          }
          case 0b001: {
            printf("slli ");
            u8 rd = (inst >> 7) & 0x1f;
            printf("%s, ", cpu.reg_names[rd].c_str());
            u8 rs1 = (inst >> 15) & 0x1f;
            printf("%s, ", cpu.reg_names[rs1].c_str());
            u8 shamt = (inst >> 20) & 0x1f;
            printf("%d\n", shamt);
            this->cpu.gp_regs[rd] = this->cpu.gp_regs[rs1] << shamt;
            break;
          }
          case 0b101:
            int op = (inst >> 31);
            if (op) {
              printf("srai ");
              u8 rd = (inst >> 7) & 0x1f;
              printf("%s, ", cpu.reg_names[rd].c_str());
              u8 rs1 = (inst >> 15) & 0x1f;
              printf("%s, ", cpu.reg_names[rs1].c_str());
              u8 shamt = (inst >> 20) & 0x1f;
              printf("%d\n", shamt);
              this->cpu.gp_regs[rd] = (i32)this->cpu.gp_regs[rs1] >> shamt;
            } else {
              printf("srli ");
              u8 rd = (inst >> 7) & 0x1f;
              printf("%s, ", cpu.reg_names[rd].c_str());
              u8 rs1 = (inst >> 15) & 0x1f;
              printf("%s, ", cpu.reg_names[rs1].c_str());
              u8 shamt = (inst >> 20) & 0x1f;
              printf("%d\n", shamt);
              this->cpu.gp_regs[rd] = this->cpu.gp_regs[rs1] >> shamt;
            }
            break;
        }
        break;
      }
      case 0x17: {
        printf("auipc");
        u8 rd = (inst >> 7) & 0x1f;
        u32 imm = (inst >> 12) << 12;
        printf(" %s, 0x%x\n", cpu.reg_names[rd].c_str(), imm);
        this->cpu.gp_regs[rd] = this->cpu.pc + imm;
        break;
      }
      case 0x23: {
        int funct3 = (inst >> 12) & 0x7;
        switch(funct3) {
          case 0b000:
            printf("sb\n");
            break;
          case 0b001:
            printf("sh\n");
            break;
          case 0b010:
            printf("sw\n");
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
};

#endif
