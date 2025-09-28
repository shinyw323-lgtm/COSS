/*
 * myuart.h
 *
 *  Created on: Sep 27, 2025
 *      Author: LG
 */

#ifndef INC_MYUART_H_
#define INC_MYUART_H_

#include "stm32h743xx.h"
#include <stdbool.h> // bool 타입을 사용하기 위해 추가

#define UART_RX_BUFFER_SIZE 20

// main.c에서 접근해야 하는 변수들은 extern으로 선언
extern volatile bool command_received;
extern volatile uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
extern volatile uint8_t uart_rx_index;
// 함수 선언
void UART_Init(void);

#endif /* INC_MYUART_H_ */
