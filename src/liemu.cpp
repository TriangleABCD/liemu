#include <cstdio>
#include <cstdlib>
#include <functional>
#include <readline/readline.h>
#include <readline/history.h>

#include "machine.hpp"

Machine m;
bool auto_ls = false;

struct Command {
  std::string cmd_name;
  std::string descr;
  std::function<int(const std::vector<std::string>&)> handle_func;
};

int cmd_help(const std::vector<std::string>& cmd);
int cmd_quit(const std::vector<std::string>& cmd);
int cmd_clear(const std::vector<std::string>& cmd);
int cmd_continue(const std::vector<std::string>& cmd);
int cmd_si(const std::vector<std::string>& cmd);
int cmd_info(const std::vector<std::string>& cmd);
int cmd_x(const std::vector<std::string>& cmd);
int cmd_ls(const std::vector<std::string>& cmd);
int cmd_w(const std::vector<std::string>& cmd);
int cmd_d(const std::vector<std::string>& cmd);
int do_ls();

std::vector<Command> cmds {
  { "help", "show help info", cmd_help },
  { "q", "quit liemu", cmd_quit },
  { "clear", "clear the screen", cmd_clear },
  { "c", "continue execute rest of insts", cmd_continue },
  { "si", "go N steps", cmd_si },
  { "info", "show reg/watchpoint value", cmd_info },
  { "x", "print mem value", cmd_x },
  { "ls", "list instructions", cmd_ls },
  { "w", "add a watch point", cmd_w },
  { "d", "delete a watch point", cmd_d },
};

int cmd_help(const std::vector<std::string>& cmd) {
  for (auto& c : cmds) {
    printf("\033[32m%s\033[0m:\t%s\n", c.cmd_name.c_str(), c.descr.c_str());
  }
  return 0;
}

int cmd_quit(const std::vector<std::string>& cmd) {
  return 1;
}

int cmd_clear(const std::vector<std::string>& cmd) {
  printf("\033[2J\033[H");
  return 0;
}

int cmd_continue(const std::vector<std::string>& cmd) {
  int r = m.execute();
  return r;
}

int cmd_si(const std::vector<std::string>& cmd) {
  int n = 1;
  if (cmd.size() >= 2) {
    try {
      n = std::stoi(cmd[1]);
    } catch(...) {
      return -1;
    }
  }
  int r = 0;
  for (int i = 0; i < n; i++) {
    r = m.execute_one_step();
    if (-1 == r || 1 == r) {
      break;
    }
  }
  if (auto_ls) {
    cmd_clear({});
    do_ls();
  }
  return r;
}

int cmd_info(const std::vector<std::string>& cmd) {
  if (cmd.size() == 1) {
    return -1;
  }
  if (cmd.size() == 2 && cmd[1] != "r" && cmd[1] != "w") {
    return -1;
  }

  if (cmd[1] == "w") {
    m.watchList.info_watchpoint(m.cpu, m.mem);
    return 0;
  }
  
  if (cmd.size() == 2) {
    m.cpu.info_reg();
  } else {
    for (auto& cc : cmd) {
      if (cc == "info" || cc == "reg") continue;
      m.cpu.info_reg(cc);
    }
  }
  return 0;
}

int cmd_x(const std::vector<std::string>& cmd) {
  if (cmd.size() <= 1 || cmd.size() > 3) {
    return -1;
  }
  int n = atoi(cmd[1].c_str());
  u32 addr = strtol(cmd[2].c_str()+2, NULL, 16);
  for (int i = 0; i < n; i++) {
    u32 word = m.mem.read_vmem(addr + i * 4);
    unsigned int bytes[4] = {
      (word) & 0xff,
      (word >> 8) & 0xff,
      (word >> 16) & 0xff,
      (word >> 24) & 0xff
    };
    printf("\033[32m0x%08x\033[0m:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n", addr+i*4, bytes[3], bytes[2], bytes[1], bytes[0]);
  }
  return 0;
}

int cmd_ls(const std::vector<std::string>& cmd) {
  auto_ls = !auto_ls;
  cmd_clear({});
  do_ls();
  return 0;
}
int do_ls() {
  u32 pc = m.cpu.pc;
  if (m.mem.read_vmem(pc) == MAGIC) {
    return 0;
  }
  u32 beg = pc - 5 * sizeof(u32);
  if (beg < 0x80000000) {
    beg = 0x80000000;
  }
  printf("\033[32m##############################################################\033[0m\n");
  for (int i = 0; ; i++) {
    u32 cur = beg + i * sizeof(u32);
    if (cur > pc && cur - pc > 5 * sizeof(u32)) {
      break;
    }
    u32 inst = m.mem.read_vmem(cur);
    if (inst == MAGIC) {
      break;
    }
    int line = (cur - 0x80000000) / sizeof(u32);
    if (cur == pc) {
      printf("\033[32m>%d\t", line+1);
      printf("0x%08x\t", cur);
      printf("%08x\t", inst);
      printf("%s\033[0m\n", m.getInst[cur].name.c_str());
    } else {
      printf(" %d\t", line+1);
      printf("0x%08x\t", cur);
      printf("%08x\t", inst);
      printf("%s\n", m.getInst[cur].name.c_str());
    }
  }
  printf("\033[32m##############################################################\033[0m\n");
  return 0;
}

int cmd_w(const std::vector<std::string>& cmd) {
  if (cmd.size() != 3) {
    return -1;
  }
  m.watchList.add_watchpoint(m.cpu, m.mem, cmd[1], cmd[2]);
  return 0;
}

int cmd_d(const std::vector<std::string>& cmd) {
  if (cmd.size() != 2) {
    return -1;
  }
  int id = atoi(cmd[1].c_str());
  m.watchList.del_watchpoint(id);
  return 0; 
}

int main(int argc, char* argv[]) {
  
  m.mem.load_insts_into_mem("insts.txt");
  m.read_insts();

  m.cpu.write_reg("ra", START_ADDR + 0x30);

  char* input;
  using_history();
  while (1) {
    m.watchList.update_watchpoint(m.cpu, m.mem);
    input = readline("\033[32m(liemu)\033[0m");
    add_history(input);
    std::vector<std::string> cmd_words = split(std::string(input));
    free(input);
    int r = -2;
    for (auto& c : cmds) {
      if (cmd_words[0] == c.cmd_name) {
        r = c.handle_func(cmd_words);
        break;
      }
    }
    if (-2 == r) {
      fprintf(stderr, "\033[31mwrong command\033[0m\n");
    } else if (1 == r) {
      break;
    }
  }
  printf("\033[32mquit liemu.\033[0m\n");
  return 0;
}
