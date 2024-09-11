.globl main

main:
<<<<<<< HEAD
  addi a0, x0, 0
  addi a1, x0, 1
  addi t0, x0, 8
=======
  li a0, 0x80002400
  li a1, 8192
  li a2, 0
>>>>>>> b62ae0bbb192bcd83f70d34b771951d380d42386
loop:
  add a2, a0, a1
  addi a0, a1, 0
  addi a1, a2, 0
  addi t0, t0, -1
  bne t0, x0, loop
  add a0, x0, a1
  addi a7, x0, 1
  ecall
spin:
  j spin
