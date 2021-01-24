AS = sdas8051
CC = sdcc
LD = sdld
OBJCOPY = objcopy

CFLAGS = --Werror -mmcs51 --model-small
LDFLAGS = -muwx -M -Y
LDFLAGS += -b HOME=0xc006
LDFLAGS += -b CSEG=0xc006
LDFLAGS += -k /usr/share/sdcc/lib/small
LDFLAGS += -l libsdcc -l libint -l liblong

RELS = header.rel main.rel bitbang_uart.rel
CTR0_RELS = crt0/crtclear.rel crt0/crtstart.rel crt0/crtxclear.rel crt0/crtxinit.rel
BUILD_RELS = out/header.rel out/crtclear.rel out/crtstart.rel out/crtxclear.rel out/crtxinit.rel out/main.rel out/bitbang_uart.rel

.PHONY: all clean
all: out/puppet.bin

out/%.rel: %.c
	@mkdir -p out
	$(CC) $(CFLAGS) -c $< -o $@

out/%.rel: %.asm
	@mkdir -p out
	$(AS) -l -o -s $@ $<

out/%.rel: crt0/%.asm
	@mkdir -p out
	$(AS) -plosgff $@ $<

out/puppet.ihx: $(BUILD_RELS)
	$(LD) -i $@ $(LDFLAGS) $(BUILD_RELS)

out/puppet.bin: out/puppet.ihx
	$(OBJCOPY) -I ihex -O binary $< $@

clean:
	rm -rf out/*
