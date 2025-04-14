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
    ANSELBbits.ANSB2 = 1;    // IR sensor connected to AN2
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
    ANSELBbits.ANSB2 = 1;    // IR connected to AN2
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
    AD1CSSLbits.CSS2 = 1;    // AN2
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

void send_uart_float_label(const char* label, float value) {
    char buffer[32];
    sprintf(buffer, "$%s:%.2f", label, value);
    char* ptr = buffer;
    while (*ptr) {
        UART_SendChar(UART_1, *ptr++);
    }
    UART_SendChar(UART_1, '\n');
}

void send_uart_hex_label(const char* label, uint16_t value) {
    // Send label
    UART_SendChar(UART_1, '$');
    while (*label) {
        UART_SendChar(UART_1, *label++);
    }
    UART_SendChar(UART_1, ':');

    // Send value as 4-digit hex (16-bit)
    for (int shift = 12; shift >= 0; shift -= 4) {
        uint8_t nibble = (value >> shift) & 0xF;
        char hex_char = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
        UART_SendChar(UART_1, hex_char);
    }

    UART_SendChar(UART_1, '*');
    UART_SendChar(UART_1, '\n');
}

void send_uart_formatted(float ir_cm, float bat_v) {
    char buffer[64];
    sprintf(buffer, "$SENS,%.2f,%.2f*\n", ir_cm, bat_v);
    char* ptr = buffer;
    while (*ptr) {
        UART_SendChar(UART_1, *ptr++);
    }
}

float ADC_ConvertIRVoltageToDistance(float v) {
    float v2 = v * v;
    float v3 = v2 * v;
    float v4 = v3 * v;
    return 2.34f - 4.74f * v + 4.06f * v2 - 1.60f * v3 + 0.24f * v4;
}
