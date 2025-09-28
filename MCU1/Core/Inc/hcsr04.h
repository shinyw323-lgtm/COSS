/*
 * hcsr04.h
 *
 *  Created on: Sep 27, 2025
 *      Author: shiny
 */

#ifndef __HCSR04_H__
#define __HCSR04_H__

#include "stm32f1xx.h"

// TIM3 초기화 (초음파 전용)
void TIM3_Init(void);

// 거리 측정 함수 (cm 단위)
float HCSR04_Read(void);

#endif /* __HCSR04_H__ */
