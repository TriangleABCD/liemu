#pragma once
#ifndef SDB_H
#define SDB_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <functional>

#include "common.hpp"
#include "Machine.hpp"
#include "Inst.hpp"
#include "Watch.hpp"

namespace Sdb {

inline std::map<u32, Inst> getParsedInst;

inline WatchList watchList;

inline void load_insts_into_mem(std::string path, Machine& m) {
  std::fstream file(path);
  if (!file.is_open()) {
    fprintf(stderr, RED("failed to open file: %s\n"), path.c_str());
    return;
  }
  std::string line;
  std::vector<u32> insts;
  while(file >> line) {
    u32 inst = strtol(line.c_str(), NULL, 16);
    insts.push_back(inst);
  }
  file.close();
  u32 addr = MEM_START;
  for (const auto& inst : insts) {
    m.memory.write(addr, inst);
    addr += 4;
  }
}


inline void preParseInst(Machine& m) {
  auto pc = m.cpu.pc;
  while (1) {
    u32 inst_value = m.memory.read(pc);
    if (inst_value == MAGIC) {
      break;
    }

    auto inst =  parseInst(inst_value, m);
    if (inst.result == -1) {
      fprintf(stderr, RED("parse inst failed\n"));
      break;
    }

    getParsedInst[pc] = inst;
    pc += 4;
  }
}


inline void resetMachine(Machine& m, std::string path, u32 _start = CODE_START, u32 _end = CODE_END) {
  m.memory.resetMemory();
  load_insts_into_mem(path, m);
  m.cpu.resetCPU(_start, _end);
  preParseInst(m);

  m.memory.write(0x80002400, 0x12345678);
}


inline int execute_one_step(Machine& m) {
  if (m.cpu.pc < CODE_START) {
    return 1;
  }

  if (getParsedInst.find(m.cpu.pc) == getParsedInst.end()) {
    return -1;
  }

  Inst inst = getParsedInst[m.cpu.pc];
  
  if (inst.result == -1) {
    fprintf(stderr, RED("finish\n"));
    return 1;
  }

  int r = inst.doit(inst, m);
  m.cpu.pc += 4;

  if (-1 == r) {
    fprintf(stderr, RED("exec error\n"));
    return r;
  }
  return r != 0;
}


inline int execute(Machine& m) {
  int r = 0;
  while (1) {
    r = execute_one_step(m);
    if (r == -1 || r == 1) {
      break;
    }
    if (watchList.update_watchpoint(m.cpu, m.memory)) {
      break;
    }
  }
  return r;
}


struct Command {
  std::string cmd_name;
  std::string cmd_descr;
  std::function<int(Machine&, const std::vector<std::string>&)> handle_func;
};

enum CmdResult {
  CMD_OK = 0,
  CMD_QUIT = 1,
  CMD_ERR = -1,
};


int cmd_help(Machine& m, const std::vector<std::string>& cmd);
int cmd_quit(Machine& m, const std::vector<std::string>& cmd);
int cmd_clear(Machine& m, const std::vector<std::string>& cmd);
int cmd_continue(Machine& m, const std::vector<std::string>& cmd);
int cmd_si(Machine& m, const std::vector<std::string>& cmd);
int cmd_info(Machine& m, const std::vector<std::string>& cmd);
int cmd_x(Machine& m, const std::vector<std::string>& cmd);
int cmd_ls(Machine& m, const std::vector<std::string>& cmd);
int cmd_w(Machine& m, const std::vector<std::string>& cmd);
int cmd_d(Machine& m, const std::vector<std::string>& cmd);


inline std::vector<Command> cmds {
  { "help", "show this help info", cmd_help },
  { "q", "quit liemu", cmd_quit },
  { "clear", "clear the screen", cmd_clear },
  { "c", "continue to execute next instructions", cmd_continue },
  { "si", "si [N], execute N steps", cmd_si },
  { "info", "info r/w [reg_name/watchpoint], show reg/watchpoint value", cmd_info },
  { "x", "x addr, print mem value on addr", cmd_x },
  { "ls", "enable listing instructions automaticlly", cmd_ls },
  { "w", "add a watch point", cmd_w },
  { "d", "d watchpoint delete a watch point", cmd_d },
};


inline int cmd_help(Machine& m, const std::vector<std::string>& cmd) {
  for (auto& c : cmds) {
    printf(GREEN("%s")":\t%s\n", c.cmd_name.c_str(), c.cmd_descr.c_str());
  }
  return CmdResult::CMD_OK;
}


inline int cmd_quit(Machine& m, const std::vector<std::string>& cmd) {
  return CmdResult::CMD_QUIT;
}


inline int cmd_clear(Machine& m, const std::vector<std::string>& cmd) {
  printf("\033[2J\033[H");
  return CmdResult::CMD_OK;
}


inline int cmd_continue(Machine& m, const std::vector<std::string>& cmd) {
  int r = execute(m);
  return CmdResult(r);
}


inline int cmd_si(Machine& m, const std::vector<std::string>& cmd) {
  int n = 1;
  if (cmd.size() >= 2) {
    try {
      n = std::stoi(cmd[1]);
    } catch (std::exception& e) {
      return CmdResult::CMD_ERR;
    }
  }

  int r = 0;
  for (int i = 0; i < n; i++) {
    r = execute_one_step(m);
    if (r == -1 || r == 1) {
      break;
    }
    if (watchList.update_watchpoint(m.cpu, m.memory)) {
      break;
    }
  }
  return CmdResult(r);
}


inline int cmd_info(Machine& m, const std::vector<std::string>& cmd) {
  if (cmd.size() == 1) {
    return CmdResult::CMD_ERR;
  }
  if (cmd.size() == 2 && cmd[1] != "r" && cmd[1] != "w") {
    return CmdResult::CMD_ERR;
  }
  
  if (cmd[1] == "w") {
    watchList.info_watchpoint(m.cpu, m.memory); 
    return CmdResult::CMD_OK;
  }

  if (cmd.size() == 2) {
    m.cpu.info_reg();
  } else {
    for (auto& cc : cmd) {
      if (cc == "info" || cc == "r") {
        continue;
      }
      m.cpu.info_reg(cc);
    }
  }
  return CmdResult::CMD_OK;
}


inline int cmd_x(Machine& m, const std::vector<std::string>& cmd) {
  if (cmd.size() <= 1 || cmd.size() > 3) {
    return CmdResult::CMD_ERR;
  }
  
  int n = atoi(cmd[1].c_str());
  u32 addr = strtol(cmd[2].c_str()+2, nullptr, 16);

  for (int i = 0; i < n; i++) {
    u32 word = m.readMem(addr + i * 4);
    unsigned int bytes[4] {
      (word) & 0xff,
      (word >> 8) & 0xff,
      (word >> 16) & 0xff,
      (word >> 24) & 0xff,
    };
    printf(GREEN("0x%08x")":\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n", addr + i * 4, bytes[3], bytes[2], bytes[1], bytes[0]);
  }

  return CmdResult::CMD_OK;
}


inline int cmd_ls(Machine& m, const std::vector<std::string>& cmd) {
  u32 pc = m.cpu.pc;
  if (m.memory.read(pc) == MAGIC) {
    return 0;
  }
  u32 beg = pc - 5 * sizeof(u32);
  if (beg < 0x80000000) {
    beg = 0x80000000;
  }
  printf(GREEN("##############################################################\n"));
  for (int i = 0; ; i++) {
    u32 cur = beg + i * sizeof(u32);
    if (cur > pc && cur - pc > 5 * sizeof(u32)) {
      break;
    }
    u32 inst = m.memory.read(cur);
    if (inst == MAGIC) {
      break;
    }
    int line = (cur - 0x80000000) / sizeof(u32);
    if (cur == pc) {
      printf("\033[32m>%d\t", line+1);
      printf("0x%08x\t", cur);
      printf("%08x\t", inst);
      printf("%s\033[0m\n", getParsedInst[cur].name.c_str());
    } else {
      printf(" %d\t", line+1);
      printf("0x%08x\t", cur);
      printf("%08x\t", inst);
      printf("%s\n", getParsedInst[cur].name.c_str());
    }
  }
  printf(GREEN("##############################################################\n"));
  return CmdResult::CMD_OK;
}


inline int cmd_w(Machine& m, const std::vector<std::string>& cmd) {
  if (cmd.size() != 3) {
    return CmdResult::CMD_ERR;
  }
  watchList.add_watchpoint(m.cpu, m.memory, cmd[1], cmd[2]);
  return CmdResult::CMD_OK;
}


inline int cmd_d(Machine& m, const std::vector<std::string>& cmd) {
  if (cmd.size() != 2) {
    return CmdResult::CMD_ERR;
  }
  int id = atoi(cmd[1].c_str());
  watchList.del_watchpoint(id);
  return CmdResult::CMD_OK;
}

}

#endif 
