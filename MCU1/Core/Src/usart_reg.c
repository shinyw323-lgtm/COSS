/*
 * usart_reg.c
 *
 *  Created on: Sep 27, 2025
 *      Author: shiny
 */
#include "usart_reg.h"

void USART1_Init(uint32_t baudrate) {
    // --- 클럭 Enable ---
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;    // GPIOA 클럭
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // USART1 클럭

    // --- PA9 = TX (AF Push-Pull), PA10 = RX (Floating input) ---
    GPIOA->CRH &= ~((0xF << ((9 - 8) * 4)) | (0xF << ((10 - 8) * 4)));
    GPIOA->CRH |=  (0xB << ((9 - 8) * 4));  // PA9: MODE=11 (50MHz), CNF=10 (AF Push-Pull)
    GPIOA->CRH |=  (0x4 << ((10 - 8) * 4)); // PA10: MODE=00, CNF=01 (Floating input)

    // --- 보레이트 설정 ---
    // APB2 클럭 = 8 MHz (PLL OFF 가정)
    uint32_t PCLK2 = 8000000;
    USART1->BRR = PCLK2 / baudrate;   // ex) 8000000 / 115200 ≈ 69

    // --- 송수신 Enable ---
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
}

void USART1_SendChar(char c) {
    while (!(USART1->SR & USART_SR_TXE)); // 송신 가능 대기
    USART1->DR = c;
}

void USART1_SendString(const char *str) {
    while (*str) {
        USART1_SendChar(*str++);
    }
}

char USART1_GetChar(void) {
    while (!(USART1->SR & USART_SR_RXNE)); // 수신 대기
    return (char)(USART1->DR & 0xFF);
}




