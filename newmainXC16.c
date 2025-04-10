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
#define ASSIGNMENT 3

char *patterns[] = {"LD1", "LD2"};

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
            send_uart_float_label("BAT", voltage);
            __delay_ms(1000);     
        }
    }

    while (ASSIGNMENT == IR_MANUAL_SAMPLE_AUTO) {
        ADC_Init_IR();
        while(1){
            uint16_t raw = ADC_ReadIR();
            float voltage = (raw / 4095.0f) * 3.3f;
            float distance_cm = ADC_ConvertIRVoltageToDistance(voltage);
            send_uart_float_label("IR", distance_cm);
            __delay_ms(500);
        }
    }

    while (ASSIGNMENT == SCAN_MODE_IR_BATTERY) {
        ADC_Init_Scan();
        while(1){
            uint16_t ir_raw = 0, bat_raw = 0;
            for (int i = 0; i < 100; i++) {
                __delay_ms(1);
                ADC_ReadBoth(&ir_raw, &bat_raw);
            }
            float ir_voltage = (ir_raw / 4095.0f) * 3.3f;
            float bat_voltage = (bat_raw / 4095.0f) * 3.3f * 2.0f;
            float distance_cm = ADC_ConvertIRVoltageToDistance(ir_voltage);
            send_uart_formatted(distance_cm, bat_voltage);
        }
    }

    return 0;
}
