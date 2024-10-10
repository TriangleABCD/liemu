#pragma once
#ifndef INST_HPP
#define INST_HPP 

#include <string>
#include <vector>
#include <functional>

#include "Machine.hpp"
#include "Trap.hpp"

struct Inst {
  int result = -1;
  std::string name = "nop";

  std::function<
    int(const Inst&, Machine&)
  > doit;

  struct {
    u8 rd, rs1, rs2, shamt;
    i16 offset;
    i32 imm;
    u16 csr;
    int csr_idx;
  } preValue;
};

struct Syscall {
  int syscall_num;
  std::string syscall_name;
  std::function<
    int(CPU&, Memory&)
  > doit;
};

inline std::vector<Syscall> syscalls {
  { 1, "sys_print_reg", [](CPU& cpu, Memory& mem) -> int {
    int val = cpu.gp_regs[cpu.reg2idx("a0")];
    printf("%d\n", val);
    return 0;
  } },
  { 2, "sys_print_str", [](CPU& cpu, Memory& mem) -> int {
    u32 addr = cpu.gp_regs[cpu.reg2idx("a0")];
    bool end = false;
    while (!end) {
      u32 word = mem.read(addr);
      u8 bytes[] {
        (u8)((word >> 0) & 0xff),
        (u8)((word >> 8) & 0xff),
        (u8)((word >> 16) & 0xff),
        (u8)((word >> 24) & 0xff),
      };
      for (int i = 0; i < 4; i++) {
        if (bytes[i] == 0) {
          end = true;
          break;
        }
        printf("%c", (char)bytes[i]);
      }
      addr += 4;
    }
    return 0;
  } },
};

inline Inst parseInst(u32 inst, Machine& m) {
  Inst res;
  res.result = 0;
  char buf[64];

  int opcode = inst & 0x7f;
  switch(opcode) {
    case 0x03: {
      u8 rd = (inst >> 7) & 0x1f;
      u8 rs1 = (inst >> 15) & 0x1f;
      i16 imm = ((i32)inst >> 20);

      res.preValue.rd = rd;
      res.preValue.rs1 = rs1;
      res.preValue.imm = imm;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {


        case 0b000: {
          res.name = "lb ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, "0x%x(%s)", (i16)imm, m.cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 addr = (i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.imm;
            u8 op = addr % 4;
            i8 byte = 0;
            if (0 == op) {
              byte = m.readMem(addr);
            } else if (2 == op) {
              byte = m.readMem(addr) >> 16;
            } else if (1 == op) {
              byte = m.readMem(addr) >> 8;
            } else {
              byte = m.readMem(addr) >> 24;
            }
            m.cpu.write_reg(inst.preValue.rd, i32(byte));
            return 0;
          };

          break;
        }
        case 0b001: {
          res.name = "lh ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, "0x%x(%s)", (i16)imm, m.cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 addr = (i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.imm;
            u8 op = addr % 4;
            i16 hw = 0;
            if (0 == op) {
              hw = m.readMem(addr);
            } else if (2 == op) {
              hw = m.readMem(addr) >> 16;
            } else {
              fprintf(stderr, RED("Unaligned memory access at 0x%08x\n"), addr);
              return -1;
            }
            m.cpu.write_reg(inst.preValue.rd, i32(hw));
            return 0;
          };

          break;
        }
        case 0b010: {
          res.name = "lw ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, "%d(%s)", (i16)imm, m.cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 addr = (i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.imm;
            m.cpu.write_reg(inst.preValue.rd, m.readMem(addr));
            return 0;
          };

          break;
        }
        case 0b100: {
          res.name = "lbu ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, "0x%x(%s)", (i16)imm, m.cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 addr = (i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.imm;
            u8 op = addr % 4;
            u8 byte = 0;
            if (0 == op) {
              byte = m.readMem(addr);
            } else if (2 == op) {
              byte = m.readMem(addr) >> 16;
            } else if (1 == op) {
              byte = m.readMem(addr) >> 8;
            } else {
              byte = m.readMem(addr) >> 24;
            }
            m.cpu.write_reg(inst.preValue.rd, u32(byte));
            return 0;
          };

          break;
        }
        case 0b101: {
          res.name = "lhu ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, "0x%x(%s)", (i16)imm, m.cpu.reg_names[rs1].c_str());
          res.name += ", " + std::string(buf);
          
          res.doit = [](const Inst& inst, Machine& m) {
            u32 addr = (i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.imm;
            u8 op = addr % 4;
            u16 hw = 0;
            if (0 == op) {
              hw = m.readMem(addr);
            } else if (2 == op) {
              hw = m.readMem(addr) >> 16;
            } else {
              fprintf(stderr, RED("Unaligned memory access at 0x%08x\n"), addr);
              return -1;
            }
            m.cpu.write_reg(inst.preValue.rd, u32(hw));
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

      res.preValue.rd = rd;
      res.preValue.rs1 = rs1;
      res.preValue.imm = imm;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          res.name = "addi ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", m.cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            m.cpu.write_reg(inst.preValue.rd, (i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.imm);
            return 0;
          };

          break;
        }
        case 0b010: {
          res.name = "slti ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", m.cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            i32 val = m.cpu.gp_regs[inst.preValue.rs1];
            if (val < inst.preValue.imm) {
              m.cpu.write_reg(inst.preValue.rd, 1);
            } else {
              m.cpu.write_reg(inst.preValue.rd, 0);
            }
            return 0;
          };

          break;
        }
        case 0b011: {
          res.name = "sltiu ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", m.cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 val = m.cpu.gp_regs[inst.preValue.rs1];
            if (val < (u32)inst.preValue.imm) {
              m.cpu.write_reg(inst.preValue.rd, 1);
            } else {
              m.cpu.write_reg(inst.preValue.rd, 0);
            }
            return 0;
          };

          break;
        }
        case 0b100: {
          res.name = "xori ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", m.cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] ^ inst.preValue.imm);
            return 0;
          };

          break;
        }
        case 0b110: {
          res.name = "ori ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", m.cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] | inst.preValue.imm);
            return 0;
          };

          break;
        }
        case 0b111: {
          res.name = "andi ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", m.cpu.reg_names[rs1].c_str(), imm);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] & inst.preValue.imm);
            return 0;
          };

          break;
        }
        case 0b001: {
          u8 shamt = (inst >> 20) & 0x1f;
          res.preValue.shamt = shamt;
          res.name = "slli ";
          res.name += m.cpu.reg_names[rd];
          sprintf(buf, ", %s, %d", m.cpu.reg_names[rs1].c_str(), shamt);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] << inst.preValue.shamt);
            return 0;
          };

          break;
        }
        case 0b101: {
          int op = (inst >> 31);
          u8 shamt = (inst >> 20) & 0x1f;
          res.preValue.shamt = shamt;
          if (op) {
            res.name = "srai ";
            res.name += m.cpu.reg_names[rd];
            sprintf(buf, ", %s, %d", m.cpu.reg_names[rs1].c_str(), shamt);
            res.name += std::string(buf);

            res.doit = [](const Inst& inst, Machine& m) {
              m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] >> inst.preValue.shamt);
              return 0;
            };
          } else {
            res.name = "srli ";
            res.name += m.cpu.reg_names[rd];
            sprintf(buf, ", %s, %d", m.cpu.reg_names[rs1].c_str(), shamt);
            res.name += std::string(buf);

            res.doit = [](const Inst& inst, Machine& m) {
              m.cpu.write_reg(inst.preValue.rd, (u32)m.cpu.gp_regs[inst.preValue.rs1] >> inst.preValue.shamt);
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
      res.preValue.rd = rd;
      res.preValue.imm = imm;
      res.name += m.cpu.reg_names[rd];
      sprintf(buf, ", 0x%x", imm);
      res.name += std::string(buf);
      
      res.doit = [](const Inst& inst, Machine& m) {
        m.cpu.write_reg(inst.preValue.rd, m.cpu.pc + inst.preValue.imm);
        return 0;
      };

      break;
    }
    case 0x23: {
      u8 rs1 = (inst >> 15) & 0x1f; 
      u8 rs2 = (inst >> 20) & 0x1f;
      i16 offset = (((i32)inst >> 25) <<  5) | (((i32)inst >> 7) & 0x1f);

      res.preValue.rs1 = rs1;
      res.preValue.rs2 = rs2;
      res.preValue.offset = offset;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          res.name = "sb ";
          res.name += m.cpu.reg_names[rs2];
          sprintf(buf, ", %d(%s)", offset, m.cpu.reg_names[rs1].c_str());
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 addr = (i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.offset;
            u8 byte = m.cpu.gp_regs[inst.preValue.rs2];
            u8 op = addr % 4;
            u32 val = m.readMem(addr);
            if (0 == op) {
              val = (val & 0xffffff00) | byte;
            } else if (2 == op) {
              val = (val & 0xff00ffff) | (byte << 16);
            } else if (1 == op) {
              val = (val & 0xffff00ff) | (byte << 8);
            } else {
              val = (val & 0x00ffffff) | (byte << 24);
            }
            m.writeMem(addr, val);
            return 0;
          };

          break;
        }
        case 0b001: {
          res.name = "sh ";
          res.name += m.cpu.reg_names[rs2];
          sprintf(buf, ", %d(%s)", offset, m.cpu.reg_names[rs1].c_str());
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 addr = (i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.offset;
            u16 hw = m.cpu.gp_regs[inst.preValue.rs2];
            u8 op = addr % 4;
            u32 val = m.readMem(addr);
            
            if (0 == op) {
              val = (val & 0xffff0000) | hw;
            } else if (2 == op) {
              val = (val & 0x0000ffff) | (hw << 16);
            } else {
              fprintf(stderr, RED("Unaligned memory access at 0x%08x\n"), addr);
              return -1;
            }
            m.writeMem(addr, val);
            return 0;
          };

          break;
        }
        case 0b010: {
          res.name = "sw ";
          res.name += m.cpu.reg_names[rs2];
          sprintf(buf, ", %d(%s)", offset, m.cpu.reg_names[rs1].c_str());
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            i32 addr = (i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.offset;
            u32 val = m.cpu.gp_regs[inst.preValue.rs2];
            m.writeMem(addr, val);
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

      res.preValue.rd = rd;
      res.preValue.rs1 = rs1;
      res.preValue.rs2 = rs2;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          int funct7 = (inst >> 25);
          if (funct7 == 0b0100000) {
            res.name = "sub ";
            res.name += m.cpu.reg_names[rd];
            res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, Machine& m) {
              m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] - m.cpu.gp_regs[inst.preValue.rs2]);
              return 0;
            };
          } else if (funct7 == 0b0000000) {
            res.name = "add ";
            res.name += m.cpu.reg_names[rd];
            res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, Machine& m) {
              m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] + m.cpu.gp_regs[inst.preValue.rs2]);
              return 0;
            };
          } else {
            res.name = "mul ";
            res.name += m.cpu.reg_names[rd];
            res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, Machine& m) {
              // LAB 1 todo
              assert(0);
              return 0;
            };
          }
          break;
        }
        case 0b001: {
          res.name = "sll ";
          res.name += m.cpu.reg_names[rd];
          res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, Machine& m) {
            m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] << (m.cpu.gp_regs[inst.preValue.rs2] & 0x1f));
            return 0;
          };
          break;
        }
        case 0b010: {
          res.name = "slt ";
          res.name += m.cpu.reg_names[rd];
          res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, Machine& m) {
            i32 val1 = m.cpu.gp_regs[inst.preValue.rs1];
            i32 val2 = m.cpu.gp_regs[inst.preValue.rs2];
            if (val1 < val2) {
              m.cpu.write_reg(inst.preValue.rd, 1);
            } else {
              m.cpu.write_reg(inst.preValue.rd, 0);
            }
            return 0;
          };
          break;
        }
        case 0b011: {
          res.name = "sltu ";
          res.name += m.cpu.reg_names[rd];
          res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, Machine& m) {
            u32 val1 = m.cpu.gp_regs[inst.preValue.rs1];
            u32 val2 = m.cpu.gp_regs[inst.preValue.rs2];
            if (val1 < val2) {
              m.cpu.write_reg(inst.preValue.rd, 1);
            } else {
              m.cpu.write_reg(inst.preValue.rd, 0);
            }
            return 0;
          };
          break;
        }
        case 0b100: {

          int funct7 = (inst >> 25);

          if (funct7 == 0) {
            res.name = "xor ";
            res.name += m.cpu.reg_names[rd];
            res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, Machine& m) {
              m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] ^ m.cpu.gp_regs[inst.preValue.rs2]);
              return 0;
            };
          } else if (funct7 == 0b0000001) {
            res.name = "div ";
            res.name += m.cpu.reg_names[rd];
            res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, Machine& m) {
              // LAB 1 todo
              assert(0);
              return 0;
            };
          }

          break;
        }
        case 0b101: {
          int op = (inst >> 31);
          if (op) {
            res.name = "sra ";
            res.name += m.cpu.reg_names[rd];
            res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, Machine& m) {
              m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] >> (m.cpu.gp_regs[inst.preValue.rs2] & 0x1f));
              return 0;
            };
          } else {
            res.name = "srl ";
            res.name += m.cpu.reg_names[rd];
            res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, Machine& m) {
              m.cpu.write_reg(inst.preValue.rd, (u32)m.cpu.gp_regs[inst.preValue.rs1] >> (m.cpu.gp_regs[inst.preValue.rs2] & 0x1f));
              return 0;
            };
          }
          break;
        }
        case 0b110: {

          int funct7 = (inst >> 25);

          if (funct7 == 0) {
            res.name = "or ";
            res.name += m.cpu.reg_names[rd];
            res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, Machine& m) {
              m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] | m.cpu.gp_regs[inst.preValue.rs2]);
              return 0;
            };
          } else if (funct7 == 0b0000001) {
            res.name = "rem ";
            res.name += m.cpu.reg_names[rd];
            res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

            res.doit = [](const Inst& inst, Machine& m) {
              // LAB 1 todo
              assert(0);
              return 0;
            };
          }

          break;
        }
        case 0b111: {
          res.name = "and ";
          res.name += m.cpu.reg_names[rd];
          res.name += ", " + m.cpu.reg_names[rs1] + ", " + m.cpu.reg_names[rs2];

          res.doit = [](const Inst& inst, Machine& m) {
            m.cpu.write_reg(inst.preValue.rd, m.cpu.gp_regs[inst.preValue.rs1] & m.cpu.gp_regs[inst.preValue.rs2]);
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

      res.preValue.rd = rd;
      res.preValue.imm = imm;

      res.name = "lui ";
      res.name += m.cpu.reg_names[rd];
      sprintf(buf, ", 0x%x", imm);
      res.name += std::string(buf);

      res.doit = [](const Inst& inst, Machine& m) {
        m.cpu.write_reg(inst.preValue.rd, inst.preValue.imm);
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
      offset <<= 1;

      res.preValue.rs1 = rs1;
      res.preValue.rs2 = rs2;
      res.preValue.offset = offset;

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          res.name = "beq ";
          res.name += m.cpu.reg_names[rs1];
          res.name += ", " + m.cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            if (m.cpu.gp_regs[inst.preValue.rs1] == m.cpu.gp_regs[inst.preValue.rs2]) {
              m.cpu.pc = (i32)m.cpu.pc + (i32)(inst.preValue.offset);
              m.cpu.pc -= 4;
            }
            return 0;
          };
          break;
        }
        case 0b001: {
          res.name = "bne ";
          res.name += m.cpu.reg_names[rs1];
          res.name += ", " + m.cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            if (m.cpu.gp_regs[inst.preValue.rs1] != m.cpu.gp_regs[inst.preValue.rs2]) {
              m.cpu.pc = (i32)m.cpu.pc + (i32)(inst.preValue.offset);
              m.cpu.pc -= 4;
            }
            return 0;
          };
          break;
        }
        case 0b100: {
          res.name = "blt ";
          res.name += m.cpu.reg_names[rs1];
          res.name += ", " + m.cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            if ((i32)m.cpu.gp_regs[inst.preValue.rs1] < (i32)m.cpu.gp_regs[inst.preValue.rs2]) {
              m.cpu.pc = (i32)m.cpu.pc + (i32)(inst.preValue.offset);
              m.cpu.pc -= 4;
            }
            return 0;
          };
          break;
        }
        case 0b101: {
          res.name = "bge ";
          res.name += m.cpu.reg_names[rs1];
          res.name += ", " + m.cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            if ((i32)m.cpu.gp_regs[inst.preValue.rs1] >= (i32)m.cpu.gp_regs[inst.preValue.rs2]) {
              m.cpu.pc = (i32)m.cpu.pc + (i32)(inst.preValue.offset);
              m.cpu.pc -= 4;
            }
            return 0;
          };
          break;
        }
        case 0b110: {
          res.name = "bltu ";
          res.name += m.cpu.reg_names[rs1];
          res.name += ", " + m.cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            if ((u32)m.cpu.gp_regs[inst.preValue.rs1] < (u32)m.cpu.gp_regs[inst.preValue.rs2]) {
              m.cpu.pc = (i32)m.cpu.pc + (i32)(inst.preValue.offset);
              m.cpu.pc -= 4;
            }
            return 0;
          };
          break;
        }
        case 0b111: {
          res.name = "bgeu ";
          res.name += m.cpu.reg_names[rs1];
          res.name += ", " + m.cpu.reg_names[rs2];
          sprintf(buf, ", 0x%x", offset);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            if ((u32)m.cpu.gp_regs[inst.preValue.rs1] >= (u32)m.cpu.gp_regs[inst.preValue.rs2]) {
              m.cpu.pc = (i32)m.cpu.pc + (i32)(inst.preValue.offset);
              m.cpu.pc -= 4;
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

      offset <<= 1;

      res.preValue.rd = rd;
      res.preValue.rs1 = rs1;
      res.preValue.offset = offset;

      res.name = "jalr ";
      res.name += m.cpu.reg_names[rd];
      sprintf(buf, ", %d(%s)", offset, m.cpu.reg_names[rs1].c_str());
      res.name += std::string(buf);

      res.doit = [](const Inst& inst, Machine& m) {
        u32 tmp = m.cpu.pc + 4;
        m.cpu.pc = ((i32)m.cpu.gp_regs[inst.preValue.rs1] + inst.preValue.offset) & 0xfffffffe;
        m.cpu.pc -= 4;
        m.cpu.write_reg(inst.preValue.rd, tmp);
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

      offset <<= 1;

      res.preValue.rd = rd;
      res.preValue.offset = offset;

      res.name = "jal ";
      res.name += m.cpu.reg_names[rd];
      sprintf(buf, ", 0x%x", offset);
      res.name += std::string(buf);

      res.doit = [](const Inst& inst, Machine& m) {
        m.cpu.write_reg(inst.preValue.rd, m.cpu.pc + 4);
        m.cpu.pc = (i32)m.cpu.pc + (i32)inst.preValue.offset;
        m.cpu.pc -= 4;
        return 0;
      };

      break;
    }
    case 0x73: {

      u8 rd = (inst >> 7) & 0x1f;
      u8 rs1 = (inst >> 15) & 0x1f;
      u16 csr = (inst >> 20) & 0xfff;
      int csr_idx = 0;
      if (csr == 0x305) {
        csr_idx = 0;
      } else if (csr == 0x341) {
        csr_idx = 1;
      } else if (csr == 0x342) {
        csr_idx = 2;
      } else if (csr == 0x305) {
        csr_idx = 3;
      }

      int funct3 = (inst >> 12) & 0x7;
      switch(funct3) {
        case 0b000: {
          if (inst == 0x30200073) {
            res.name = "mret";
            res.doit = [](const Inst& inst, Machine& m) {
              trap_ret(m);
              return 0;
            };
            break;
          }
          int op = (inst >> 20);
          if (op) {
            res.name = "ebreak (skip)";
          } else {
            res.name = "ecall";
            res.doit = [](const Inst& inst, Machine& m) {
              u8 syscall_num = m.cpu.gp_regs[m.cpu.reg2idx("a7")];
              int r = -1;
              for (auto& syscall : syscalls) {
                if (syscall.syscall_num == syscall_num) {
                  r = syscall.doit(m.cpu, m.memory);
                  break;
                }
              }
              return r;
            };
          }
          break;
        }
        case 0b001: {
          res.name = "csrrw ";
          res.preValue.rd = rd;
          res.preValue.rs1 = rs1;
          res.preValue.csr = csr;
          res.preValue.csr_idx = csr_idx;

          std::string csr_name = m.cpu.csr_names[csr_idx];

          sprintf(buf, "%s, %s, %s", m.cpu.reg_names[rd].c_str(), csr_name.c_str(), m.cpu.reg_names[rs1].c_str());
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 csr_val = m.cpu.csr[inst.preValue.csr_idx];
            m.cpu.write_reg(inst.preValue.rd, csr_val);
            m.cpu.csr[inst.preValue.csr_idx] = m.cpu.gp_regs[inst.preValue.rs1];
            return 0;
          };

          break;
        }
        case 0b010: {
          res.name = "csrrs ";
          res.preValue.rd = rd;
          res.preValue.rs1 = rs1;
          res.preValue.csr = csr;
          res.preValue.csr_idx = csr_idx;

          std::string csr_name = m.cpu.csr_names[csr_idx];
          sprintf(buf, "%s, %s, %s", m.cpu.reg_names[rd].c_str(), csr_name.c_str(), m.cpu.reg_names[rs1].c_str());
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 csr_val = m.cpu.csr[inst.preValue.csr_idx];
            m.cpu.write_reg(inst.preValue.rd, csr_val);
            m.cpu.csr[inst.preValue.csr_idx] = csr_val | m.cpu.gp_regs[inst.preValue.rs1];
            return 0;
          };

          break;
        }
        case 0b011: {
          res.name = "csrrc ";
          res.preValue.rd = rd;
          res.preValue.rs1 = rs1;
          res.preValue.csr = csr;
          res.preValue.csr_idx = csr_idx;

          std::string csr_name = m.cpu.csr_names[csr_idx];
          sprintf(buf, "%s, %s, %s", m.cpu.reg_names[rd].c_str(), csr_name.c_str(), m.cpu.reg_names[rs1].c_str());
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 csr_val = m.cpu.csr[inst.preValue.csr_idx];
            m.cpu.write_reg(inst.preValue.rd, csr_val);
            m.cpu.csr[inst.preValue.csr_idx] = csr_val & ~(m.cpu.gp_regs[inst.preValue.rs1]);
            return 0;
          };

          break;
        }
        case 0b101: {
          res.name = "csrrwi ";
          res.preValue.rd = rd;
          res.preValue.rs1 = rs1;
          res.preValue.csr = csr;
          res.preValue.csr_idx = csr_idx;

          std::string csr_name = m.cpu.csr_names[csr_idx];
          sprintf(buf, "%s, %s, 0b%05b", m.cpu.reg_names[rd].c_str(), csr_name.c_str(), rs1);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 csr_val = m.cpu.csr[inst.preValue.csr_idx];
            m.cpu.write_reg(inst.preValue.rd, csr_val);
            m.cpu.csr[inst.preValue.csr_idx] = (u32)inst.preValue.rs1;
            return 0;
          };

          break;
        }
        case 0b110: {
          res.name = "csrrsi ";
          res.preValue.rd = rd;
          res.preValue.rs1 = rs1;
          res.preValue.csr = csr;
          res.preValue.csr_idx = csr_idx;

          std::string csr_name = m.cpu.csr_names[csr_idx];
          sprintf(buf, "%s, %s, 0b%05b", m.cpu.reg_names[rd].c_str(), csr_name.c_str(), rs1);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 csr_val = m.cpu.csr[inst.preValue.csr_idx];
            m.cpu.write_reg(inst.preValue.rd, csr_val);
            m.cpu.csr[inst.preValue.csr_idx] = csr_val | (u32)inst.preValue.rs1;
            return 0;
          };

          break;
        }
        case 0b111: {
          res.name = "csrrci ";
          res.preValue.rd = rd;
          res.preValue.rs1 = rs1;
          res.preValue.csr = csr;
          res.preValue.csr_idx = csr_idx;

          std::string csr_name = m.cpu.csr_names[csr_idx];
          sprintf(buf, "%s, %s, 0b%05b", m.cpu.reg_names[rd].c_str(), csr_name.c_str(), rs1);
          res.name += std::string(buf);

          res.doit = [](const Inst& inst, Machine& m) {
            u32 csr_val = m.cpu.csr[inst.preValue.csr_idx];
            m.cpu.write_reg(inst.preValue.rd, csr_val);
            m.cpu.csr[inst.preValue.csr_idx] = csr_val & ~((u32)inst.preValue.rs1);
            return 0;
          };

          break;
        }
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
