CFLAGS += -O2 -Werror -Wextra

all: flasher i2c_vs9989

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

flasher: flasher.o vxis.o
	$(CC) $(CFLAGS) -o $@ flasher.o vxis.o

i2c_vs9989: i2c_vs9989.o vxis.o
	$(CC) $(CFLAGS) -o $@ i2c_vs9989.o vxis.o

clean:
	rm -f flasher i2c_vs9989 *.o
