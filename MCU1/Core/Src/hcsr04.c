/*
 * hcsr04.c
 *
 *  Created on: Sep 27, 2025
 *      Author: shiny
 */
#include "hcsr04.h"

#define TRIG_PIN   12   // PA12
#define ECHO_PIN   11   // PA11

// --- TIM3 초기화 (1us tick) ---
void TIM3_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;   // TIM3 클럭 Enable
    TIM3->PSC = 8 - 1;     // 8MHz / 64 = 1MHz (1 tick = 1us)
    TIM3->ARR = 0xFFFF;     // 최대값
    TIM3->CR1 |= TIM_CR1_CEN;
}

// --- µs 단위 Delay (TIM3 기반) ---
static void delay_us(uint16_t us) {
    TIM3->CNT = 0;
    while (TIM3->CNT < us);
}

// --- 초음파 센서 거리 측정 ---
float HCSR04_Read(void) {
    uint32_t t1 = 0, t2 = 0;

    // 1. TRIG = 10us High
    GPIOA->BSRR = (1 << TRIG_PIN);   // Set
    delay_us(10);
    GPIOA->BRR = (1 << TRIG_PIN);    // Reset

    // 2. ECHO Rising Edge 대기 (타임아웃 30ms)
    uint32_t timeout = 30000;
    while (!(GPIOA->IDR & (1 << ECHO_PIN))) {
        if (--timeout == 0) return -1.0f;  // Timeout
    }
    t1 = TIM3->CNT;   // Rising 순간 카운트 저장

    // 3. ECHO Falling Edge 대기 (타임아웃 30ms)
    timeout = 30000;
    while (GPIOA->IDR & (1 << ECHO_PIN)) {
        if (--timeout == 0) return -1.0f;  // Timeout
    }
    t2 = TIM3->CNT;   // Falling 순간 카운트 저장

    // 4. 펄스 길이 계산
    uint32_t width = (t2 >= t1) ? (t2 - t1) : (0xFFFF - t1 + t2);

    // 5. 거리 변환 (단위: cm)
    return (float)width * 0.017f;
}
