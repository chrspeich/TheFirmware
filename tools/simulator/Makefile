CC=clang
CFLAGS=-ggdb -fblocks -Icortex-m0p -Iperipherals -Icore -std=c11 -Wall
LDFLAGS=-lev
SRC=core/mcu.c core/gdb.c core/elf.c peripherals/ram.c peripherals/flash.c peripherals/uart.c peripherals/unittest.c cortex-m0p/mcu.c simulator.c
OBJS=$(SRC:.c=.o)

simulator: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

.c.o:
	$(CC) -c $(CFLAGS) -MMD -MF $<.d -o $@ $<

clean:
	rm -f simulator $(OBJS) $(SRC:.c=.c.d)

-include $(SRC:.c=.c.d)
