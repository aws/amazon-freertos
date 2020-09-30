#ifndef __UART_IF_H__
#define __UART_IF_H__

// TI-Driver includes
#include <ti/drivers/UART.h>
#include <ti_drivers_config.h>

/* API */

UART_Handle UartTerm_Init(void);

int UartTerm_Report(const char *pcFormat, ...);

int UartTerm_TrimSpace(char * pcInput);

int UartTerm_GetCmd(char *pcBuffer, unsigned int uiBufLen);

void UartTerm_Message(const char *str);

void UartTerm_ClearTerm();

char UartTerm_getch(void);

void UartTerm_putch(char ch);

#endif // __UART_IF_H__
