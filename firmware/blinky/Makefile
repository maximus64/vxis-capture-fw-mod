MAIN_FILE=main.asm

.PHONY: all clean

all: blinky.bin

%.rel: %.asm
	sdas8051 -l -o -s $<

blinky.ihx: blinky.rel
	sdld -m -u -i $@ $<

blinky.bin: blinky.ihx
	objcopy -I ihex -O binary $< $@

clean:
	rm -f *.rel *.ihx *.lst *.bin *.map *.rst *.sym
