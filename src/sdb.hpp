#pragma once
#ifndef SDB_H
#define SDB_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <functional>

#include "common.hpp"
#include "Machine.hpp"
#include "Inst.hpp"

namespace Sdb {

inline struct {
  bool list = false;
} config;


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

  return CmdResult::CMD_OK;
}

inline int cmd_si(Machine& m, const std::vector<std::string>& cmd) {

  return CmdResult::CMD_OK;
}

inline int cmd_info(Machine& m, const std::vector<std::string>& cmd) {
  if (cmd.size() == 1) {
    return CmdResult::CMD_ERR;
  }
  if (cmd.size() == 2 && cmd[1] != "r" && cmd[1] != "w") {
    return CmdResult::CMD_ERR;
  }
  
  if (cmd[1] == "w") {
    m.watchList.info_watchpoint(m.cpu, m.memory); 
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

  return CmdResult::CMD_OK;
}

inline int cmd_ls(Machine& m, const std::vector<std::string>& cmd) {

  return CmdResult::CMD_OK;
}

inline int cmd_w(Machine& m, const std::vector<std::string>& cmd) {

  return CmdResult::CMD_OK;
}

inline int cmd_d(Machine& m, const std::vector<std::string>& cmd) {

  return CmdResult::CMD_OK;
}

}

#endif 
