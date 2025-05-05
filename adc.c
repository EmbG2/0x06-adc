/*
 * File:   adc.c
 * Author: shady
 *
 * Created on April 8, 2025, 8:37 PM
 */


#include "xc.h"
#include "adc.h"

void ADC_Init_Battery(void) {
    AD1CON1bits.ADON = 0;    // stop ADC
    ANSELBbits.ANSB11 = 1;   // battery connected to AN11
    AD1CON1bits.ASAM = 0;    // manual start
    AD1CON1bits.SSRC = 0;    // manual end
    AD1CON1bits.AD12B = 1;   // 1-channel ADC operation
    AD1CON1bits.FORM = 0;    // integer in range 0-4095
    AD1CHS0bits.CH0SA = 11;  // selection of the analog pin
    AD1CON3bits.ADCS = 8;    // TAD = 8 * TCY
    AD1CON1bits.ADON = 1;    // start ADC
}

void ADC_Init_IR(void) {
    AD1CON1bits.ADON = 0;    // stop ADC
    ANSELBbits.ANSB5 = 1;    // IR sensor connected to AN5
    ANSELBbits.ANSB4 = 0;    // set the enable pin of the IR sensor as output
    LATBbits.LATB4 = 1;      // enable the IR sensor
    AD1CON1bits.ASAM = 0;    // manual start
    AD1CON3bits.SAMC = 16;   // sample time = 16 TAD
    AD1CON1bits.SSRC = 7;    // automatic end
    AD1CON1bits.AD12B = 1;   // 1-channel ADC operation
    AD1CON1bits.FORM = 0;    // integer in range 0-4095
    AD1CHS0bits.CH0SA = 2;   // selection of the analog pin
    AD1CON3bits.ADCS = 8;    // TAD = 8 * TCY
    AD1CON1bits.ADON = 1;    // start ADC
}

void ADC_Init_Scan(void) {
    AD1CON1bits.ADON = 0;    // stop ADC
    ANSELBbits.ANSB5 = 1;    // IR connected to AN5
    ANSELBbits.ANSB4 = 0;    // set the enable pin of the IR sensor as output
    LATBbits.LATB4 = 1;      // enable the IR sensor
    ANSELBbits.ANSB11 = 1;   // battery connected to AN11
    AD1CON1bits.ASAM = 1;    // automatic start
    AD1CON3bits.SAMC = 16;   // sample time = 16 TAD
    AD1CON1bits.SSRC = 7;    // automatic end
    AD1CON1bits.AD12B = 1;   // is it one channel or two channel?
    AD1CON1bits.FORM = 0;    // integer in range of 0-4095

    AD1CON2bits.CSCNA = 1;   // scan mode is enabled
    AD1CON2bits.CHPS = 0;    // scan one channel
    AD1CON2bits.SMPI = 1;    // means we scan 2 samples (N-1)
    AD1CON3bits.ADCS = 8;    // TAD = 8 * TCY
    AD1CSSL = 0;             // Don't scan all bits except:
    AD1CSSLbits.CSS5 = 1;    // AN5
    AD1CSSLbits.CSS11 = 1;   // AN11
    AD1CON1bits.ADON = 1;    // start ADC
}

uint16_t ADC_ReadBattery(void) {
    AD1CON1bits.SAMP = 1;     // begin the sampling since ASAM is 0
    tmr_wait_ms(TIMER1, 10);
    AD1CON1bits.SAMP = 0;     // stop the sampling
    while (!AD1CON1bits.DONE);
    uint16_t result = ADC1BUF0;
    AD1CON1bits.DONE = 0;
    return result;
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
