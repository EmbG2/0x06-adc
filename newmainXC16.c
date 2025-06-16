/*
 * File:   newmainXC16.c
 * Author: shady
 *
 * Created on April 8, 2025, 8:21 PM
 */

#include "xc.h"
#include "adc.h"
#include "uart.h"
#include <stdio.h>
#include <libpic30.h>

#define BATTERY_MANUAL_SAMPLING 1
#define IR_MANUAL_SAMPLE_AUTO 2
#define SCAN_MODE_IR_BATTERY 3
#define ASSIGNMENT 2

char *patterns[] = {"LD1", "LD2"};
char buffer[64];

int main(void) {

    CircularBuffer main_buffer_1;
    CircularBuffer main_buffer_2;
    CircularBuffer sec_buffer_1;
    CircularBuffer sec_buffer_2;
    UART_Init(UART_1);
    buffer_init(&main_buffer_1, patterns, 2);
    buffer_init(&sec_buffer_1, patterns, 2);
    buffer_init(&main_buffer_2, patterns, 2);
    buffer_init(&sec_buffer_2, patterns, 2);

    if (ASSIGNMENT == BATTERY_MANUAL_SAMPLING) {
        ADC_Init_Battery();
        while(1){
            uint16_t raw = ADC_ReadBattery();
            float voltage = (raw / 4095.0f) * 3.3f * 2.0f;
            sprintf(buffer, "$BAT:%.2f", voltage);
            send_uart_string(UART_1, buffer);
            tmr_wait_ms_3(TIMER1, 10);
            if (transmit_buffer1.count > 0){
                IEC0bits.U1TXIE = 1;
            }
        }
    }

    while (ASSIGNMENT == IR_MANUAL_SAMPLE_AUTO) {
        ADC_Init_IR();
        while(1){
            uint16_t raw = ADC_ReadIR();
            float voltage = (raw / 4095.0f) * 3.3f;
            float distance_cm = ADC_ConvertIRVoltageToDistance(voltage);
            sprintf(buffer, "$IR:%.2f", distance_cm);
            send_uart_string(UART_1, buffer);
            tmr_wait_ms_3(TIMER1, 10);
            if (transmit_buffer1.count > 0){
                IEC0bits.U1TXIE = 1;
            }
        }
    }

    while (ASSIGNMENT == SCAN_MODE_IR_BATTERY) {
        ADC_Init_Scan();
        while(1){
            uint16_t ir_raw = 0, bat_raw = 0;
            for (int i = 0; i < 100; i++) {
                tmr_wait_ms_3(TIMER1, 10);
                ADC_ReadBoth(&ir_raw, &bat_raw);
            }
            float ir_voltage = (ir_raw / 4095.0f) * 3.3f;
            float bat_v = (bat_raw / 4095.0f) * 3.3f * 2.0f;
            float ir_cm = ADC_ConvertIRVoltageToDistance(ir_voltage);
            sprintf(buffer, "$SENS,%.2f,%.2f*\n", ir_cm, bat_v);
            send_uart_string(UART_1, buffer);
            if (transmit_buffer1.count > 0){
                IEC0bits.U1TXIE = 1;
            }
        }
    }

    return 0;
}
