.globl _start

_start:
  li a1, 1
  beq a0, a1, TRAP
  li a1, 2
  beq a0, a1, TRAP
  li a1, 3
  beq a0, a1, TRAP
  li a1, 4
  beq a0, a1, TRAP

TRAP:
  li a7, 0
  ecall
  add x0, x0, x0
  ecall
  add x0, x0, x0
  ecall
  add x0, x0, x0
  ecall

  mret
