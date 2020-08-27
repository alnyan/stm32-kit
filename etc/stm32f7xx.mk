CFLAGS+=-mcpu=cortex-m7
LDFLAGS+=-nostdlib
LDSCRIPT=etc/stm32f7xx.ld

OBJS+=$(O)/stm32f7/usart.o
