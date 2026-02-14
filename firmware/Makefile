MMCU    = atmega328p
DF_CPU  = 16000000UL

PREFIX  = avr-
CC      = $(PREFIX)gcc
CP      = $(PREFIX)objcopy

CFLAGS  = -Wall -mmcu=$(MMCU) -DF_CPU=$(DF_CPU) -Os
LDFLAGS = -mmcu=$(MMCU)

BUILDDIR = build
SOURCES  = src/main.c
OBJECTS  = $(SOURCES:src/%.c=$(BUILDDIR)/%.o)
TARGET   = $(BUILDDIR)/firmware.elf
HEX      = $(TARGET:.elf=.hex)

all: $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

upload: $(HEX)
	avrdude -c usbasp -p m328p -U flash:w:$<:i

$(HEX): $(TARGET)
	$(CP) -O ihex -R .eeprom $< $@

$(TARGET): $(BUILDDIR) $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILDDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

.PHONY: all upload clean
