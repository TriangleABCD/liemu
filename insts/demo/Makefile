out.s: a.out
	riscv64-linux-gnu-objdump -d a.out > out.s

a.out: demo.c
	riscv64-linux-gnu-gcc demo.c -O3 -static --freestanding -nostdlib -march=rv32ima -mabi=ilp32 2>/dev/null

clean:
	rm -rf a.out out.s

.PHONY: clean
