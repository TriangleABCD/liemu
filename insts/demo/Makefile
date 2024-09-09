out.s: a.out
	riscv64-linux-gnu-objdump -d a.out > out.inst

a.out: demo.s
	riscv64-linux-gnu-as demo.s -march=rv32ima -mabi=ilp32 2>/dev/null

clean:
	rm -rf a.out out.inst

.PHONY: clean
