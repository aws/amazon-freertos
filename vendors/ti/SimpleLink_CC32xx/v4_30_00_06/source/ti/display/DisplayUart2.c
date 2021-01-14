/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* -----------------------------------------------------------------------------
 *  Includes
 * -----------------------------------------------------------------------------
 */
#include <ti/drivers/UART2.h>

#include <ti/display/Display.h>
#include <ti/display/DisplayUart2.h>

#include <string.h>

#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SystemP.h>

/* -----------------------------------------------------------------------------
 *  Constants and macros
 * -----------------------------------------------------------------------------
 */
#ifndef MIN
#  define MIN(n, m)    (((n) > (m)) ? (m) : (n))
#endif

#define DISPLAY_UART_ESC_INITIAL            "\x1b\x63"    /* Reset terminal */                \
                                            "\x1b[2J"     /* Clear screen   */                \
                                            "\x1b[10r"    /* Scrolling region from line 10 */ \
                                            "\x1b[11;1H"  /* Set initial cursor to line 11 */

#define DISPLAY_UART_ESC_MOVEPOS_FMT        "\x1b\x37"    /* Save cursor position */ \
                                            "\x1b[10r"    /* Retransmit scroll    */ \
                                            "\x1b[%d;%dH" /* Move cursor fmt str  */

#define DISPLAY_UART_ESC_RESTOREPOS         "\x1b\x38"    /* Restore saved cursor pos */

#define DISPLAY_UART_ESC_CLEAR_SCREEN       "\x1b[2J"     /* Clear screen       */
#define DISPLAY_UART_ESC_CLEAR_CUR_LEFT     "\x1b[1K"     /* Clear cursor left  */
#define DISPLAY_UART_ESC_CLEAR_CUR_RIGHT    "\x1b[0K"     /* Clear cursor right */
#define DISPLAY_UART_ESC_CLEAR_BOTH         "\x1b[2K"     /* Clear line         */
#define DISPLAY_UART_ESC_CLEARSEQ_LEN       4

/* -----------------------------------------------------------------------------
 *   Type definitions
 * -----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------
 *                           Local variables
 * -----------------------------------------------------------------------------
 */
/* Display function table for minimal UART implementation */
const Display_FxnTable DisplayUart2Min_fxnTable = {
    DisplayUart2Min_init,
    DisplayUart2Min_open,
    DisplayUart2Min_clear,
    DisplayUart2Min_clearLines,
    DisplayUart2Min_vprintf,
    DisplayUart2Min_close,
    DisplayUart2Min_control,
    DisplayUart2Min_getType,
};

/* Display function table for UART ANSI implementation */
const Display_FxnTable DisplayUart2Ansi_fxnTable = {
    DisplayUart2Ansi_init,
    DisplayUart2Ansi_open,
    DisplayUart2Ansi_clear,
    DisplayUart2Ansi_clearLines,
    DisplayUart2Ansi_vprintf,
    DisplayUart2Ansi_close,
    DisplayUart2Ansi_control,
    DisplayUart2Ansi_getType,
};

/* -----------------------------------------------------------------------------
 *                                          Functions
 * -----------------------------------------------------------------------------
 */

 /*!
 * @fn          DisplayUart2Min_init
 *
 * @brief       Does nothing.
 *
 * @return      void
 */
void DisplayUart2Min_init(Display_Handle handle)
{
}

 /*!
 * @fn          DisplayUart2Ansi_init
 *
 * @brief       Does nothing.
 *
 * @return      void
 */
void DisplayUart2Ansi_init(Display_Handle handle)
{
}

 /*!
 * @fn          DisplayUart2Min_open
 *
 * @brief       Initialize the UART transport
 *
 * @descr       Opens the UART index specified in the HWAttrs, and creates a
 *              mutex semaphore
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       params - display parameters
 *
 * @return      Pointer to Display_Config struct
 */
Display_Handle DisplayUart2Min_open(Display_Handle hDisplay,
                                Display_Params *params)
{
    DisplayUart2_HWAttrs *hwAttrs = (DisplayUart2_HWAttrs *)hDisplay->hwAttrs;
    DisplayUart2_Object  *object  = (DisplayUart2_Object  *)hDisplay->object;

    UART2_Params uartParams;
    UART2_Params_init(&uartParams);
    uartParams.baudRate  = hwAttrs->baudRate;
    uartParams.writeMode = UART2_Mode_BLOCKING;

    object->mutex = SemaphoreP_createBinary(1);
    if (object->mutex == NULL)
    {
        return NULL;
    }

    object->hUart = UART2_open(hwAttrs->uartIdx, &uartParams);
    if (NULL == object->hUart)
    {
        return NULL;
    }

    return hDisplay;
}

 /*!
 * @fn          DisplayUart2Ansi_open
 *
 * @brief       Initialize the UART transport
 *
 * @descr       Opens the UART index specified in the HWAttrs, and creates a
 *              mutex semaphore
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       params - display parameters
 *
 * @return      Pointer to Display_Config struct
 */
Display_Handle DisplayUart2Ansi_open(Display_Handle hDisplay,
                                     Display_Params *params)
{
    DisplayUart2_HWAttrs *hwAttrs = (DisplayUart2_HWAttrs *)hDisplay->hwAttrs;
    DisplayUart2_Object  *object  = (DisplayUart2_Object  *)hDisplay->object;

    UART2_Params uartParams;
    UART2_Params_init(&uartParams);
    uartParams.baudRate  = hwAttrs->baudRate;
    uartParams.writeMode = UART2_Mode_BLOCKING;

    object->mutex = SemaphoreP_createBinary(1);
    if (object->mutex == NULL)
    {
        return NULL;
    }

    object->hUart = UART2_open(hwAttrs->uartIdx, &uartParams);
    if (NULL == object->hUart)
    {
        return NULL;
    }

    switch (params->lineClearMode)
    {
        case DISPLAY_CLEAR_BOTH:  object->lineClearSeq = DISPLAY_UART_ESC_CLEAR_BOTH; break;
        case DISPLAY_CLEAR_LEFT:  object->lineClearSeq = DISPLAY_UART_ESC_CLEAR_CUR_LEFT; break;
        case DISPLAY_CLEAR_RIGHT: object->lineClearSeq = DISPLAY_UART_ESC_CLEAR_CUR_RIGHT; break;
        default: /* fall-through */
        case DISPLAY_CLEAR_NONE: object->lineClearSeq = NULL; break;
    }

    /* Send VT100 initial config to terminal */
    UART2_write(object->hUart, DISPLAY_UART_ESC_INITIAL, sizeof DISPLAY_UART_ESC_INITIAL - 1,
                NULL);

    return hDisplay;
}

/*!
 * @fn          DisplayUart2Min_clear
 *
 * @brief       Does nothing, as output is stateless.
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayUart2Min_clear(Display_Handle hDisplay)
{
}

/*!
 * @fn          DisplayUart2Ansi_clear
 *
 * @brief       Does nothing, as output is stateless.
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayUart2Ansi_clear(Display_Handle hDisplay)
{
    DisplayUart2_Object  *object  = (DisplayUart2_Object  *)hDisplay->object;
    DisplayUart2_HWAttrs *hwAttrs = (DisplayUart2_HWAttrs *)hDisplay->hwAttrs;

    if (SemaphoreP_pend(object->mutex, hwAttrs->mutexTimeout) == SemaphoreP_OK)
    {
        UART2_write(object->hUart, DISPLAY_UART_ESC_CLEAR_SCREEN, 4, NULL);
        SemaphoreP_post(object->mutex);
    }
}

/*!
 * @fn          DisplayUart2Min_clearLines
 *
 * @brief       Does nothing, as output is stateless.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       lineFrom - line index (0 .. )
 * @param       lineTo - line index (0 .. )
 *
 * @return      void
 */
void DisplayUart2Min_clearLines(Display_Handle hDisplay,
                               uint8_t lineFrom, uint8_t lineTo)
{
}

/*!
 * @fn          DisplayUart2Ansi_clearLines
 *
 * @brief       Does nothing, as output is stateless.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       lineFrom - line index (0 .. )
 * @param       lineTo - line index (0 .. )
 *
 * @return      void
 */
void DisplayUart2Ansi_clearLines(Display_Handle hDisplay,
                                uint8_t lineFrom, uint8_t lineTo)
{
    DisplayUart2_Object  *object  = (DisplayUart2_Object  *)hDisplay->object;
    DisplayUart2_HWAttrs *hwAttrs = (DisplayUart2_HWAttrs *)hDisplay->hwAttrs;

    uint32_t      strSize = 0;
    uint32_t      curLine = 0;
    const uint8_t uartClearLineMoveDown[] = "\x1b[2K\x1b\x45";

    if (lineTo <= lineFrom)
    {
        lineTo = lineFrom;
    }

    if (SemaphoreP_pend(object->mutex, hwAttrs->mutexTimeout) == SemaphoreP_OK)
    {
        strSize += SystemP_snprintf(hwAttrs->strBuf, hwAttrs->strBufLen,
                                   DISPLAY_UART_ESC_MOVEPOS_FMT, lineFrom + 1, 0);

        for (curLine = lineFrom + 1; curLine < lineTo + 2; curLine++)
        {
            memcpy(hwAttrs->strBuf + strSize, uartClearLineMoveDown, sizeof uartClearLineMoveDown - 1);
            strSize += sizeof uartClearLineMoveDown - 1;

            if (hwAttrs->strBufLen - strSize < sizeof DISPLAY_UART_ESC_RESTOREPOS - 1 + sizeof uartClearLineMoveDown - 1)
            {
                UART2_write(object->hUart, hwAttrs->strBuf, strSize,
                        NULL);
                strSize = 0;
            }
        }

        memcpy(hwAttrs->strBuf + strSize, DISPLAY_UART_ESC_RESTOREPOS, sizeof DISPLAY_UART_ESC_RESTOREPOS - 1);
        strSize += sizeof DISPLAY_UART_ESC_RESTOREPOS - 1;

        UART2_write(object->hUart, hwAttrs->strBuf, strSize, NULL);
        SemaphoreP_post(object->mutex);
    }
}


/*!
 * @fn          DisplayUart2Min_vprintf
 *
 * @brief       Write a text string to UART with return and newline.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       line - line index (0..)
 * @param       column - column index (0..)
 * @param       fmt - format string
 * @param       aN - optional format arguments
 *
 * @return      void
 */
void DisplayUart2Min_vprintf(Display_Handle hDisplay, uint8_t line,
                             uint8_t column, const char *fmt, va_list va)
{
    DisplayUart2_Object  *object  = (DisplayUart2_Object  *)hDisplay->object;
    DisplayUart2_HWAttrs *hwAttrs = (DisplayUart2_HWAttrs *)hDisplay->hwAttrs;

    uint32_t strSize = 0;

    if (SemaphoreP_pend(object->mutex, hwAttrs->mutexTimeout) == SemaphoreP_OK)
    {
        SystemP_vsnprintf(hwAttrs->strBuf, hwAttrs->strBufLen - 2, fmt, va);

        strSize = strlen(hwAttrs->strBuf);
        hwAttrs->strBuf[strSize++] = '\r';
        hwAttrs->strBuf[strSize++] = '\n';

        UART2_write(object->hUart, hwAttrs->strBuf, strSize, NULL);
        SemaphoreP_post(object->mutex);
    }
}

/*!
 * @fn          DisplayUart2Ansi_vprintf
 *
 * @brief       Write a text string to UART with return and newline.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       line - line index (0..)
 * @param       column - column index (0..)
 * @param       fmt - format string
 * @param       aN - optional format arguments
 *
 * @return      void
 */
void DisplayUart2Ansi_vprintf(Display_Handle hDisplay, uint8_t line,
                             uint8_t column, const char *fmt, va_list va)
{
    DisplayUart2_Object  *object  = (DisplayUart2_Object  *)hDisplay->object;
    DisplayUart2_HWAttrs *hwAttrs = (DisplayUart2_HWAttrs *)hDisplay->hwAttrs;

    uint32_t       strSize = 0;

    char          *strBuf = hwAttrs->strBuf;
    const uint16_t bufLen = hwAttrs->strBufLen;

    if (SemaphoreP_pend(object->mutex, hwAttrs->mutexTimeout) == SemaphoreP_OK)
    {
        if (line != DisplayUart2_SCROLLING)
        {
            /* Add cursor movement escape sequence */
            strSize += SystemP_snprintf(strBuf + strSize, bufLen - strSize - 2,
                                        DISPLAY_UART_ESC_MOVEPOS_FMT, line + 1, column + 1);

            /* Add line clearing escape sequence */
            if (object->lineClearSeq)
            {
                memcpy(strBuf + strSize, object->lineClearSeq, DISPLAY_UART_ESC_CLEARSEQ_LEN);
                strSize += DISPLAY_UART_ESC_CLEARSEQ_LEN;
            }
        }

        strSize += SystemP_vsnprintf(strBuf + strSize, bufLen - strSize - 2, fmt, va);

        if (line != DisplayUart2_SCROLLING)
        {
            memcpy(strBuf + strSize, DISPLAY_UART_ESC_RESTOREPOS, sizeof DISPLAY_UART_ESC_RESTOREPOS - 1);
            strSize += 2;
        }
        else
        {
            strBuf[strSize++] = '\r';
            strBuf[strSize++] = '\n';
        }

        UART2_write(object->hUart, strBuf, strSize, NULL);
        SemaphoreP_post(object->mutex);
    }
}

/*!
 * @fn          DisplayUart2Min_close
 *
 * @brief       Closes the UART handle
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayUart2Min_close(Display_Handle hDisplay)
{
    DisplayUart2_Object *object = (DisplayUart2_Object  *)hDisplay->object;

    UART2_close(object->hUart);
    object->hUart = NULL;

    // Not sure what happens if someone is pending
    SemaphoreP_delete(object->mutex);
}

/*!
 * @fn          DisplayUart2Ansi_close
 *
 * @brief       Closes the UART handle
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayUart2Ansi_close(Display_Handle hDisplay)
{
    DisplayUart2_Object *object = (DisplayUart2_Object  *)hDisplay->object;

    UART2_close(object->hUart);
    object->hUart = NULL;

    // Not sure what happens if someone is pending
    SemaphoreP_delete(object->mutex);
}

/*!
 * @fn          DisplayUart2Min_control
 *
 * @brief       Function for setting control parameters of the Display driver
 *              after it has been opened.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       cmd - command to execute
 * @param       arg - argument to the command
 *
 * @return      ::DISPLAY_STATUS_UNDEFINEDCMD because no commands are supported
 */
int DisplayUart2Min_control(Display_Handle hDisplay, unsigned int cmd, void *arg)
{
    return DISPLAY_STATUS_UNDEFINEDCMD;
}

/*!
 * @fn          DisplayUart2Ansi_control
 *
 * @brief       Function for setting control parameters of the Display driver
 *              after it has been opened.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       cmd - command to execute
 * @param       arg - argument to the command
 *
 * @return      ::DISPLAY_STATUS_UNDEFINEDCMD because no commands are supported
 */
int DisplayUart2Ansi_control(Display_Handle hDisplay, unsigned int cmd, void *arg)
{
    return DISPLAY_STATUS_UNDEFINEDCMD;
}

/*!
 * @fn          DisplayUart2Min_getType
 *
 * @brief       Returns type of transport
 *
 * @return      Display type UART
 */
unsigned int DisplayUart2Min_getType(void)
{
    return Display_Type_UART;
}

/*!
 * @fn          DisplayUart2Ansi_getType
 *
 * @brief       Returns type of transport
 *
 * @return      Display type UART
 */
unsigned int DisplayUart2Ansi_getType(void)
{
    return Display_Type_UART | Display_Type_ANSI;
}
