/*
 * timer_custom.c
 *
 *  Created on: Sep 27, 2025
 *      Author: shiny
 */
#include "timer_custom.h"

void TIM2_PWM_Init(void) {
    // 1. GPIOA, TIM2 클럭 Enable
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 2. PA0 → Alternate Function Push-Pull
    GPIOA->CRL &= ~(0xF << (0 * 4));
    GPIOA->CRL |=  (0xB << (0 * 4));   // MODE=11 (50MHz), CNF=10 (AF Push-Pull)

    // 3. TIM2 설정
    TIM2->PSC = 8 - 1;        // 8MHz / 8 = 1MHZ 변경
    TIM2->ARR = 20000 - 1;     // 1MHz / 20000 = 50Hz
    TIM2->CCR1 = 10000;        // 50% duty (10ms)

    // 4. PWM 모드 설정
    TIM2->CCMR1 &= ~(0x7 << 4);
    TIM2->CCMR1 |=  (0x6 << 4);    // OC1M = 110 (PWM1 mode)
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM2->CCER  |= TIM_CCER_CC1E;  // CH1 enable

    // 5. 타이머 Enable
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_SetDuty(uint16_t duty) {
    TIM2->CCR1 = duty;
}

