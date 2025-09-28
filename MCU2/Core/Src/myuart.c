/*
 * myuart.c
 *
 *  Created on: Sep 27, 2025
 *      Author: LG
 */
#include "myuart.h"
#include <string.h>


volatile bool command_received = false;
volatile uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
volatile uint8_t uart_rx_index = 0;


void UART_Init(void)
{
    // 1. USART2 클럭 활성화 (APB1 버스)
    RCC->APB1LENR |= RCC_APB1LENR_USART2EN;


    // 2. Baud Rate 설정 (115200 bps)
    // BRR = f_CK / Baudrate = 64,000,000 / 115200 = 556
    USART2->BRR = 556;

    // 3. UART 설정 및 활성화
    // CR1 레지스터 설정:
    // UE: UART 활성화
    // RE: 수신 활성화
    // RXNEIE: 수신 인터럽트 활성화 (가장 중요!)
    USART2->CR1 |= (1U << 0) | (1U << 2) | (1U << 5);

    // 4. NVIC(Nested Vectored Interrupt Controller)에서 USART2 인터럽트 활성화
    NVIC_SetPriority(USART2_IRQn, 2); // 인터럽트 우선순위 설정 (0이 가장 높음)
    NVIC_EnableIRQ(USART2_IRQn);
}

