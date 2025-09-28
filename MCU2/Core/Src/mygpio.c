/*
 * mygpio.c
 *
 *  Created on: Sep 27, 2025
 *      Author: LG
 */
#include "mygpio.h"
#include "stm32h743xx.h"

void GPIO_Init(void)
{
    //  사용할 GPIO 포트의 클럭 활성화
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN; // GPIOA: ADC 핀 (PA0-7)
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN; // GPIOB: ADC(PB0), PWM(PB6)
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN; // GPIOC: ADC 핀 (PC4-5)
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN; // GPIOD: UART 핀

    //  PWM 출력 핀 설정 (TIM4_CH1 -> PB6)
    GPIOB->MODER   &= ~(3U << (6 * 2));  //초기화
    GPIOB->MODER   |=  (2U << (6 * 2));  //AF > timer 사용할예정
    GPIOB->OSPEEDR |=  (3U << (6 * 2));  //Very High Speed: PWM출력시 신호왜곡 줄이기
    GPIOB->AFR[0]  &= ~(0xF << (6 * 4)); //초기화
    GPIOB->AFR[0]  |=  (2U << (6 * 4));  //AF2 > timer2 사용

    // UART 통신 핀 설정 (USART2: PD6=RX)
    // PD6 (RX) 단방향 통신
    GPIOD->MODER   &= ~(3U << (6 * 2)); //초기화
    GPIOD->MODER   |=  (2U << (6 * 2)); // AF
    GPIOD->OSPEEDR |=  (3U << (6 * 2)); // Very High Speed
    GPIOD->AFR[0]  &= ~(0xF << (6 * 4)); //초기화
    GPIOD->AFR[0]  |=  (7U << (6 * 4));  // AF7 > USART2 RX 기능
    //PD5 (TX) 안쓰는데 없으면 UART안됨 ㅠㅠ
    GPIOD->MODER   &= ~(3U << (5 * 2));
    GPIOD->MODER   |=  (2U << (5 * 2));
    GPIOD->OSPEEDR |=  (3U << (5 * 2));
    GPIOD->AFR[0]  &= ~(0xF << (5 * 4));
    GPIOD->AFR[0]  |=  (7U << (5 * 4));

    // 11개 홀 센서 ADC 핀 설정
    // 모든 ADC 핀 아날로그 모드('11')

    // GPIOA PA0 ~ PA7, 8개 핀
    for (int i = 0; i <= 7; i++)
    {
        GPIOA->MODER |= (3U << (i * 2));  //ADC 아날로그모드
        GPIOA->PUPDR &= ~(3U << (i * 2));
    }

    // **GPIOB (PB0, 1개 핀)**
    GPIOB->MODER |= (3U << (0 * 2));
    GPIOB->PUPDR &= ~(3U << (0 * 2));

    // **GPIOC (PC4, PC5, 2개 핀)**
    GPIOC->MODER |= (3U << (4 * 2));
    GPIOC->PUPDR &= ~(3U << (4 * 2));
    GPIOC->MODER |= (3U << (5 * 2));
    GPIOC->PUPDR &= ~(3U << (5 * 2));


}
