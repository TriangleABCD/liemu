#include "machine.hpp"

Machine m;

int main(int argc, char* argv[]) {
  m.mem.write_vmem(START_ADDR, 0);
  m.mem.write_vmem(START_ADDR+4, 1);
  m.mem.write_vmem(START_ADDR+8, 2);
  m.mem.write_vmem(START_ADDR+12, 3);
  m.mem.write_vmem(START_ADDR+16, 4);

  m.execute();
  return 0;
}
