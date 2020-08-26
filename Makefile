.PHONY: all clean

ifndef MCU
$(error Please define the target MCU: $${MCU})
endif
include etc/${MCU}.mk

O=build
DIRS=$(patsubst src/%,$(O)/%,$(shell find src -type d -printf "%P\n")) $(O)
CC=$(CROSS_COMPILE)gcc
OBJCOPY=$(CROSS_COMPILE)objcopy

CFLAGS+=-ffreestanding \
		-O0 -gdwarf -mthumb

OBJS=$(O)/entry.o \
	 $(O)/main.o

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

flash: $(O)/image.hex
	st-flash --format ihex write $(O)/image.hex

gdb: $(O)/image.elf
	./etc/gdb.sh
