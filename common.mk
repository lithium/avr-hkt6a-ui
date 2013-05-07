#
# avr makefile defaults
#
#

check   = $(shell $(CC) $1 -c -xc /dev/null -o/dev/null 2>/dev/null && echo $1)

CC  = avr-gcc
OPTIM   = -Os -ffunction-sections $(call check,-fno-split-wide-types)
CFLAGS  = -g -Wall -I. $(DEBUG_FLAGS) $(INCLUDES) $(OPTIM) -DF_CPU=$(F_CPU)
LDFLAGS = -g -Wl,--relax,--gc-sections
MODULES = $(OBJECTS)
AVRDUDE = avrdude -c $(AVRDUDE_PROGRAMMER) -p $(AVRDUDE_PART) -P $(AVRDUDE_PORT)


.S.o:
	$(CC) $(CFLAGS) $(TARGET_ARCH) -x assembler-with-cpp -c $< -o $@


main.hex:

all:    main.hex

clean:
	rm -f main.elf $(MODULES)

clobber:    clean
	rm -f main.hex

main.elf:   $(MODULES)
	$(LINK.o) -o $@ $(MODULES)

main.hex:   main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

flash:      main.hex
	$(AVRDUDE) -U flash:w:main.hex


