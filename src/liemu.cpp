#include <cstdio>
#include <cstdlib>
#include <functional>
#include <readline/readline.h>
#include <readline/history.h>

#include "machine.hpp"

Machine m;

// struct Command {
//   std::string cmd_name;
//   std::string descr;
//   std::function<int(std::string)> handle_func;
// };
//
// std::vector<Command> cmds {
//   { "help", "show help info",  },
//   { "q", "quit liemu" }, 
// };

int main(int argc, char* argv[]) {
  m.mem.write_vmem(START_ADDR, 0);
  m.mem.write_vmem(START_ADDR+4, 1);
  m.mem.write_vmem(START_ADDR+8, 2);
  m.mem.write_vmem(START_ADDR+12, 3);
  m.mem.write_vmem(START_ADDR+16, 4);
  
  char* input;
  using_history();
  while (1) {
    input = readline("(liemu)");
    std::string cmd(input);
    free(input);
    if ("q" == cmd || "quit" == cmd || "exit" == cmd) {
      printf("liemu quited\n");
      break;
    } else if ("si" == cmd) {
      m.execute_one_step();
    } else if ("c" == cmd) {
      m.execute();
    } else if ("info" == cmd) {
      m.cpu.info_reg();
    } else {
      fprintf(stderr, "wrong command!\n");
    }
  }

  return 0;
}
