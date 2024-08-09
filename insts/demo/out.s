
a.out：     文件格式 elf32-littleriscv


Disassembly of section .text:

000100b8 <main>:
   100b8:	00100793          	li	a5,1
   100bc:	00100513          	li	a0,1
   100c0:	01300693          	li	a3,19
   100c4:	0080006f          	j	100cc <main+0x14>
   100c8:	00070513          	mv	a0,a4
   100cc:	00f50733          	add	a4,a0,a5
   100d0:	00050793          	mv	a5,a0
   100d4:	fea6dae3          	bge	a3,a0,100c8 <main+0x10>
   100d8:	00008067          	ret
