#include <cstdio>
#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>

#include "sdb.hpp"

Machine m;

int main(int argc, char* argv[]) {
  char* input;
  using_history();
  
  Sdb::cmd_clear(m, {});

  while (1) {
    input = readline("\033[32m(liemu)\033[0m");
    add_history(input);
    std::vector<std::string> cmd_words = split(std::string(input));
    free(input);
    int r = -2;
    for (auto& c : Sdb::cmds) {
      if (cmd_words[0] == c.cmd_name) {
        r = c.handle_func(m, cmd_words);
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
