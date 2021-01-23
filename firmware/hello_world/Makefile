AS = sdas8051
CC = sdcc
LD = sdld
OBJCOPY = objcopy

CFLAGS = --Werror -mmcs51 --model-small
LDFLAGS = -muwx -M -Y
LDFLAGS += -b HOME=0xc006
LDFLAGS += -b CSEG=0xc006
LDFLAGS += -k /usr/bin/../share/sdcc/lib/small
LDFLAGS += -k /usr/share/sdcc/lib/small
LDFLAGS += -l libsdcc -l libint -l liblong -l libfloat

RELS = main.rel header.rel bitbang_uart.rel
CTR0_RELS = crt0/crtclear.rel crt0/crtstart.rel crt0/crtxinit.rel  crt0/crtxclear.rel
BUILD_RELS = $(patsubst %,out/%,$(RELS))
BUILD_RELS += $(patsubst crt0/%,out/%,$(CTR0_RELS))

.PHONY: all clean
all: out/hello.bin

out/%.rel: %.c
	@mkdir -p out
	$(CC) $(CFLAGS) -c $< -o $@

out/%.rel: %.asm
	@mkdir -p out
	$(AS) -l -o -s $@ $<

out/%.rel: crt0/%.asm
	@mkdir -p out
	$(AS) -plosgff $@ $<

out/hello.ihx: linker.lk $(BUILD_RELS)
	$(LD) $(LDFLAGS) -i $@ $(BUILD_RELS)

out/hello.bin: out/hello.ihx
	$(OBJCOPY) -I ihex -O binary $< $@

clean:
	rm -rf out/*
