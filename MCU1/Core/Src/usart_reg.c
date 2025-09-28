/*
 * usart_reg.c
 *
 *  Created on: Sep 27, 2025
 *      Author: shiny
 */
#include "usart_reg.h"

void USART1_Init(uint32_t baudrate) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;    // GPIOA 클럭
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // USART1 클럭

    // PA9 = TX (AF Push-Pull)
    GPIOA->CRH &= ~(0xF << ((9 - 8) * 4));
    GPIOA->CRH |=  (0xB << ((9 - 8) * 4));  // PA9: MODE=11 (50MHz), CNF=10 (AF Push-Pull)

    uint32_t PCLK2 = 8000000; //클럭 8MHZ
    USART1->BRR = PCLK2 / baudrate; //baudrate 115200

    // 송신 Enable만
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;
}

void USART1_SendString(const char *str) {
    while (*str) {
        while (!(USART1->SR & USART_SR_TXE)); // 송신 가능 대기
        USART1->DR = *str++; //문자열 송신
    }
}
