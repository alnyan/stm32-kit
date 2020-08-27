.section .vectors, "a", %progbits
.type _vectors, %object
_vectors:
    .word __stack_top       // 0x00 Stack pointer
    .word _start            // 0x04 Reset
    .word 0                 // 0x08 NMI
    .word 0                 // 0x0C HardFault
    .word 0                 // 0x10 MemManage
    .word 0                 // 0x14 BusFault
    .word 0                 // 0x18 UsageFault
    .word 0                 // 0x1C -
    .word 0                 // 0x20 -
    .word 0                 // 0x24 -
    .word 0                 // 0x28 -

    // Specific to STM32F7xx
    .word 0                 // 0x002C   SVCall
    .word 0                 // 0x0030   Debug Monitor
    .word 0                 // 0x0034   -
    .word 0                 // 0x0038   PendSV
    .word irq_systick       // 0x003C   SysTick
    .word 0                 // 0x0040   WWDG
    .word 0                 // 0x0044   PVD
    .word 0                 // 0x0048   TAMP_STAMP
    .word 0                 // 0x004C   RTC_WKUP
    .word 0                 // 0x0050   FLASH
    .word 0                 // 0x0054   RCC
    .word 0                 // 0x0058   EXTI0
    .word 0                 // 0x005C   EXTI1
    .word 0                 // 0x0060   EXTI2
    .word 0                 // 0x0064   EXTI3
    .word 0                 // 0x0068   EXTI4
    .word 0                 // 0x006C   DMA1_Stream0
    .word 0                 // 0x0070   DMA1_Stream1
    .word 0                 // 0x0074   DMA1_Stream2
    .word 0                 // 0x0078   DMA1_Stream3
    .word 0                 // 0x007C   DMA1_Stream4
    .word 0                 // 0x0080   DMA1_Stream5
    .word 0                 // 0x0084   DMA1_Stream6
    .word 0                 // 0x0088   ADC
    .word 0                 // 0x008C   CAN1_TX
    .word 0                 // 0x0090   CAN1_RX0
    .word 0                 // 0x0094   CAN1_RX1
    .word 0                 // 0x0098   CAN1_SCE
    .word 0                 // 0x009C   EXTI9_5
    .word 0                 // 0x00A0   TIM1_BRK_TIM9
    .word 0                 // 0x00A4   TIM1_UP_TIM10
    .word 0                 // 0x00A8   TIM1_TRG_COM_TIM11
    .word 0                 // 0x00AC   TIM1_CC
    .word 0                 // 0x00B0   TIM2
    .word 0                 // 0x00B4   TIM3
    .word 0                 // 0x00B8   TIM4

.size _vectors, . - _vectors
