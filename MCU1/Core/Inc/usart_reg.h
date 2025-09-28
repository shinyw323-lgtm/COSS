/*
 * usart_reg.h
 *
 *  Created on: Sep 27, 2025
 *      Author: shiny
 */

#ifndef UART_REG_H
#define UART_REG_H

#include "stm32f1xx.h"

void USART1_Init(uint32_t baudrate);
void USART1_SendString(const char *str);

#endif
