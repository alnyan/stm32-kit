.PHONY: all clean

O=build
DIRS=$(patsubst %,$(O)/%,$(shell find src -type d -printf "%P\n")) $(O)
CC=$(CROSS_COMPILE)gcc
OBJCOPY=$(CROSS_COMPILE)objcopy

HDRS=$(shell find include -type f -name "*.h")
OBJS=$(O)/entry.o \
	 $(O)/vectors.o \
	 $(O)/main.o

CFLAGS=-ffreestanding \
		-O0 -gdwarf -mthumb \
		-Werror \
		-Wall \
		-Wextra \
		-Wpedantic \
		-Iinclude
LDFLAGS=

ifndef MCU
$(error Please define the target MCU: $${MCU})
endif
include etc/${MCU}.mk

all: $(DIRS) $(O)/image.hex

clean:
	rm -rf $(O)

$(DIRS):
	mkdir -p $@

$(O)/%.o: src/%.S $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(O)/%.o: src/%.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(O)/image.elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) -T$(LDSCRIPT) -o $@ $(OBJS)

$(O)/image.hex: $(O)/image.elf
	$(OBJCOPY) -O ihex $< $@

##

flash: $(DIRS) $(O)/image.hex
	st-flash --format ihex write $(O)/image.hex

gdb: $(DIRS) $(O)/image.elf
	./etc/gdb.sh
