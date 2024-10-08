.globl _start

_start:
  li a1, 1
  beq a0, a1, TRAP_1
  li a1, 2
  beq a0, a1, TRAP_2
  li a1, 3
  beq a0, a1, TRAP_3
  li a1, 4
  beq a0, a1, TRAP_4

TRAP_1:
  li a7, 1
  ecall
  add x0, x0, x0
  ecall
  j RET

TRAP_2:
  li a7, 1
  ecall
  addi t0, t0, 1
  addi t0, t0, -1
  ecall
  j RET

TRAP_3:
  li a7, 1
  ecall
  xori t1, t1, 0
  ecall
  j RET

TRAP_4:
  li a7, 1
  ecall
  andi t2, t2, 1
  ecall
  j RET

RET:
  mret
