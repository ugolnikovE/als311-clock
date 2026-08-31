MMCU    = atmega328p

PREFIX  = avr-
CC      = $(PREFIX)gcc
CP      = $(PREFIX)objcopy

CFLAGS  = -Wall -mmcu=$(MMCU) -Iinclude -Os -g -MMD -MP
LDFLAGS = -mmcu=$(MMCU)

BUILDDIR = build
SOURCES  = src/main.c src/hal/systimer.c src/drivers/als311.c src/hal/i2c.c src/drivers/ds3231.c src/drivers/button.c src/app/clock.c
OBJECTS  = $(SOURCES:src/%.c=$(BUILDDIR)/%.o)
DEPS     = $(OBJECTS:.o=.d)
TARGET   = $(BUILDDIR)/firmware.elf
HEX      = $(TARGET:.elf=.hex)

all: $(TARGET) $(HEX)

size: $(TARGET)
	avr-size --mcu=$(MMCU) -C $<

upload: $(HEX)
	avrdude -c usbasp -p m328p -U flash:w:$<:i

$(HEX): $(TARGET)
	$(CP) -O ihex -R .eeprom $< $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILDDIR)/%.o: src/%.c Makefile
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

-include $(DEPS)

.PHONY: all upload clean size
