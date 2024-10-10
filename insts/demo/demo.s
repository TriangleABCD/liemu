.globl _start

_start:
  li t0, 28
  li t1, 13
  mul a0, t0, t1
  li a7, 1
  ecall
  div a0, t0, t1
  li a7, 1
  ecall
  rem a0, t0, t1
  li a7, 1
  ecall

  li t0, 1145
  li t1, -14
  mul a0, t0, t1
  li a7, 1
  ecall
  div a0, t0, t1
  li a7, 1
  ecall
  rem a0, t0, t1
  li a7, 1
  ecall

  li t0, -5730
  li t1, 233
  mul a0, t0, t1
  li a7, 1
  ecall
  div a0, t0, t1
  li a7, 1
  ecall
  rem a0, t0, t1
  li a7, 1
  ecall

  li t0, -1919
  li t1, -810
  mul a0, t0, t1
  li a7, 1
  ecall
  div a0, t0, t1
  li a7, 1
  ecall
  rem a0, t0, t1
  li a7, 1
  ecall
