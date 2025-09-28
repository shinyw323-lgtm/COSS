/*
 * gpio_custom.c
 *
 *  Created on: Sep 27, 2025
 *      Author: shiny
 */
#include "gpio_custom.h"
#include "usart_reg.h"



void GPIO_Init_Custom(void) {
    // --- 클럭 활성화 ---
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;   // GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;   // AFIO
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_DISABLE; // PA13, PA14, PA15 GPIO 가능

    // --- PB0: enable (Output, 초기값 = 1 → 모터 정지) ---
    GPIOB->CRL &= ~(0xF << (0 * 4));
    GPIOB->CRL |=  (0x2 << (0 * 4));      // Output 2MHz, Push-Pull
    GPIOB->ODR |= (1 << 0);               // 초기값 HIGH = STOP

    // --- PB6: 버튼 입력 (Pull-up) ---
    GPIOB->CRL &= ~(0xF << (6 * 4));
    GPIOB->CRL |=  (0x8 << (6 * 4));        // Input with Pull-up/Down
    GPIOB->ODR |= (1 << 6);                 // 내부 Pull-up 활성화

    // --- EXTI6 ← PB6 ---
    AFIO->EXTICR[1] &= ~(0xF << 8);
    AFIO->EXTICR[1] |=  (0x1 << 8);

    // --- PA12: TRIG (Output, Push-Pull, LOW) ---
    GPIOA->CRH &= ~(0xF << ((12 - 8) * 4));
    GPIOA->CRH |=  (0x2 << ((12 - 8) * 4)); // Output 2MHz, Push-Pull
    GPIOA->ODR &= ~(1 << 12);               // 초기값 LOW

    // --- PA11: ECHO (Input, Pull-down) ---
    GPIOA->CRH &= ~(0xF << ((11 - 8) * 4));
    GPIOA->CRH |=  (0x8 << ((11 - 8) * 4)); // Input with Pull-up/Down
    GPIOA->ODR &= ~(1 << 11);               // Pull-down

    // --- PA14: buzzer (Output, 초기값 OFF) ---
    GPIOA->CRH &= ~(0xF << ((14 - 8) * 4));
    GPIOA->CRH |=  (0x2 << ((14 - 8) * 4)); // Output 2MHz, Push-Pull
    GPIOA->ODR &= ~(1 << 14);               // 초기값 LOW (OFF)

    // 버튼(PB6)을 눌렀을 때 Falling Edge 신호로 인터럽트 발생하도록 설정
    EXTI->IMR  |= (1 << 6);       // 인터럽트 요청 허용
    EXTI->FTSR |= (1 << 6);       // 눌렀을 때(Falling) 인터럽트 발생
    EXTI->RTSR &= ~(1 << 6);	  // Rising Edge는 비활성화
    NVIC_EnableIRQ(EXTI9_5_IRQn); // NVIC에 EXTI9_5 인터럽트 등록
}


