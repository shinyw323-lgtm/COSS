/*
 * mypwm.c
 *
 *  Created on: Sep 27, 2025
 *      Author: LG
 */
#include "mypwm.h"

void PWM_Init(void)
{
    RCC->APB1LENR |= (1U << 2);   // TIM4EN: TIM4 클럭 활성화

    // PWM 주파수 50Hz 생성
    // Frequency = 64,000,000 / ((63 + 1) * (19999 + 1)) = 50 Hz
    TIM4->PSC = 63;    // Prescaler: 타이머 클럭을 64분주 (64MHz -> 1MHz)
    TIM4->ARR = 19999; // Auto-Reload Register: 카운터가 0~19999까지 셈 (20ms 주기)

    // 채널 1을 PWM 모드 1로 설정
    TIM4->CCMR1 &= ~(7U << 4); // OC1M 비트 초기화
    TIM4->CCMR1 |= (6U << 4); // '110': PWM mode 1
    TIM4->CCMR1 |= (1U << 3); // OC1PE: 출력 비교 프리로드 활성화

    // 초기 펄스 폭을 1500 (1.5ms, 서보모터 중앙)으로 설정
    TIM4->CCR1 = 1500;

    // 채널 1 출력 활성화
    TIM4->CCER |= (1U << 0);           // CC1E 비트 활성화
    // 타이머 카운터 시작
    TIM4->CR1 |= (1U << 0);            // CEN 비트 활성화
}

/**
 * @brief  서보모터의 펄스 폭을 설정합니다. (TIM4_CH1)
 * @retval None
 */
void PWM_SetPulse(uint16_t pulse) //듀티비 변경해주는 함수
{
    // ARR 값을 초과하지 않도록 방지 (최대 19999)
    if (pulse > TIM4->ARR) {
        pulse = TIM4->ARR;
    }
    TIM4->CCR1 = pulse; // CCR1 레지스터에 값을 써서 펄스 폭 직접 변경
}
