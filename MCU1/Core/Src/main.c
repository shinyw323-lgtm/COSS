#include "stm32f1xx.h"
#include "gpio_custom.h"
#include "timer_custom.h"
#include "usart_reg.h"
#include "hcsr04.h"
#include <stdio.h>

volatile uint8_t motorOn = 0;  // 버튼으로 토글되는 모터 상태

// --- 인터럽트 ---
void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1 << 6)) {
        EXTI->PR |= (1 << 6);  // Pending 클리어

        for (volatile int i = 0; i < 50000; i++); // 채터링 방지

        if ((GPIOB->IDR & (1 << 6)) == 0) {  // 실제 버튼 눌림 확인
            motorOn = !motorOn;
            if (motorOn) USART1_SendString("START\n");
            else USART1_SendString("STOP\n");
        }
    }
}

int main(void)
{
    GPIO_Init_Custom();
    TIM2_PWM_Init();
    TIM3_Init();
    USART1_Init(115200);

    float distance = 0.0f;

    while (1)
    {
        distance = HCSR04_Read();

        if (distance > 0 && distance < 10.0f) {   // 가까우면 강제 STOP
            USART1_SendString("STOP\n");
            GPIOA->BSRR = (1 << 14);   // Buzzer ON
            motorOn = 0;               // 버튼 상태도 OFF로 리셋
        } else {
            GPIOA->BSRR = (1 << (14+16));  // Buzzer OFF
        }

        // 🔑 motorOn 값에 따라 모터 제어 (공통)
        if (motorOn) {
            GPIOB->BSRR = (1 << (0+16));  // PB0 = Low → 모터 ON
        } else {
            GPIOB->BSRR = (1 << 0);       // PB0 = High → 모터 OFF
        }

        for (volatile int i = 0; i < 60000; i++);
    }
}

