#pragma once
#ifndef INST_HPP
#define INST_HPP 

#include <string>
#include <vector>
#include <functional>

#include "common.hpp"
#include "CPU.hpp"
#include "memory.hpp"

struct Inst {
  int result = -1;
  std::string name = "nop";
  std::function<int(const Inst&, CPU& cpu, Memory& mem)> doit;
  
  u8 rd, rs1, rs2, shamt;
  i16 offset;
  i32 imm;
};

struct Syscall {
  int num;
  std::string name;
  std::function<int(CPU& cpu, Memory& mem)> doit;
};

inline std::vector<Syscall> syscalls {
  { 1, "sys_print_reg", [](CPU& cpu, Memory& mem) -> int {
    int val = cpu.gp_regs[cpu.reg2idx("a0")];
    printf("%d\n", val);
    return 0;
  } },
};

inline Inst parse_inst(u32 inst, CPU& cpu, Memory& mem) {
  Inst res;
  res.result = 0;
  char buf[64];

  int opcode = inst & 0x7f;
  switch(opcode) {
    case 0x03: {
      u8 rd = (inst >> 7) & 0x1f;
      u8 rs1 = (inst >> 15) & 0x1f;
      i16 imm = ((i32)inst >> 20);

      res.rd = rd;
      res.rs1 = rs1;
      res.imm = imm;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {


        case 0b000: {
          res.name = "lb ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, "0x%x(%s)", (i16)imm, cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            u32 addr = (i32)cpu.gp_regs[inst.rs1] + inst.imm;
            u8 op = addr % 4;
            i8 byte = 0;
            if (0 == op) {
              byte = mem.read_vmem(addr);
            } else if (2 == op) {
              byte = mem.read_vmem(addr) >> 16;
            } else if (1 == op) {
              byte = mem.read_vmem(addr) >> 8;
            } else {
              byte = mem.read_vmem(addr) >> 24;
            }
            cpu.write_reg(inst.rd, i32(byte));
            return 0;
          };

          break;
        }
        case 0b001: {
          res.name = "lh ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, "0x%x(%s)", (i16)imm, cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            u32 addr = (i32)cpu.gp_regs[inst.rs1] + inst.imm;
            u8 op = addr % 4;
            i16 hw = 0;
            if (0 == op) {
              hw = mem.read_vmem(addr);
            } else if (2 == op) {
              hw = mem.read_vmem(addr) >> 16;
            } else if (1 == op) {
              hw = mem.read_vmem(addr) >> 8;
            } else {
              i8 hw1 = mem.read_vmem(addr) >> 24;
              i8 hw2 = mem.read_vmem(addr+1);
              hw = (hw2 << 8) | hw1;
            }
            cpu.write_reg(inst.rd, i32(hw));
            return 0;
          };

          break;
        }
        case 0b010: {
          res.name = "lw ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, "%d(%s)", (i16)imm, cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            u32 addr = (i32)cpu.gp_regs[inst.rs1] + inst.imm;
            cpu.write_reg(inst.rd, mem.read_vmem(addr));
            return 0;
          };

          break;
        }
        case 0b100: {
          res.name = "lbu ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, "0x%x(%s)", (i16)imm, cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            u32 addr = (i32)cpu.gp_regs[inst.rs1] + inst.imm;
            u8 op = addr % 4;
            u8 byte = 0;
            if (0 == op) {
              byte = mem.read_vmem(addr);
            } else if (2 == op) {
              byte = mem.read_vmem(addr) >> 16;
            } else if (1 == op) {
              byte = mem.read_vmem(addr) >> 8;
            } else {
              byte = mem.read_vmem(addr) >> 24;
            }
            cpu.write_reg(inst.rd, u32(byte));
            return 0;
          };

          break;
        }
        case 0b101: {
          res.name = "lhu ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, "0x%x(%s)", (i16)imm, cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);
          
          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            u32 addr = (i32)cpu.gp_regs[inst.rs1] + inst.imm;
            u8 op = addr % 4;
            u16 hw = 0;
            if (0 == op) {
              hw = mem.read_vmem(addr);
            } else if (2 == op) {
              hw = mem.read_vmem(addr) >> 16;
            } else if (1 == op) {
              hw = mem.read_vmem(addr) >> 8;
            } else {
              u8 hw1 = mem.read_vmem(addr) >> 24;
              u8 hw2 = mem.read_vmem(addr+1);
              hw = (hw2 << 8) | hw1;
            }
            cpu.write_reg(inst.rd, u32(hw));
            return 0;
          };

          break;
        }
        default:
          res.result = -1;
          return res;
      }
      break;
    }
    case 0x0f: {
      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000:
          res.name = "fence (skip)";
          break;
        case 0b001:
          res.name = "fence.i (skip)";
          break;
        default:
          res.result = -1;
          return res;
      }
      break;
    }
    case 0x13: {
      
      u8 rd = (inst >> 7) & 0x1f;
      u8 rs1 = (inst >> 15) & 0x1f;
      i16 imm = ((i32)inst >> 20);

      res.rd = rd;
      res.rs1 = rs1;
      res.imm = imm;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          res.name = "addi ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, (i32)cpu.gp_regs[inst.rs1] + inst.imm);
            return 0;
          };

          break;
        }
        case 0b010: {
          res.name = "slti ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            i32 val = cpu.gp_regs[inst.rs1];
            if (val < inst.imm) {
              cpu.write_reg(inst.rd, 1);
            } else {
              cpu.write_reg(inst.rd, 0);
            }
            return 0;
          };

          break;
        }
        case 0b011: {
          res.name = "sltiu ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            u32 val = cpu.gp_regs[inst.rs1];
            if (val < (u32)inst.imm) {
              cpu.write_reg(inst.rd, 1);
            } else {
              cpu.write_reg(inst.rd, 0);
            }
            return 0;
          };

          break;
        }
        case 0b100: {
          res.name = "xori ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] ^ inst.imm);
            return 0;
          };

          break;
        }
        case 0b110: {
          res.name = "ori ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] | inst.imm);
            return 0;
          };

          break;
        }
        case 0b111: {
          res.name = "andi ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] & inst.imm);
            return 0;
          };

          break;
        }
        case 0b001: {
          u8 shamt = (inst >> 20) & 0x1f;
          res.shamt = shamt;
          res.name = "slli ";
          res.name += cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", cpu.reg_names[rs1].c_str(), shamt);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] << inst.shamt);
            return 0;
          };

          break;
        }
        case 0b101: {
          int op = (inst >> 31);
          u8 shamt = (inst >> 20) & 0x1f;
          res.shamt = shamt;
          if (op) {
            res.name = "srai ";
            res.name += cpu.reg_names[rd];
            sprintf(buf, ", %s, %d", cpu.reg_names[rs1].c_str(), shamt);
            res.name += std::string(buf);

            res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] >> inst.shamt);
              return 0;
            };
          } else {
            res.name = "srli ";
            res.name += cpu.reg_names[rd];
            sprintf(buf, ", %s, %d", cpu.reg_names[rs1].c_str(), shamt);
            res.name += std::string(buf);

            res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, (u32)cpu.gp_regs[inst.rs1] >> inst.shamt);
              return 0;
            };
          }
          break;
        }
      }
      break;
    }
    case 0x17: {
      res.name = "auipc ";
      u8 rd = (inst >> 7) & 0x1f;
      i32 imm = ((i32)inst >> 12) << 12;
      res.rd = rd;
      res.imm = imm;
      res.name += cpu.reg_names[rd];
      sprintf(buf, ", 0x%x", imm);
      res.name += std::string(buf);
      
      res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
        cpu.write_reg(inst.rd, cpu.pc + inst.imm);
        return 0;
      };

      break;
    }
    case 0x23: {
      u8 rs1 = (inst >> 15) & 0x1f; 
      u8 rs2 = (inst >> 20) & 0x1f;
      i16 offset = (((i32)inst >> 25) <<  5) | (((i32)inst >> 7) & 0x1f);

      res.rs1 = rs1;
      res.rs2 = rs2;
      res.offset = offset;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          res.name = "sb ";
          res.name += cpu.reg_names[rs2];
          sprintf(buf, ", %d(%s)", offset, cpu.reg_names[rs1].c_str());
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            u32 addr = (i32)cpu.gp_regs[inst.rs1] + inst.offset;
            u8 byte = cpu.gp_regs[inst.rs2];
            u8 op = addr % 4;
            u32 val = mem.read_vmem(addr);
            if (0 == op) {
              val = (val & 0xffffff00) | byte;
            } else if (2 == op) {
              val = (val & 0xff00ffff) | (byte << 16);
            } else if (1 == op) {
              val = (val & 0xffff00ff) | (byte << 8);
            } else {
              val = (val & 0x00ffffff) | (byte << 24);
            }
            mem.write_vmem(addr, val);
            return 0;
          };

          break;
        }
        case 0b001: {
          res.name = "sh ";
          res.name += cpu.reg_names[rs2];
          sprintf(buf, ", %d(%s)", offset, cpu.reg_names[rs1].c_str());
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            u32 addr = (i32)cpu.gp_regs[inst.rs1] + inst.offset;
            u16 hw = cpu.gp_regs[inst.rs2];
            u8 op = addr % 4;
            u32 val = mem.read_vmem(addr);
            
            if (0 == op) {
              val = (val & 0xffff0000) | hw;
            } else if (2 == op) {
              val = (val & 0x0000ffff) | (hw << 16);
            } else if (1 == op) {
              val = (val & 0xff0000ff) | (hw << 8);
            } else {
              u32 val1 = mem.read_vmem(addr+1);
              val1 = (val1 & 0xffffff00) | (hw >> 8);
              mem.write_vmem(addr+1, val1);
              val = (val & 0x00ffffff) | (hw << 24);
            }
            mem.write_vmem(addr, val);
            return 0;
          };

          break;
        }
        case 0b010: {
          res.name = "sw ";
          res.name += cpu.reg_names[rs2];
          sprintf(buf, ", %d(%s)", offset, cpu.reg_names[rs1].c_str());
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            i32 addr = (i32)cpu.gp_regs[inst.rs1] + inst.offset;
            u32 val = cpu.gp_regs[inst.rs2];
            mem.write_vmem(addr, val);
            return 0;
          };

          break;
        }
        default:
          res.result = -1;
          return res;
      }
      break;
    }
    case 0x33: {
      u8 rd = (inst >> 7) & 0x1f;
      u8 rs1 = (inst >> 15) & 0x1f;
      u8 rs2 = (inst >> 20) & 0x1f;

      res.rd = rd;
      res.rs1 = rs1;
      res.rs2 = rs2;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          int op = (inst >> 31);
          if (op) {
            res.name = "sub ";
            res.name += cpu.reg_names[rd];
            res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
              cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] - cpu.gp_regs[inst.rs2]);
              return 0;
            };
          } else {
            res.name = "add ";
            res.name += cpu.reg_names[rd];
            res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
              cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] + cpu.gp_regs[inst.rs2]);
              return 0;
            };
          }
          break;
        }
        case 0b001: {
          res.name = "sll ";
          res.name += cpu.reg_names[rd];
          res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] << (cpu.gp_regs[inst.rs2] & 0x1f));
            return 0;
          };
          break;
        }
        case 0b010: {
          res.name = "slt ";
          res.name += cpu.reg_names[rd];
          res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            i32 val1 = cpu.gp_regs[inst.rs1];
            i32 val2 = cpu.gp_regs[inst.rs2];
            if (val1 < val2) {
              cpu.write_reg(inst.rd, 1);
            } else {
              cpu.write_reg(inst.rd, 0);
            }
            return 0;
          };
          break;
        }
        case 0b011: {
          res.name = "sltu ";
          res.name += cpu.reg_names[rd];
          res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            u32 val1 = cpu.gp_regs[inst.rs1];
            u32 val2 = cpu.gp_regs[inst.rs2];
            if (val1 < val2) {
              cpu.write_reg(inst.rd, 1);
            } else {
              cpu.write_reg(inst.rd, 0);
            }
            return 0;
          };
          break;
        }
        case 0b100: {
          res.name = "xor ";
          res.name += cpu.reg_names[rd];
          res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] ^ cpu.gp_regs[inst.rs2]);
            return 0;
          };
          break;
        }
        case 0b101: {
          int op = (inst >> 31);
          if (op) {
            res.name = "sra ";
            res.name += cpu.reg_names[rd];
            res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
              cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] >> (cpu.gp_regs[inst.rs2] & 0x1f));
              return 0;
            };
          } else {
            res.name = "srl ";
            res.name += cpu.reg_names[rd];
            res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
              cpu.write_reg(inst.rd, (u32)cpu.gp_regs[inst.rs1] >> (cpu.gp_regs[inst.rs2] & 0x1f));
              return 0;
            };
          }
          break;
        }
        case 0b110: {
          res.name = "or ";
          res.name += cpu.reg_names[rd];
          res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] | cpu.gp_regs[inst.rs2]);
            return 0;
          };
          break;
        }
        case 0b111: {
          res.name = "and ";
          res.name += cpu.reg_names[rd];
          res.name += ", " + cpu.reg_names[rs1] + ", " + cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            cpu.write_reg(inst.rd, cpu.gp_regs[inst.rs1] & cpu.gp_regs[inst.rs2]);
            return 0;
          };
          break;
        }
        default:
          res.result = -1;
          return res;
      }
      break;
    }
    case 0x37: {
      u8 rd = (inst >> 7) & 0x1f;
      i32 imm = ((i32)inst >> 12) << 12;

      res.rd = rd;
      res.imm = imm;

      res.name = "lui ";
      res.name += cpu.reg_names[rd];
      sprintf(buf, ", 0x%x", imm);
      res.name += std::string(buf);

      res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
        cpu.write_reg(inst.rd, inst.imm);
        return 0;
      };
      break;
    }
    case 0x63: {
      u8 rs1 = (inst >> 15) & 0x1f;
      u8 rs2 = (inst >> 20) & 0x1f;

      i32 offset = (((i32)inst >> 31) << 11) 
                |  ((((i32)inst >> 7) & 1) << 10)
                |  ((((i32)inst >> 25) & 0x3f) << 4)
                |  (((i32)inst >> 8) & 0xf);
      offset = offset << 1;

      res.rs1 = rs1;
      res.rs2 = rs2;
      res.offset = offset;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          res.name = "beq ";
          res.name += cpu.reg_names[rs1];
          res.name += ", " + cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            if (cpu.gp_regs[inst.rs1] == cpu.gp_regs[inst.rs2]) {
              cpu.pc += inst.offset;
            }
            return 0;
          };
          break;
        }
        case 0b001: {
          res.name = "bne ";
          res.name += cpu.reg_names[rs1];
          res.name += ", " + cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            if (cpu.gp_regs[inst.rs1] != cpu.gp_regs[inst.rs2]) {
              cpu.pc += inst.offset;
            }
            return 0;
          };
          break;
        }
        case 0b100: {
          res.name = "blt ";
          res.name += cpu.reg_names[rs1];
          res.name += ", " + cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            if ((i32)cpu.gp_regs[inst.rs1] < (i32)cpu.gp_regs[inst.rs2]) {
              cpu.pc += inst.offset;
            }
            return 0;
          };
          break;
        }
        case 0b101: {
          res.name = "bge ";
          res.name += cpu.reg_names[rs1];
          res.name += ", " + cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            if ((i32)cpu.gp_regs[inst.rs1] >= (i32)cpu.gp_regs[inst.rs2]) {
              cpu.pc += inst.offset;
            }
            return 0;
          };
          break;
        }
        case 0b110: {
          res.name = "bltu ";
          res.name += cpu.reg_names[rs1];
          res.name += ", " + cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            if ((u32)cpu.gp_regs[inst.rs1] < (u32)cpu.gp_regs[inst.rs2]) {
              cpu.pc += inst.offset;
            }
            return 0;
          };
          break;
        }
        case 0b111: {
          res.name = "bgeu ";
          res.name += cpu.reg_names[rs1];
          res.name += ", " + cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
            if ((u32)cpu.gp_regs[inst.rs1] >= (u32)cpu.gp_regs[inst.rs2]) {
              cpu.pc += inst.offset;
            }
            return 0;
          };
          break;
        }
        default:
          res.result = -1;
          return res;
      }
      break;
    }
    case 0x67: {
      u8 rd = (inst >> 7) & 0x1f;
      u8 rs1 = (inst >> 15) & 0x1f;
      i16 offset = ((i32)inst >> 20);

      res.rd = rd;
      res.rs1 = rs1;
      res.offset = offset;

      res.name = "jalr ";
      res.name += cpu.reg_names[rd];
      sprintf(buf, ", %d(%s)", offset, cpu.reg_names[rs1].c_str());
      res.name += std::string(buf);

      res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
        u32 tmp = cpu.pc + 4;
        cpu.pc = ((i32)cpu.gp_regs[inst.rs1] + inst.offset) & 0xfffffffe;
        cpu.write_reg(inst.rd, tmp);
        return 0;
      };

      break;
    }
    case 0x6f: {
      u8 rd = (inst >> 7) & 0x1f;
      i32 offset = (((i32)inst >> 31) << 19) 
                |  ((((i32)inst >> 12) & 0xff) << 11)
                |  ((((i32)inst >> 20) & 1) << 10)
                |  (((i32)inst >> 21) & 0x3ff);
      offset = offset << 1;
      res.rd = rd;
      res.offset = offset;

      res.name = "jal ";
      res.name += cpu.reg_names[rd];
      sprintf(buf, ", 0x%x", offset);
      res.name += std::string(buf);

      res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
        cpu.write_reg(inst.rd, cpu.pc + 4);
        cpu.pc += inst.offset;
        return 0;
      };

      break;
    }
    case 0x73: {
      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          int op = (inst >> 20);
          if (op) {
            res.name = "ebreak (skip)";
          } else {
            res.name = "ecall";
            res.doit = [](const Inst& inst, CPU& cpu, Memory& mem) {
              u8 syscall_num = cpu.gp_regs[cpu.reg2idx("a7")];
              int r = -1;
              for (auto& syscall : syscalls) {
                if (syscall.num == syscall_num) {
                  r = syscall.doit(cpu, mem);
                  break;
                }
              }
              return r;
            };
          }
          break;
        }
        case 0b001:
          res.name = "csrrw (skip)";
          break;
        case 0b010:
          res.name = "csrrs (skip)";
          break;
        case 0b011:
          res.name = "csrrc (skip)";
          break;
        case 0b101:
          res.name = "csrrwi (skip)";
          break;
        case 0b110:
          res.name = "csrrsi (skip)";
          break;
        case 0b111:
          res.name = "csrrci (skip)";
          break;
      }
      break;
    }
    default:
      res.result = -1;
      return res;
  }
  return res;
}
#endif
