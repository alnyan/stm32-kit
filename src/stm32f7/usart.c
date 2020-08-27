#include "stm32f7/usart.h"

void usart_tx(volatile struct stm32_usart *usart, int ch) {
    while (!(usart->isr & (1 << 7)));
    usart->tdr = ch & 0xFF;
}
