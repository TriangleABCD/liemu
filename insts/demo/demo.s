.globl main

main:
  li a0, 0x80002400
  li t0, 1024
  li t1, 0
loop:
  slli t2, t1, 2
  add t2, t2, a0
  lw t3, 0(t2)
  addi t3, t3, 1
  sw t3, 0(t2)
  addi t1, t1, 1
  bne t1, t0, loop
spin:
  j spin
