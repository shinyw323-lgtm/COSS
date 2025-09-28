/*
 * mypwm.h
 *
 *  Created on: Sep 27, 2025
 *      Author: LG
 */

#ifndef INC_MYPWM_H_
#define INC_MYPWM_H_
#include "stm32h743xx.h"

void PWM_Init(void);
void PWM_SetPulse(uint16_t pulse);

#endif /* INC_MYPWM_H_ */
