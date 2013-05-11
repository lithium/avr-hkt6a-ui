
TARGET_ARCH	= -mmcu=atmega328p
# INCLUDES	= -Ilib
#DEBUG_FLAGS	= -DDEBUG_LEVEL=1
F_CPU		= 16000000


OBJECTS		= main.o lcd.o millis.o event.o screentable.o screen.o


AVRDUDE_PROGRAMMER = arduino
AVRDUDE_PART = m328p
AVRDUDE_PORT = /dev/tty.usbmodemfa1221


include common.mk

