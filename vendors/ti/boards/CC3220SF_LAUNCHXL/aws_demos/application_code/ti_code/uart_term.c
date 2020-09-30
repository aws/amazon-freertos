/*
 * 	Terminal
 */

// Standard includes
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include "uart_term.h"

// FreeRTOS includes
#include "FreeRTOS.h"

extern int vsnprintf (char * s, size_t n, const char * format, va_list arg );

//*****************************************************************************
//                          LOCAL DEFINES
//*****************************************************************************
#define IS_SPACE(x)       (x == 32 ? 1 : 0)

//*****************************************************************************
//                 GLOBAL VARIABLES
//*****************************************************************************
static UART_Handle      uartHandle;

//*****************************************************************************
//
//! Initialization
//!
//! This function
//!        1. Configures the UART to be used.
//!
//! \param  none
//!
//! \return none
//
//*****************************************************************************
UART_Handle InitTerm(void)
{

    UART_Params   		uartParams;

    UART_init();
    UART_Params_init(&uartParams);

    uartParams.writeDataMode    = UART_DATA_BINARY;
    uartParams.readDataMode     = UART_DATA_BINARY;
    uartParams.readReturnMode   = UART_RETURN_FULL;
    uartParams.readEcho         = UART_ECHO_OFF;
    uartParams.baudRate         = 115200;

    uartHandle = UART_open(Board_UART0, &uartParams);
    /* remove uart receive from LPDS dependency */
    UART_control(uartHandle, UART_CMD_RXDISABLE, NULL);

    return(uartHandle);
}

//*****************************************************************************
//
//! prints the formatted string on to the console
//!
//! \param[in]  format  - is a pointer to the character string specifying the
//!                       format in the following arguments need to be
//!                       interpreted.
//! \param[in]  [variable number of] arguments according to the format in the
//!             first parameters
//!
//! \return count of characters printed
//
//*****************************************************************************
int Report(const char *pcFormat, ...)
{
    int     iRet = 0;
    char        *pcBuff;
    char        *pcTemp;
    int         iSize = 256;
    va_list     list;


    pcBuff = (char*)pvPortMalloc(iSize);
    if(pcBuff == NULL)
    {
        return -1;
    }
    while(1)
    {
        va_start(list,pcFormat);
        iRet = vsnprintf(pcBuff, iSize, pcFormat, list);
        va_end(list);
        if((iRet > -1) && (iRet < iSize))
        {
            break;
        }
        else
        {
            // Reallocate a larger buffer
            pcTemp = pvPortMalloc(iSize * 2);

            if(pcTemp == NULL)
            {
                Message("Could not reallocate memory\n\r");
                iRet = -1;
                break;
            }
            else
            {
                // Copy data to larger buffer, then free smaller buffer
                memcpy(pcTemp, pcBuff, iSize);
                vPortFree(pcBuff);

                // Update buffer and size
                pcBuff = pcTemp;
                iSize *= 2;
            }
        }
    }
    Message(pcBuff);
    vPortFree(pcBuff);

    return iRet;
}

//*****************************************************************************
//
//! Trim the spaces from left and right end of given string
//!
//! \param  pcInput - string on which trimming happens
//!
//! \return length of trimmed string
//
//*****************************************************************************
int TrimSpace(char * pcInput)
{
    size_t      size;
    char        *endStr;
    char        *strData = pcInput;
    char        index = 0;


    size = strlen(strData);

    if (!size)
    {
        return 0;
    }

    endStr = strData + size - 1;
    while((endStr >= strData) && (IS_SPACE(*endStr)))
    {
        endStr--;
    }
    *(endStr + 1) = '\0';

    while(*strData && IS_SPACE(*strData))
    {
        strData++;
        index++;
    }
    memmove(pcInput, strData, strlen(strData) + 1);

    return strlen(pcInput);
}

//*****************************************************************************
//
//! Get the Command string from UART
//!
//! \param[in]  pucBuffer   - is the command store to which command will be
//!                           populated
//! \param[in]  ucBufLen    - is the length of buffer store available
//!
//! \return Length of the bytes received. -1 if buffer length exceeded.
//!
//*****************************************************************************
int GetCmd(char *pcBuffer, unsigned int uiBufLen)
{
    char    cChar;
    int     iLen = 0;


    UART_readPolling(uartHandle, &cChar, 1);

    iLen = 0;

    //
    // Checking the end of Command
    //
    while(1)
    {
        //
        // Handling overflow of buffer
        //
        if(iLen >= uiBufLen)
        {
            return -1;
        }

        //
        // Copying Data from UART into a buffer
        //
        if((cChar == '\r') || (cChar =='\n'))
        {
            UART_writePolling(uartHandle, &cChar, 1);
            break;
        }
        else if(cChar == '\b')
        {
            //
            // Deleting last character when you hit backspace
            //
            char    ch;

            UART_writePolling(uartHandle, &cChar, 1);
            ch = ' ';
            UART_writePolling(uartHandle, &ch, 1);
            if(iLen)
            {
                UART_writePolling(uartHandle, &cChar, 1);
                iLen--;
            }
            else
            {
                ch = '\a';
                UART_writePolling(uartHandle, &ch, 1);
            }
        }
        else
        {
            //
            // Echo the received character
            //
            UART_writePolling(uartHandle, &cChar, 1);

            *(pcBuffer + iLen) = cChar;
            iLen++;
        }

        UART_readPolling(uartHandle, &cChar, 1);
    }

    *(pcBuffer + iLen) = '\0';

    return iLen;
}

//*****************************************************************************
//
//! Outputs a character string to the console
//!
//! This function
//!        1. prints the input string character by character on to the console.
//!
//! \param[in]  str - is the pointer to the string to be printed
//!
//! \return none
//!
//! \note If UART_NONPOLLING defined in than Message or UART write should be
//!       called in task/thread context only.
//
//*****************************************************************************
void Message(const char *str)
{
#ifdef UART_NONPOLLING
    UART_write(uartHandle, str, strlen(str));
#else
    UART_writePolling(uartHandle, str, strlen(str));
#endif
}

//*****************************************************************************
//
//! Clear the console window
//!
//! This function
//!        1. clears the console window.
//!
//! \param  none
//!
//! \return none
//
//*****************************************************************************
void ClearTerm()
{
    Message("\33[2J\r");
}

//*****************************************************************************
//
//! Read a character from the console
//!
//! \param none
//!
//! \return Character
//
//*****************************************************************************
char getch(void)
{
  char  ch;


  UART_readPolling(uartHandle, &ch, 1);
  return ch;
}

//*****************************************************************************
//
//! Outputs a character to the console
//!
//! \param[in]  char    - A character to be printed
//!
//! \return none
//
//*****************************************************************************
void putch(char ch)
{
  UART_writePolling(uartHandle, &ch, 1);
}
