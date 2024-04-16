#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <functional>
#include <readline/readline.h>
#include <readline/history.h>

#include "machine.hpp"

Machine m;

struct Command {
  std::string cmd_name;
  std::string descr;
  std::function<int(const std::vector<std::string>&)> handle_func;
};

int cmd_help(const std::vector<std::string>& cmd);
int cmd_quit(const std::vector<std::string>& cmd);
int cmd_continue(const std::vector<std::string>& cmd);
int cmd_si(const std::vector<std::string>& cmd);
int cmd_info(const std::vector<std::string>& cmd);
int cmd_x(const std::vector<std::string>& cmd);

std::vector<Command> cmds {
  { "help", "show help info", cmd_help },
  { "q", "quit liemu", cmd_quit },
  { "c", "continue execute rest of insts", cmd_continue },
  { "si", "go N steps", cmd_si },
  { "info", "print reg value", cmd_info },
  { "x", "print mem value", cmd_x },
};

int cmd_help(const std::vector<std::string>& cmd) {
  for (auto& c : cmds) {
    printf("%s:\t%s\n", c.cmd_name.c_str(), c.descr.c_str());
  }
  return 0;
}

int cmd_quit(const std::vector<std::string>& cmd) {
  printf("liemu quited.\n");
  return 1;
}

int cmd_continue(const std::vector<std::string>& cmd) {
  m.execute();
  return 0;
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

  for (int i = 0; i < n; i++) {
    m.execute_one_step();
  }
  return 0;
}

int cmd_info(const std::vector<std::string>& cmd) {
  if (cmd.size() == 1) {
    return -1;
  }
  if (cmd.size() == 2 && cmd[1] != "reg") {
    return -1;
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
    printf("0x%08x:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n", addr+i*4, bytes[3], bytes[2], bytes[1], bytes[0]);
  }
  return 0;
}

std::vector<std::string> split(std::string str) {
  std::vector<std::string> words;
  std::istringstream ss(str);

  std::string word;
  while (ss >> word) {
    words.push_back(word);
  }

  return words;
}

int main(int argc, char* argv[]) {
  
  u32 insts[] = {
    0x00028737,
    0x00c75713,
    0x000027b7,
    0x00c7d793,
    0x00e787b3
  };
  
  for (int i = 0; i < int(sizeof(insts) / sizeof(u32)); i++) {
    m.mem.write_vmem(START_ADDR + 4*i, insts[i]);
  }

  char* input;
  using_history();
  while (1) {
    input = readline("(liemu)");
    add_history(input);
    std::vector<std::string> cmd_words = split(std::string(input));
    free(input);
    int r = -1;
    for (auto& c : cmds) {
      if (cmd_words[0] == c.cmd_name) {
        r = c.handle_func(cmd_words);
        break;
      }
    }
    if (-1 == r) {
      fprintf(stderr, "wrong command\n");
    } else if (1 == r) {
      break;
    }
  }

  return 0;
}
