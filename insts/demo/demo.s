.globl main

main:
  li a0, 0x80002400
  lw a1, 0(a0)
  li a0, 0x80002404
  sw a1, 0(a0)
  lw a2, 0(a0)
