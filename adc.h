/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef ADC_H
#define	ADC_H

#include <xc.h> 
#include "timer.h"
#include "uart.h"

#define ADC_REF_VOLTAGE 3.3f
#define ADC_RESOLUTION 4095.0f

void ADC_Init_Battery(void);
void ADC_Init_IR(void);
void ADC_Init_Scan(void);

uint16_t ADC_ReadBattery(void);
uint16_t ADC_ReadIR(void);
void ADC_ReadBoth(uint16_t *ir_raw, uint16_t *bat_raw);

float ADC_ConvertIRVoltageToDistance(float voltage);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* ADC_H */

