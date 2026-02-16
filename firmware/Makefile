MMCU    = atmega328p

PREFIX  = avr-
CC      = $(PREFIX)gcc
CP      = $(PREFIX)objcopy

CFLAGS  = -Wall -mmcu=$(MMCU) -Isrc -Os
LDFLAGS = -mmcu=$(MMCU)

BUILDDIR = build
SOURCES  = src/main.c src/sys/timer.c
OBJECTS  = $(SOURCES:src/%.c=$(BUILDDIR)/%.o)
TARGET   = $(BUILDDIR)/firmware.elf
HEX      = $(TARGET:.elf=.hex)

all: $(TARGET)

size: $(TARGET)
	avr-size --mcu=$(MMCU) -C $<
        
upload: $(HEX)
	avrdude -c usbasp -p m328p -U flash:w:$<:i

$(HEX): $(TARGET)
	$(CP) -O ihex -R .eeprom $< $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILDDIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

.PHONY: all upload clean size
