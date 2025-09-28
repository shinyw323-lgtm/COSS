/*
 * mytimer.h
 *
 *  Created on: Sep 28, 2025
 *      Author: LG
 */

#ifndef INC_MYTIMER_H_
#define INC_MYTIMER_H_

#include "stm32h743xx.h"

void TIM2_TRGO_Init(uint32_t ms);  // 주기(ms) 지정
void TIM2_Start(void);

#endif /* INC_MYTIMER_H_ */
