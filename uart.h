/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef UART_H
#define	UART_H

#include "buffer.h"
#include "timer.h"
#include <xc.h> 

#define UART_1 1
#define UART_2 2
#define FCY 72000000UL 
#define BAUDRATE 9600
#define BRGVAL ((FCY / BAUDRATE) / 16 - 1)

void UART_Init(unsigned char uart);
void UART_SendChar(unsigned char uart, char data);

// interrupt function declarations
extern void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void);
extern void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* UART_H */

