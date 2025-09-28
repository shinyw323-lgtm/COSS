/*
 * myadcdma.c
 *
 *  Created on: Sep 27, 2025
 *      Author: LG
 */
#include "myadcdma.h"
#include "mypwm.h"      // 서보모터 제어를 위해 PWM 헤더 포함
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32h743xx.h"
#include "stm32h7xx_hal.h" //HAL_Delay 쓰기위해
// 실제 변수 정의
SensorSystem sensors;

// main.c에 정의된 system_active 변수를 참조하기 위한 extern 선언
extern volatile uint8_t system_active;


/* ============================================================================ */
/* 데이터 처리 함수들                */
/* ============================================================================ */

//홀센서의 모든값들이 임계값 이하일때 TRUE 반환
bool allBelowThreshold(float *arr, int size, int threshold) {
    for (int i = 0; i < size; i++) {
        if (arr[i] >= threshold) return false;
    }
    return true;
}

//자기로프가 없을때 홀센서의 값들을 저장하여 OFFSET보정 - 안하면 노이즈가 많이낌
void initADC(void) {
    for (int i = 0; i < ADC_CHANNEL_NUM; i++) {
        sensors.offset[i] = sensors.raw[i];
    }
    sensors.initialized = 1; //한번 실행 됐으면 sensors.initialized 1로 바뀌고 다시 안함
}

//이동평균 과거 값 2개를 배열에 저장하여 평균을 내줌 - 1.0ms 0.5ms 지금
static float moveAvg(int channel_index)
{
    float sum = 0;
    for (int i = 0; i < HISTORY_SIZE; i++) {
        sum += sensors.history[channel_index][i];
    }
    return sum / HISTORY_SIZE; //HISTORY_SIZE 3
}

//이동평균 취한 값을 인접한 두 배열의 갑과 평균 , 맨 왼쪽/오른쪽은 자기자신값을 한번더 더하고 평균
void smoothAvg(void) {
    for (int i = 0; i < ADC_CHANNEL_NUM; i++) {
        if (i == 0)
            sensors.smooth[i] = (sensors.moveAvg[i] * 2 + sensors.moveAvg[i + 1]) / 3.0f;
        else if (i == ADC_CHANNEL_NUM - 1)
            sensors.smooth[i] = (sensors.moveAvg[i] * 2 + sensors.moveAvg[i - 1]) / 3.0f;
        else
            sensors.smooth[i] = (sensors.moveAvg[i - 1] + sensors.moveAvg[i] + sensors.moveAvg[i + 1]) / 3.0f;
    }
}

//모든 평균을 취하고 깔끔해진 값에서 가장 높은 값과 Index를 top3까지 찾는 함수
void findTop3(void) {
    int max1 = -1, max2 = -1, max3 = -1;
    float v1 = -1, v2 = -1, v3 = -1;
    for (int i = 0; i < ADC_CHANNEL_NUM; i++) {
        float val = sensors.smooth[i];
        if (val > v1) {
            v3 = v2; max3 = max2;
            v2 = v1; max2 = max1;
            v1 = val; max1 = i;
        } else if (val > v2) {
            v3 = v2; max3 = max2;
            v2 = val; max2 = i;
        } else if (val > v3) {
            v3 = val; max3 = i;
        }
    }
    sensors.top3.first = max1;  sensors.top3.val1 = v1;
    sensors.top3.second = max2; sensors.top3.val2 = v2;
    sensors.top3.third = max3;  sensors.top3.val3 = v3;
}

/**
 * @brief
 */

void ADC_Process_Data(void)
{
    if (system_active){
    	// 첫 실행 시 오프셋 보정
    	if (!sensors.initialized) {
        initADC();
    	}
    	/*홀센서가 3.3V에서 중간값을 기준으로 N극이면 증가, S극이면 감소
    	->초기값(offset)을 빼고 절댓값 취하면 N,S 상관없이 자력만 생각*/
    	for (int i = 0; i < ADC_CHANNEL_NUM; i++) {
    		sensors.value[i] = abs((int16_t)sensors.raw[i] - sensors.offset[i]);
    		sensors.history[i][sensors.history_index] = sensors.value[i];
    	}
    	//이동평균
    	for (int i = 0; i < ADC_CHANNEL_NUM; i++) {
    		sensors.moveAvg[i] = moveAvg(i);
    	}
    	// 인덱스를 1 증가시키고, 버퍼 크기를 넘어가면 다시 0으로 돌아오게 (Circular Buffer)
    	sensors.history_index = (sensors.history_index + 1) % HISTORY_SIZE;
    	smoothAvg();
    	findTop3();
    	int center = 5; // 11개 센서의 중앙 인덱스
    	if (allBelowThreshold(sensors.smooth, ADC_CHANNEL_NUM, 10)){
    		PWM_SetPulse(1500); // 모든 값이 10 이하일 때 서보모터 중앙으로

    	}
    	else{
    		//Center 에서 오차 범위 만큼 서보모터 조향
    		float error = (float)sensors.top3.first - center;
    		int servo_pwm = 1500 + (int)(error * 80);
    		PWM_SetPulse(servo_pwm);
    	}
    }
    else{
    	PWM_SetPulse(1500); //system_active low이면 중앙정렬 (uart에서 stop)
    }
}


/* ============================================================================ */
/* ADC 및 DMA 하드웨어 초기화                              */
/* ============================================================================ */

/**
 * @brief
 */


void ADC_DMA_Init(void)
{
    //ADC 전용 클럭 선택
    RCC->D3CCIPR &= ~RCC_D3CCIPR_ADCSEL; //초기화
    RCC->D3CCIPR |= (2U << RCC_D3CCIPR_ADCSEL_Pos); // ADC 클럭 소스로 PER_CK 선택

    //버스 클럭 활성화
    RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;


    //ADC를 보정 및 활성화하기 전, 반드시 먼저 비활성화
    if (ADC1->CR & ADC_CR_ADEN)
    {
        ADC1->CR |= ADC_CR_ADDIS;
        while(ADC1->CR & ADC_CR_ADEN);
    }

    //Deep-power down 해제 및 ADC 전압 레귤레이터 활성화
    ADC1->CR &= ~ADC_CR_DEEPPWD;
    ADC1->CR |= ADC_CR_ADVREGEN;
    HAL_Delay(1);

    //ADC 보정(Calibration) 수행
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);

    //ADC 활성화 및 준비 대기
    ADC1->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY));

    //DMA 설정
    while (DMA1_Stream0->CR & DMA_SxCR_EN); //스트림이 켜져 있으면 끌 때까지 대기 (안 끄면 설정 변경 불가)
    DMAMUX1_Channel0->CCR = (9U << DMAMUX_CxCR_DMAREQ_ID_Pos);
    // DMAMUX1 채널 0에 ADC1 DMA 요청 ID 연결
    // RM0433 DMAMUX 표에서 ADC1 = DMA request ID 9

    DMA1_Stream0->CR = (0U << DMA_SxCR_DIR_Pos)   |// 전송 방향: Peripheral-to-Memory (0)
                       (1U << DMA_SxCR_CIRC_Pos)  |// Circular 모드: 버퍼가 끝나면 자동으로 처음부터 다시
                       (1U << DMA_SxCR_MINC_Pos)  |// Memory Increment: 메모리 주소 자동 증가
                       (1U << DMA_SxCR_PSIZE_Pos) |// Peripheral Size = 16bit (ADC DR가 16bit이므로)
                       (1U << DMA_SxCR_MSIZE_Pos) |// Memory Size = 16bit (수신 버퍼도 16bit 배열)
                       (2U << DMA_SxCR_PL_Pos)    |// Priority Level = High
                       (1U << DMA_SxCR_TCIE_Pos); // Transfer Complete Interrupt Enable
    DMA1_Stream0->NDTR = ADC_CHANNEL_NUM; // 전송할 항목 개수(ADC 채널 수 = 11)
    DMA1_Stream0->PAR = (uint32_t)&(ADC1->DR);// Peripheral Address: ADC1->DR 데이터 레지스터
    DMA1_Stream0->M0AR = (uint32_t)sensors.raw;// Memory Address: 변환 결과를 저장할 sensors.raw 배열

    //ADC 상세 설정

    ADC1->PCSEL = //채널이 ADC 변환에 사용될 수 있게 미리 선택
        (1U<<16)|(1U<<17)|(1U<<14)|(1U<<15)|
        (1U<<18)|(1U<<19)|(1U<<3 )|(1U<<7 )|
        (1U<<4 )|(1U<<8 )|(1U<<9 );

    ADC12_COMMON->CCR |= (1U << 18); // PRESC: ADC 클럭 분주비
    ADC1->CFGR = 0; // 초기화
    ADC1->CFGR |= (1U << 1)  | // DMACFG = 1 : DMA Circular Mode
                  (2U << 2)  | // RES = 010 : 12-bit Resolution (0~4095)
                  (1U << 10) | // EXTEN = 01 : Hardware trigger on rising edge
                  (11U << 5);  // EXTSEL = 01011 : TIM2_TRGO 선택
    ADC1->CFGR |= (1U << 0);   // DMAEN = 1: DMA 모드 활성화

    ADC1->SQR1 = ((11 - 1) << ADC_SQR1_L_Pos) //(변환할 채널 수 - 1)
               | (16U << ADC_SQR1_SQ1_Pos)  // PA0
               | (17U << ADC_SQR1_SQ2_Pos)  // PA1
               | (14U << ADC_SQR1_SQ3_Pos)  // PA2
               | (15U << ADC_SQR1_SQ4_Pos); // PA3

    ADC1->SQR2 = (18U << ADC_SQR2_SQ5_Pos)  // PA4
               | (19U << ADC_SQR2_SQ6_Pos)  // PA5
               | (3U  << ADC_SQR2_SQ7_Pos)  // PA6
               | (7U  << ADC_SQR2_SQ8_Pos)  // PA7
               | (4U  << ADC_SQR2_SQ9_Pos); // PC4

    ADC1->SQR3 = (8U  << ADC_SQR3_SQ10_Pos) // PC5
               | (9U  << ADC_SQR3_SQ11_Pos);// PB0
    ADC1->SMPR2 = 0xFFFFFFFF;
    ADC1->SMPR1 = 0xFFFFFFFF;


    DMA1_Stream0->CR |= DMA_SxCR_EN;
    NVIC_SetPriority(DMA1_Stream0_IRQn, 1);
    NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    ADC1->CR |= ADC_CR_ADSTART;

}

