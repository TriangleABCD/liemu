#include "machine.hpp"

Machine m;

int main(int argc, char* argv[]) {
  m.cpu.info_reg(std::string(argv[1]));
  return 0;
}
