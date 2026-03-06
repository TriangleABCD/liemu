CROSS_PREFIX := riscv64-unknown-elf-
AS      := $(CROSS_PREFIX)as
LD      := $(CROSS_PREFIX)ld
OBJCOPY := $(CROSS_PREFIX)objcopy

ASFLAGS := -march=rv32i -mabi=ilp32
LDFLAGS := -m elf32lriscv -T link.ld

ASM_SRCS := $(wildcard *.s)
BINARIES := $(ASM_SRCS:.s=.bin)

.PHONY: all clean
all: $(BINARIES)

%.bin: %.s
	@echo "Building $@ from $<"
	@$(AS) $(ASFLAGS) -o $@.tmp.o $<
	@$(LD) $(LDFLAGS) -Ttext=0x0 -o $@.tmp.elf $@.tmp.o \
		--oformat=elf32-littleriscv 2>/dev/null || \
		$(LD) $(LDFLAGS) -Ttext=0x0 -o $@.tmp.elf $@.tmp.o
	@$(OBJCOPY) -O binary $@.tmp.elf $@
	@rm -f $@.tmp.o $@.tmp.elf
	@echo "Generated $@"

clean:
	rm -f *.bin *.o *.elf *.tmp.*

list:
	@echo "ASM sources: $(ASM_SRCS)"
	@echo "Binaries to build: $(BINARIES)"
