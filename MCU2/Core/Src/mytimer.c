/*
 * mytimer.c
 *
 *  Created on: Sep 28, 2025
 *      Author: LG
 */
#include "mytimer.h"
void TIM2_TRGO_Init(uint32_t ms)
{
    // TIM2 클럭 활성화
    RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;

    // 1 kHz 타이머 만들기 (1 ms tick)
    TIM2->PSC = (SystemCoreClock / 1000) - 1; // 1 kHz
    TIM2->ARR = ms;                           // ms 주기

    // TRGO=Update Event
    TIM2->CR2 &= ~(7U << 4);
    TIM2->CR2 |=  (2U << 4); // MMS=010: update event as TRGO
}

void TIM2_Start(void)
{
    TIM2->CR1 |= TIM_CR1_CEN; // Timer enable
}

