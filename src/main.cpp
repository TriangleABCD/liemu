#include <cstdio>
#include <cstdlib>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "sdb.hpp"

Machine m;

int main(int argc, char* argv[]) {
  char* input;
  using_history();

  assert(argc == 2);
  std::string inst_path = std::string(argv[1]);

  Sdb::cmd_clear(m, {});
  Sdb::resetMachine(m, inst_path, CODE_START, CODE_START-4);

  while (1) {
    input = readline(GREEN("(liemu)"));
    add_history(input);
    std::vector<std::string> cmd_words = split(std::string(input));
    free(input);
    int r = -1;
    for (auto& c : Sdb::cmds) {
      if (cmd_words[0] == c.cmd_name) {
        r = c.handle_func(m, cmd_words);
        break;
      }
    }
    if (Sdb::CmdResult::CMD_ERR == r) {
      fprintf(stderr, RED("wrong command\n"));
    } else if (Sdb::CmdResult::CMD_QUIT == r) {
      break;
    }
  }
  printf(RED("quit liemu.\n"));
  return 0;
}
