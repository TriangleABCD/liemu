.globl main

main:
  li a0, 0x80002400
  li a1, 1024
  li a2, 0
loop:
  slli t0, a2, 2
  add t0, a0, t0
  lw t1, 0(t0)
  addi t1, t1, 1
  sw t1, 0(t0)

  addi a2, a2, 1
  bne a2, a1, loop
