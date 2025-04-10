/*
 * File:   adc.c
 * Author: shady
 *
 * Created on April 8, 2025, 8:37 PM
 */


#include "xc.h"
#include "adc.h"

#define ADC_REF_VOLTAGE 3.3f
#define ADC_RESOLUTION 4095.0f
#define TIMER1 1
#define TIMER2 2

void ADC_Init_Battery(void) {
    AD1CON1bits.ADON = 0;
    ANSELBbits.ANSB11 = 1;
    AD1CON1bits.ASAM = 0;
    AD1CON1bits.SSRC = 0;
    AD1CON1bits.AD12B = 1;
    AD1CON1bits.FORM = 0;
    AD1CHS0bits.CH0SA = 11;
    AD1CON3bits.ADCS = 8;
    AD1CON1bits.ADON = 1;
}

void ADC_Init_IR(void) {
    AD1CON1bits.ADON = 0;
    ANSELBbits.ANSB2 = 1;
    AD1CON1bits.ASAM = 0;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.AD12B = 1;
    AD1CON1bits.FORM = 0;
    AD1CHS0bits.CH0SA = 2;
    AD1CON3bits.SAMC = 16;
    AD1CON3bits.ADCS = 8;
    AD1CON1bits.ADON = 1;
}

void ADC_Init_Scan(void) {
    AD1CON1bits.ADON = 0;
    ANSELBbits.ANSB2 = 1;
    ANSELBbits.ANSB11 = 1;
    AD1CON1bits.ASAM = 1;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.AD12B = 1;
    AD1CON1bits.FORM = 0;
    AD1CON2bits.CHPS = 0;
    AD1CON2bits.CSCNA = 1;
    AD1CON2bits.SMPI = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON3bits.ADCS = 8;
    AD1CSSL = 0;
    AD1CSSLbits.CSS2 = 1;
    AD1CSSLbits.CSS11 = 1;
    AD1CON1bits.ADON = 1;
}

uint16_t ADC_ReadBattery(void) {
    AD1CON1bits.DONE = 0;
    AD1CON1bits.SAMP = 1;
    tmr_wait_ms(TIMER1, 10);
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE);
    return ADC1BUF0;
}

uint16_t ADC_ReadIR(void) {
    AD1CON1bits.DONE = 0;
    AD1CON1bits.SAMP = 1;
    tmr_wait_ms(TIMER1, 10);
    while (!AD1CON1bits.DONE);
    return ADC1BUF0;
}

void ADC_ReadBoth(uint16_t *ir_raw, uint16_t *bat_raw) {
    while (!AD1CON1bits.DONE);
    *ir_raw = ADC1BUF0;
    *bat_raw = ADC1BUF1;
}

float ADC_ConvertIRVoltageToDistance(float v) {
    float v2 = v * v;
    float v3 = v2 * v;
    float v4 = v3 * v;
    return 2.34f - 4.74f * v + 4.06f * v2 - 1.60f * v3 + 0.24f * v4;
}
