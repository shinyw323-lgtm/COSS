/*
 * myadcdma.h
 *
 *  Created on: Sep 27, 2025
 *      Author: LG
 */

#ifndef INC_MYADCDMA_H_
#define INC_MYADCDMA_H_

#define ADC_CHANNEL_NUM 11
#define HISTORY_SIZE 3
#include <stdbool.h>
#include "stm32h743xx.h"

typedef struct {
    uint16_t raw[ADC_CHANNEL_NUM];       // DMA로 들어오는 원본 ADC 값
    uint16_t offset[ADC_CHANNEL_NUM];    // 초기 보정값
    uint16_t value[ADC_CHANNEL_NUM];     // offset 보정 후 절대값
    float moveAvg[ADC_CHANNEL_NUM];   // 이동 평균 (현재는 현재값만 사용)
    float smooth[ADC_CHANNEL_NUM];    // 인접 평균 (후처리)
    uint8_t  initialized;                // offset 보정 완료 flag
    uint16_t history[ADC_CHANNEL_NUM][HISTORY_SIZE]; // 과거 N개의 값을 저장할 2차원 배열
    uint8_t  history_index;                          // 현재 저장할 위치를 가리키는 인덱스

    // Top3 결과
    struct {
        int first, second, third;
        float val1, val2, val3;
    } top3;

    // 시간 관리
    uint32_t lastPrint;
} SensorSystem;


// 다른 파일에서 접근할 수 있도록 extern으로 선언
extern SensorSystem sensors;
extern volatile uint8_t system_active; // main.c에 있는 변수를 사용하기 위해 extern 선언

// 함수 선언
void ADC_DMA_Init(void);
void ADC_Process_Data(void); // 인터럽트 핸들러에서 호출될 데이터 처리 함수


#endif /* INC_MYADCDMA_H_ */
