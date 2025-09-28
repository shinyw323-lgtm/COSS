/*
 * timer_custom.h
 *
 *  Created on: Sep 27, 2025
 *      Author: shiny
 */

/*
 * timercustom.h
 *
 * Created on: Sep 27, 2025
 * Author: shiny
 */

#ifndef __TIMERCUSTOM_H__
#define __TIMERCUSTOM_H__

#include "stm32f1xx.h"   // STM32F103 레지스터 정의 헤더

// === 함수 프로토타입 ===

// TIM2 CH1(PA0) PWM 초기화 (50Hz 기본)
void TIM2_PWM_Init(void);

// 듀티 사이클 변경 (CCR1 값을 바꿔서 duty 조절)
void TIM2_SetDuty(uint16_t duty);

#endif /* __TIMERCUSTOM_H__ */

