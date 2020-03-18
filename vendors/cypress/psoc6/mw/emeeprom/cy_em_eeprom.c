/***************************************************************************//**
* \file cy_em_eeprom.c
* \version 2.0
*
* \brief
*  This file provides source code of the API for the Emulated EEPROM library.
*  The Emulated EEPROM API allows creating an emulated EEPROM in flash that
*  has the ability to do wear leveling and restore corrupted data from a
*  redundant copy.
*
********************************************************************************
* \copyright
* Copyright 2017-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <stddef.h>
#include <string.h>
#include "cy_em_eeprom.h"

/*******************************************************************************
* Private Function Prototypes
*******************************************************************************/
static cy_en_em_eeprom_status_t CheckRanges(
                const cy_stc_eeprom_config_t * config);
static uint8_t CalcChecksum(
                const uint8_t rowData[],
                uint32_t len);
static cy_en_em_eeprom_status_t WriteRow(
                const uint32_t * rowAddr,
                const uint32_t * rowData,
                const cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t EraseRow(
                const uint32_t * rowAddr,
                const uint32_t * ramBuffAddr,
                const cy_stc_eeprom_context_t * context);
static uint32_t CalculateRowChecksum(
                const uint32_t * ptrRow);
static uint32_t GetStoredRowChecksum(
                const uint32_t * ptrRow);
static uint32_t GetStoredSeqNum(
                const uint32_t * ptrRow);
static cy_en_em_eeprom_status_t CheckRowChecksum(
                const uint32_t * ptrRow);
static cy_en_em_eeprom_status_t DefineLastWrittenRow(
                cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t ReadSimpleMode(
                uint32_t addr,
                void * eepromData,
                uint32_t size,
                const cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t ReadExtendedMode(
                uint32_t addr,
                void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t WaitTillComplete(void);
static cy_en_em_eeprom_status_t WriteExtendedMode(
                uint32_t addr,
                const void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t WriteSimpleMode(
                uint32_t addr,
                const void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t CheckLastWrittenRowIntegrity(
                uint32_t * ptrSeqNum,
                cy_stc_eeprom_context_t * context);
static uint32_t * GetNextRowPointer(
                uint32_t * ptrRow,
                const cy_stc_eeprom_context_t * context);
static uint32_t * GetReadRowPointer(
                uint32_t * ptrRow,
                const cy_stc_eeprom_context_t * context);
static cy_en_em_eeprom_status_t CopyHistoricData(
                uint32_t * ptrRowWrite,
                uint32_t * ptrRow,
                const cy_stc_eeprom_context_t * context);
static void CopyHeadersData(
                uint32_t * ptrRowWrite,
                uint32_t * ptrRow,
                const cy_stc_eeprom_context_t * context);

/*******************************************************************************
* Function Name: Cy_Em_EEPROM_Init
****************************************************************************//**
*
* Initializes the Emulated EEPROM library by filling the context structure.
*
* This function is called by the application program prior to calling any
* other function of the Em_EEPROM middleware.
*
* Do not modify the context structure after it is filled with this function.
* Modification of the context structure may cause unexpected behavior
* of the Cy_Em_EEPROM functions that rely on this context structure.
*
* This function does the following:
* 1. Checks the provided configuration for correctness.
* 2. Copies the provided configuration into the context structure.
*
* \param config
* The pointer to the configuration structure. See \ref cy_stc_eeprom_config_t.
*
* \param context
* The pointer to the Em_EEPROM context structure to be filled by the function.
* See \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the status of the operation \ref cy_en_em_eeprom_status_t :
* - CY_EM_EEPROM_SUCCESS   - The operation is successfully completed.
* - CY_EM_EEPROM_BAD_PARAM - The input parameter is invalid.
* - CY_EM_EEPROM_BAD_DATA  - The configuration is incorrect.
*
*******************************************************************************/
cy_en_em_eeprom_status_t Cy_Em_EEPROM_Init(
                const cy_stc_eeprom_config_t * config,
                cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_PARAM;

    if ((NULL != context) && (NULL != config))
    {
        ret = CheckRanges(config);

        if (CY_EM_EEPROM_SUCCESS == ret)
        {
            /* Copies the user's config structure fields into the context */
            context->eepromSize = config->eepromSize;
            context->simpleMode = config->simpleMode;
            if (0u != context->simpleMode)
            {
                context->wearLevelingFactor = 1u;
                context->redundantCopy = 0u;
            }
            else
            {
                context->wearLevelingFactor = config->wearLevelingFactor;
                context->redundantCopy = config->redundantCopy;
            }
            context->blockingWrite = config->blockingWrite;
            context->userFlashStartAddr = config->userFlashStartAddr;

            /* Stores frequently used data for internal use */
            context->numberOfRows = CY_EM_EEPROM_GET_NUM_ROWS_IN_EEPROM(context->eepromSize, context->simpleMode);
            context->byteInRow = CY_EM_EEPROM_EEPROM_DATA_LEN(context->simpleMode);
        }
    }
    return(ret);
}


/*******************************************************************************
* Function Name: Cy_Em_EEPROM_Read
****************************************************************************//**
*
* Reads data from a specified location. This function takes a logical
* Em_EEPROM address, converts it to an actual physical address where
* data is stored and returns the data to the user.
*
* This function uses a buffer of the flash row size to perform the read
* operation. For the size of the row, refer to the specific PSoC device
* datasheet.
*
* There are restrictions on using the read-while-write (RWW) feature for
* EEPROM emulation. There are also multiple constraints for blocking and
* nonblocking flash operations, relating to interrupts, power modes,
* IPC usage, etc. Refer to the "Flash (Flash System Routine)" section of
* the PSoC 6 Peripheral Driver Library (psoc6pdl) API Reference Manual.<br>
*
* \param addr
* The logical start address in the Em_EEPROM storage to start reading data from.
*
* \param eepromData
* The pointer to a user array to write data to.
*
* \param size
* The amount of data to read in bytes.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the status of the operation \ref cy_en_em_eeprom_status_t :
* - CY_EM_EEPROM_SUCCESS      - The operation is successfully completed.
* - CY_EM_EEPROM_BAD_CHECKSUM - One of the row the data read from contains bad checksum.
* - CY_EM_EEPROM_REDUNDANT_COPY_USED - Some data were read from redundant copy.
* - CY_EM_EEPROM_BAD_PARAM    - The function input parameter is invalid.
*
*******************************************************************************/
cy_en_em_eeprom_status_t Cy_Em_EEPROM_Read(
                uint32_t addr,
                void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_PARAM;

    /* Validates the input parameters */
    if ((0u != size) && ((addr + size) <= (context->eepromSize)) && (NULL != eepromData))
    {
        if (0u != context->simpleMode)
        {
            ret = ReadSimpleMode(addr, eepromData, size, (const cy_stc_eeprom_context_t *)context);
        }
        else
        {
            ret = ReadExtendedMode(addr, eepromData, size, context);
        }
    }

    return(ret);
}


/*******************************************************************************
* Function Name: ReadSimpleMode
****************************************************************************//**
*
* Reads data from a specified location in Simple mode only.
*
* \param addr
* The logical start address in the Em_EEPROM storage to start reading data from.
*
* \param eepromData
* The pointer to a user array to write data to.
*
* \param size
* The amount of data to read in bytes.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* This function returns \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t ReadSimpleMode(
                uint32_t addr,
                void * eepromData,
                uint32_t size,
                const cy_stc_eeprom_context_t * context)
{
    /* Copies data to the user's buffer */
    (void)memcpy(eepromData, (const void *)(context->userFlashStartAddr + addr), size);

    return (CY_EM_EEPROM_SUCCESS);
}


/*******************************************************************************
* Function Name: ReadExtendedMode
****************************************************************************//**
*
* Reads data from a specified location when Simple Mode is disabled.
*
* \param addr
* The logical start address in the Em_EEPROM storage to start reading data from.
*
* \param eepromData
* The pointer to a user array to write data to.
*
* \param size
* The amount of data to read in bytes.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* This function returns \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t ReadExtendedMode(
                uint32_t addr,
                void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_SUCCESS;
    cy_en_em_eeprom_status_t retHistoricCrc;
    uint32_t i;
    uint32_t k;
    uint32_t numRowReads;
    uint32_t seqNum;
    uint32_t sizeToCopy;
    uint32_t sizeRemaining;
    uint32_t userBufferAddr;
    uint32_t * ptrRow;
    uint32_t * ptrRowWork;
    uint32_t curRowOffset;
    uint32_t strHistAddr;
    uint32_t endHistAddr;
    uint32_t currentAddr;
    uint32_t strHeadAddr;
    uint32_t endHeadAddr;
    uint32_t dstOffset;
    uint32_t srcOffset;
    cy_en_em_eeprom_status_t crcStatus;
    uint32_t numReads = context->numberOfRows;

    /* 1. Clears the user buffer */
    (void)memset(eepromData, 0, size);

    /* 2. Ensures the last written row is correct */
    (void)CheckLastWrittenRowIntegrity(&seqNum, context);

    /* 3. Reads relevant historic data into user's buffer */
    currentAddr = addr;
    sizeRemaining = size;
    userBufferAddr = (uint32_t)eepromData;
    numRowReads = ((((addr + size) - 1u) / context->byteInRow) - (addr / context->byteInRow)) + 1u;

    ptrRow = (uint32_t *)(context->userFlashStartAddr + (CY_EM_EEPROM_FLASH_SIZEOF_ROW * (addr / context->byteInRow)));

    for (i = 0u; i < numRowReads; i++)
    {
        if (1u < context->wearLevelingFactor)
        {
            /* Jumps to the active sector if wear leveling is enabled */
            ptrRow = GetReadRowPointer(context->ptrLastWrittenRow, context);
            /* Finds a row in the active sector with a relevant historic data address */
            for (k = 0u; k < context->numberOfRows; k++)
            {
                ptrRow = GetNextRowPointer(ptrRow, context);
                strHistAddr = ((((uint32_t)ptrRow - context->userFlashStartAddr) /
                            CY_EM_EEPROM_FLASH_SIZEOF_ROW) % context->numberOfRows) *
                            (CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u);
                endHistAddr = strHistAddr + (CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u);
                if ((currentAddr >= strHistAddr) && (currentAddr < endHistAddr))
                {
                    /* The row with needed address is found */
                    break;
                }
            }
        }

        curRowOffset = context->byteInRow + (currentAddr % context->byteInRow);
        sizeToCopy = context->byteInRow - (currentAddr % context->byteInRow);
        if (i >= (numRowReads - 1u))
        {
            sizeToCopy = sizeRemaining;
        }

        retHistoricCrc = CY_EM_EEPROM_SUCCESS;
        if (CY_EM_EEPROM_SUCCESS != CheckRowChecksum(ptrRow))
        {
            /* CRC is bad. Checks if the redundant copy if enabled */
            retHistoricCrc = CY_EM_EEPROM_BAD_CHECKSUM;
            if (0u != context->redundantCopy)
            {
                ptrRow += ((context->numberOfRows * context->wearLevelingFactor) *
                        CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32);
                if (CY_EM_EEPROM_SUCCESS == CheckRowChecksum(ptrRow))
                {
                    retHistoricCrc = CY_EM_EEPROM_REDUNDANT_COPY_USED;
                }
            }
        }
        /* If the correct CRC is found, then copies the data to the user's buffer */
        if (CY_EM_EEPROM_BAD_CHECKSUM != retHistoricCrc)
        {
            (void)memcpy((void *)userBufferAddr, (const void *)((uint32_t)ptrRow + curRowOffset), sizeToCopy);
        }
        else
        {
            (void)memset((void *)userBufferAddr, 0, sizeToCopy);
            if ((0u == GetStoredSeqNum(ptrRow)) && (0u == GetStoredRowChecksum(ptrRow)))
            {
                /*
                * Considers a row with a bad checksum as the row never that has never been
                * written before if the sequence number and CRC values are zeros.
                * In this case, reports zeros and does not
                * report the failed status of the write operation.
                */
                retHistoricCrc = CY_EM_EEPROM_SUCCESS;
            }
        }


        if (1u >= context->wearLevelingFactor)
        {
            ptrRow = GetNextRowPointer(ptrRow, context);
        }

        sizeRemaining -= sizeToCopy;
        currentAddr += sizeToCopy;
        userBufferAddr += sizeToCopy;

        /* Reports the status of the CRC verification in the following order:
        * The highest priority: CY_EM_EEPROM_BAD_CHECKSUM
        *                       CY_EM_EEPROM_REDUNDANT_COPY_USED
        * The lowest priority:  CY_EM_EEPROM_SUCCESS
        */
        if (CY_EM_EEPROM_BAD_CHECKSUM == retHistoricCrc)
        {
            ret = CY_EM_EEPROM_BAD_CHECKSUM;
        }
        if (CY_EM_EEPROM_SUCCESS == ret)
        {
            ret = retHistoricCrc;
        }
    }

    /* 4. Reads data from all active headers */
    ptrRow = GetReadRowPointer(context->ptrLastWrittenRow, context);
    userBufferAddr = (uint32_t)eepromData;
    for (i = 0u; i < numReads; i++)
    {
        ptrRow = GetNextRowPointer(ptrRow, context);
        ptrRowWork = ptrRow;
        /* Checks CRC of the row to be read except the last row of a recently created header */
        crcStatus = CheckRowChecksum(ptrRowWork);
        if ((CY_EM_EEPROM_SUCCESS != crcStatus) && (0u != context->redundantCopy))
        {
            /* Calculates the redundant copy pointer */
            ptrRowWork += ((context->numberOfRows * context->wearLevelingFactor) *
                CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32);
            crcStatus = CheckRowChecksum(ptrRowWork);
        }

        /* Skips the row if CRC is bad */
        if (CY_EM_EEPROM_SUCCESS == crcStatus)
        {
            /* The address of header data */
            strHeadAddr = ptrRowWork[CY_EM_EEPROM_HEADER_ADDR_OFFSET_U32];
            endHeadAddr = strHeadAddr + ptrRowWork[CY_EM_EEPROM_HEADER_LEN_OFFSET_U32];

            /* Skips the row if the header data address is out of the user's requested address range */
            if ((strHeadAddr < (addr + size)) && (endHeadAddr > addr))
            {
                dstOffset = (strHeadAddr > addr) ? (strHeadAddr - addr): (0u);
                srcOffset = (strHeadAddr > addr) ? (0u) : (addr - strHeadAddr);
                /* Calculates the number of bytes to be read from the current row's EEPROM header */
                sizeToCopy = (strHeadAddr > addr) ? (strHeadAddr) : (addr);
                sizeToCopy = ((endHeadAddr < (addr + size)) ? endHeadAddr : (addr + size)) - sizeToCopy;

                /* Reads from the memory and writes to the buffer */
                (void)memcpy((void *)((uint32_t)eepromData + dstOffset),
                             (const void *)((uint32_t)ptrRowWork + srcOffset + CY_EM_EEPROM_HEADER_DATA_OFFSET),
                             sizeToCopy);
            }
        }
    }

    return(ret);
}


/*******************************************************************************
* Function Name: Cy_Em_EEPROM_Write
****************************************************************************//**
*
* Writes data to a specified location. This function takes the
* logical Em_EEPROM address and converts it to an actual physical address
* and writes data there. If wear leveling is enabled, the writing process
* uses the wear leveling techniques.
*
* This is a blocking function and it does not return until the write
* operation is completed. The user's application program cannot enter
* Hibernate mode until the write is completed.
* The write operation is allowed in CPU Sleep and System Deep Sleep modes.
* Do not reset your device during the flash operation, including the XRES pin,
* a software reset, and watchdog reset sources. Also, configure low-voltage
* detect circuits to generate an interrupt instead of a reset. Otherwise,
* portions of flash may undergo unexpected changes.
*
* This function uses a buffer of the flash row size to perform the write
* operation. For the size of the row, refer to the specific PSoC device
* datasheet.
*
* If the blocking write option is used, and write or erase operations are
* performed by CM4, the user's code on CM0P and CM4 is blocked until the
* operations are completed. If the operations are performed by CM0P, the
* user's code on CM4 is not blocked and the user code's on CM0P is blocked
* until the operation is completed. Plan your task allocation accordingly.
*
* There are restrictions on using the read-while-write (RWW) feature for
* EEPROM emulation. There are also multiple constraints for blocking and
* nonblocking flash operations, relating to interrupts, Power modes,
* IPC usage, etc. Refer to the "Flash (Flash System Routine)" section of
* the PSoC 6 Peripheral Driver Library (psoc6pdl) API Reference Manual.<br>
*
* \param addr
* The logical start address in the Em_EEPROM storage to start writing data to.
*
* \param eepromData
* Data to write to Em_EEPROM.
*
* \param size
* The amount of data to write to Em_EEPROM in bytes.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the status of the operation \ref cy_en_em_eeprom_status_t :
* - CY_EM_EEPROM_SUCCESS      - The operation is successfully completed.
* - CY_EM_EEPROM_BAD_CHECKSUM - One of the row the data read from contains bad checksum.
* - CY_EM_EEPROM_REDUNDANT_COPY_USED - Some data were read from redundant copy.
* - CY_EM_EEPROM_WRITE_FAIL   - The write operation is failed.
* - CY_EM_EEPROM_BAD_PARAM    - The function input parameter is invalid.
*
*******************************************************************************/
cy_en_em_eeprom_status_t Cy_Em_EEPROM_Write(
                uint32_t addr,
                const void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_PARAM;

    /* Checks if the Em_EEPROM data does not exceed the Em_EEPROM capacity */
    if ((0u != size) && ((addr + size) <= (context->eepromSize)) && (NULL != eepromData))
    {
        if (0u != context->simpleMode)
        {
            ret = WriteSimpleMode(addr, eepromData, size, context);
        }
        else
        {
            ret = WriteExtendedMode(addr, eepromData, size, context);
        }
    }
    return(ret);
}


/*******************************************************************************
* Function Name: WriteSimpleMode
****************************************************************************//**
*
* Writes data to a specified location in Simple Mode only.
*
* \param addr
* The logical start address in the Em_EEPROM storage to start writing data to.
*
* \param eepromData
* Data to write to Em_EEPROM.
*
* \param size
* The amount of data to write to Em_EEPROM in bytes.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* This function returns \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t WriteSimpleMode(
                uint32_t addr,
                const void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret;
    uint32_t wrCnt = 0u;
    uint32_t numBytes;
    uint32_t startAddr = addr % CY_EM_EEPROM_FLASH_SIZEOF_ROW;
    uint32_t numWrites = (((size + startAddr) - 1u) / CY_EM_EEPROM_FLASH_SIZEOF_ROW) + 1u;
    uint32_t * ptrRow = (uint32_t *)(context->userFlashStartAddr + (addr - startAddr));
    uint32_t ptrUserData = (uint32_t)eepromData;
    uint32_t writeRamBuffer[CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32];

    do
    {
        /* Fills the RAM buffer with flash data for the case when not a whole row is requested to be overwritten */
        (void)memcpy((void *)&writeRamBuffer[0u], (const void *)ptrRow, CY_EM_EEPROM_FLASH_SIZEOF_ROW);
        /* Calculates the number of bytes to be written into the current row */
        numBytes = CY_EM_EEPROM_FLASH_SIZEOF_ROW - startAddr;
        if (numBytes > size)
        {
            numBytes = size;
        }
        /* Overwrites the RAM buffer with new data */
        (void)memcpy((void *)((uint32_t)&writeRamBuffer[0u] + startAddr), (const void *)ptrUserData, numBytes);

        /* Writes data to the specified flash row */
        ret = WriteRow(ptrRow, &writeRamBuffer[0u], context);

        if (CY_EM_EEPROM_SUCCESS == ret)
        {
            context->ptrLastWrittenRow = ptrRow;
        }
        else
        {
            break;
        }
        /* Update pointers for the next row to be written if any */
        startAddr = 0u;
        size -= numBytes;
        ptrUserData += numBytes;
        ptrRow += CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32;
        wrCnt++;
    }
    while (wrCnt < numWrites);

    return(ret);
}


/*******************************************************************************
* Function Name: WriteExtendedMode
****************************************************************************//**
*
* Writes data to a specified location when Simple Mode is disabled.
*
* \param addr
* The logical start address in the Em_EEPROM storage to start writing data to.
*
* \param eepromData
* Data to write to Em_EEPROM.
*
* \param size
* The amount of data to write to Em_EEPROM in bytes.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* This function returns \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t WriteExtendedMode(
                uint32_t addr,
                const void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_SUCCESS;
    cy_en_em_eeprom_status_t retWriteRow = CY_EM_EEPROM_SUCCESS;
    uint32_t wrCnt;
    uint32_t seqNum;
    uint32_t * ptrRow;
    uint32_t * ptrRowCopy;
    uint32_t writeRamBuffer[CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32];
    uint32_t ptrUserData = (uint32_t)eepromData;
    uint32_t numWrites = ((size - 1u) / CY_EM_EEPROM_HEADER_DATA_LEN) + 1u;

    /* Checks CRC of the last written row and find the last written row if the CRC is broken */
    (void)CheckLastWrittenRowIntegrity(&seqNum, context);
    ptrRow = context->ptrLastWrittenRow;

    for (wrCnt = 0u; wrCnt < numWrites; wrCnt++)
    {
        ptrRow = GetNextRowPointer(ptrRow, context);
        seqNum++;

        /* 1. Clears the RAM buffer */
        (void)memset(&writeRamBuffer[0u], 0, CY_EM_EEPROM_FLASH_SIZEOF_ROW);

        /* 2. Fills the EM_EEPROM service header info */
        writeRamBuffer[CY_EM_EEPROM_HEADER_SEQ_NUM_OFFSET_U32] = seqNum;
        writeRamBuffer[CY_EM_EEPROM_HEADER_ADDR_OFFSET_U32] = addr;
        writeRamBuffer[CY_EM_EEPROM_HEADER_LEN_OFFSET_U32] = CY_EM_EEPROM_HEADER_DATA_LEN;
        if (wrCnt == (numWrites - 1u))
        {
            /* Fills in the remaining size if this is the last row to write */
            writeRamBuffer[CY_EM_EEPROM_HEADER_LEN_OFFSET_U32] = size;
        }

        /* 3. Writes the user's data to the buffer */
        (void)memcpy((void *)&writeRamBuffer[CY_EM_EEPROM_HEADER_DATA_OFFSET_U32],
                     (const void *)ptrUserData,
                     writeRamBuffer[CY_EM_EEPROM_HEADER_LEN_OFFSET_U32]);

        /* 4. Writes the historic data to the buffer */
        ret = CopyHistoricData(&writeRamBuffer[0u], ptrRow, context);

        /* 5. Writes the data from other headers */
        CopyHeadersData(&writeRamBuffer[0u], ptrRow, context);

        /* 6. Calculates a checksum */
        writeRamBuffer[CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET_U32] = CalculateRowChecksum(&writeRamBuffer[0u]);

        /* 7. Writes data to the specified flash row */
        retWriteRow = WriteRow(ptrRow, &writeRamBuffer[0u], context);
        if ((CY_EM_EEPROM_SUCCESS == retWriteRow) && (0u != context->redundantCopy))
        {
			/* Writes data to the specified flash row in the redundant copy area */
			ptrRowCopy = ptrRow + ((context->numberOfRows * context->wearLevelingFactor) * CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32);
			retWriteRow = WriteRow(ptrRowCopy, &writeRamBuffer[0u], context);
        }

        if (CY_EM_EEPROM_SUCCESS == retWriteRow)
        {
            context->ptrLastWrittenRow = ptrRow;
        }
        else
        {
            break;
        }

        /* Switches to the next row */
        size -= CY_EM_EEPROM_HEADER_DATA_LEN;
        addr += CY_EM_EEPROM_HEADER_DATA_LEN;
        ptrUserData += CY_EM_EEPROM_HEADER_DATA_LEN;
    }

    if (CY_EM_EEPROM_SUCCESS != retWriteRow)
    {
    	ret = retWriteRow;
    }

    return(ret);
}


/*******************************************************************************
* Function Name: Cy_Em_EEPROM_Erase
****************************************************************************//**
*
* This function erases the entire content of Em_EEPROM.
*
* Erased values are all zeros.
*
* In the \ref section_em_eeprom_mode_simple the function just erases the
* entire content of Em_EEPROM.
*
* When Simple Mode is disabled, the function first performs one write
* operation to keep the number of writes completed so far and then erases
* all rest content of Em_EEPROM memory. So, the Cy_Em_EEPROM_NumWrites()
* function returns the correct value.
* Refer to the Cy_Em_EEPROM_Write() function for the side effect of the
* write operation. If the write operation is failed then corresponding
* status is returned and no erase operation is executed.
*
* This function uses a buffer of the flash row size to perform the erase
* operation. For the size of the row, refer to the specific PSoC device
* datasheet.
*
* This is a blocking function and it does not return until the erase
* operation is completed. The user's application program cannot enter
* Hibernate mode until the write is completed.
* The write operation is allowed in CPU Sleep and System Deep Sleep modes.
* Do not reset your device during the flash operation, including the XRES pin,
* a software reset, and watchdog reset sources. Also, configure low-voltage
* detect circuits to generate an interrupt instead of a reset. Otherwise,
* portions of flash may undergo unexpected changes.
*
* If the blocking write option is used, and write or erase operations are
* performed by CM4, the user's code on CM0P and CM4 is blocked until the
* operations are completed. If the operations are performed by CM0P, the
* user's code on CM4 is not blocked and the user code's on CM0P is blocked
* until the operation is completed. Plan your task allocation accordingly.
*
* There are restrictions on using the read-while-write (RWW) feature for
* EEPROM emulation. There are also multiple constraints for blocking and
* nonblocking flash operations, relating to interrupts, power mode,
* IPC usage, etc. Refer to the "Flash (Flash System Routine)" section of
* the PSoC 6 Peripheral Driver Library (psoc6pdl) API Reference Manual.<br>
* Also, refer to the \ref section_em_eeprom_miscellaneous section for
* the different Em_EEPROM middleware restrictions and limitations.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the status of the operation \ref cy_en_em_eeprom_status_t :
* - CY_EM_EEPROM_SUCCESS    - The operation is successfully completed.
* - CY_EM_EEPROM_WRITE_FAIL - The either Write or Erase operation is failed.
*
*******************************************************************************/
cy_en_em_eeprom_status_t Cy_Em_EEPROM_Erase(cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_SUCCESS;
    cy_en_em_eeprom_status_t retStatus;
    uint32_t i;
    uint32_t * ptrRow;
    uint32_t * ptrRowCopy;
    uint32_t seqNum;
    uint32_t writeRamBuffer[CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32] = {0u};
    uint32_t numRows = context->numberOfRows * context->wearLevelingFactor;

    if (0u != context->simpleMode)
    {
        ptrRow = (uint32_t *)context->userFlashStartAddr;
        for (i = 0u; i < numRows; i++)
        {
            retStatus = EraseRow(ptrRow, &writeRamBuffer[0u], context);
            if (CY_EM_EEPROM_SUCCESS == ret)
            {
                ret = retStatus;
            }
            ptrRow += CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32;
        }
    }
    else
    {
        /* Checks CRC if it is really the last row and the max sequence number */
        (void)CheckLastWrittenRowIntegrity(&seqNum, context);

        /* Gets the last written row pointer */
        ptrRow = context->ptrLastWrittenRow;
        ptrRow = GetNextRowPointer(ptrRow, context);

        /* Prepares a zero buffer with a sequence number and checksum */
        writeRamBuffer[CY_EM_EEPROM_HEADER_SEQ_NUM_OFFSET_U32] = seqNum + 1u;
        writeRamBuffer[CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET_U32] = CalculateRowChecksum(&writeRamBuffer[0u]);

        /* Performs writing */
        ret = WriteRow(ptrRow, &writeRamBuffer[0u], context);
        /* Duplicates writing into a redundant copy if enabled */
        if (0u != context->redundantCopy)
        {
            ptrRowCopy = ptrRow + (numRows * CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32);
            retStatus = WriteRow(ptrRowCopy, &writeRamBuffer[0u], context);
            if (CY_EM_EEPROM_SUCCESS == ret)
            {
                ret = retStatus;
            }
        }

        /* If the write operation is unsuccessful, skip erasing Em_EEPROM */
        if (CY_EM_EEPROM_SUCCESS == ret)
        {
            context->ptrLastWrittenRow = ptrRow;
            /* One row is already overwritten, so reduces the number of rows to be erased by one */
            for (i = 0u; i < (numRows - 1u); i++)
            {
                ptrRow = GetNextRowPointer(ptrRow, context);
                retStatus = EraseRow(ptrRow, &writeRamBuffer[0u], context);
                if (CY_EM_EEPROM_SUCCESS == ret)
                {
                    ret = retStatus;
                }
                /* Erases the redundant copy if enabled */
                if (0u != context->redundantCopy)
                {
                    ptrRowCopy = ptrRow + (numRows * CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32);
                    retStatus = EraseRow(ptrRowCopy, &writeRamBuffer[0u], context);
                    if (CY_EM_EEPROM_SUCCESS == ret)
                    {
                        ret = retStatus;
                    }
                }
            }
        }
    }

    return(ret);
}


/*******************************************************************************
* Function Name: Cy_Em_EEPROM_NumWrites
****************************************************************************//**
*
* Returns the number of the Em_EEPROM Writes completed so far.
*
* This function returns zero in the \ref section_em_eeprom_mode_simple since
* the number of writes is not available in this case.
*
* Use this function to monitor the flash memory endurance. The higher
* the value, the less the flash memory endurance.
* Refer to the datasheet write endurance spec to determine the maximum
* number of write cycles. If the Wear Leveling feature is enabled, then the
* maximum of write cycles is increased by
* \ref cy_stc_eeprom_config_t.wearLevelingFactor.
*
* The returned number does not include the number of writes into the
* redundant copy if enabled.
*
* This function does the following:
* 1. Returns the number of writes stored in the the last written row if the
*    checksum of the row is correct.
* 2. If the checksum of the last written row is invalid and the redundant copy
*    feature is enabled, then the row checksum of the redundant copy is verified
*    for correctness and corresponding number of writes is returned.
* 3. If the checksum of the last written row is incorrect, then a special
*    algorithm of searching the last written is executed and a corresponding
*    number of writes is reported.
* 4. If the algorithm is unable to find the last written row (there is no row
*    with the correct checksum), then zero is returned.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* The number of writes performed to the Em_EEPROM.
*
*******************************************************************************/
uint32_t Cy_Em_EEPROM_NumWrites(cy_stc_eeprom_context_t * context)
{
    uint32_t seqNum;

    (void)CheckLastWrittenRowIntegrity(&seqNum, context);

    return(seqNum);
}


/*******************************************************************************
* Function Name: CalcChecksum
****************************************************************************//**
*
* Implements CRC-8 used in the checksum calculation for the redundant copy
* algorithm.
*
* \param rowData
* The row data to be used to calculate the checksum.
*
* \param len
* The length of rowData.
*
* \return
* The calculated value of CRC-8.
*
*******************************************************************************/
static uint8_t CalcChecksum(const uint8_t rowData[], uint32_t len)
{
    uint8_t crc = CY_EM_EEPROM_CRC8_SEED;
    uint8_t i;
    uint16_t cnt = 0u;

    while(cnt != len)
    {
        crc ^= rowData[cnt];
        for (i = 0u; i < CY_EM_EEPROM_CRC8_POLYNOM_LEN; i++)
        {
            crc = CY_EM_EEPROM_CALCULATE_CRC8(crc);
        }
        cnt++;
    }

    return (crc);
}


/*******************************************************************************
* Function Name: CheckRanges
****************************************************************************//**
*
* Checks the provided configuration for parameter validness and size of
* the requested flash.
*
* \param config
* The pointer to a configuration structure. See \ref cy_stc_eeprom_config_t.
*
* \return
* Returns the status of operation. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t CheckRanges(const cy_stc_eeprom_config_t * config)
{
    uint32_t startAddr;
    uint32_t endAddr;
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_DATA;
    const cy_stc_eeprom_config_t * cfg = config;

    /* Checks the parameter validness */
    if ((0u != cfg->userFlashStartAddr) &&
        (0u != cfg->eepromSize) &&
        (1u >= cfg->simpleMode) &&
        (1u >= cfg->blockingWrite) &&
        (1u >= cfg->redundantCopy) &&
        (0u < cfg->wearLevelingFactor) &&
        (CY_EM_EEPROM_MAX_WEAR_LEVELING_FACTOR >= cfg->wearLevelingFactor))
    {
        /* Checks the flash size and location */
        startAddr = cfg->userFlashStartAddr;
        endAddr = startAddr + CY_EM_EEPROM_GET_PHYSICAL_SIZE(cfg->eepromSize, cfg->simpleMode, cfg->wearLevelingFactor, (uint32_t)cfg->redundantCopy);

        /* Range checks if there is enough flash for Em_EEPROM */
        if (CY_EM_EEPROM_IS_IN_FLASH_RANGE(startAddr, endAddr))
        {
            ret = CY_EM_EEPROM_SUCCESS;
        }
    }
    return (ret);
}


/*******************************************************************************
* Function Name: WriteRow
****************************************************************************//**
*
* Writes one flash row starting from the specified row address.
*
* \param rowAdd
* The pointer to the flash row.
*
* \param rowData
* The pointer to the data to be written to the row.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the status of operation. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t WriteRow(
                const uint32_t * rowAddr,
                const uint32_t * rowData,
                const cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_WRITE_FAIL;

    if (0u != context->blockingWrite)
    {
        /* Does the blocking write */
        if (CY_FLASH_DRV_SUCCESS == Cy_Flash_WriteRow((uint32_t)rowAddr, rowData))
        {
            ret = CY_EM_EEPROM_SUCCESS;
        }
    }
    else
    {
        /* Initiates the write */
        if (CY_FLASH_DRV_OPERATION_STARTED == Cy_Flash_StartWrite((uint32_t)rowAddr, rowData))
        {
            ret = WaitTillComplete();
        }
    }

    return (ret);
}


/*******************************************************************************
* Function Name: EraseRow
****************************************************************************//**
*
* Erases one flash row starting from the specified row address. If the redundant
* copy option is enabled, the corresponding row in the redundant copy will also
* be erased.
*
* \param rowAdd
* The pointer of the flash row.
*
* \param ramBuffAddr
* The address of the RAM buffer that contains zeroed data.
* Reserved for compatibility.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the status of operation. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t EraseRow(
                const uint32_t * rowAddr,
                const uint32_t * ramBuffAddr,
                const cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_WRITE_FAIL;

    (void)ramBuffAddr; /* To avoid the compiler warning */

    if (0u != context->blockingWrite)
    {
        /* Erases the flash row */
        if (CY_FLASH_DRV_SUCCESS == Cy_Flash_EraseRow((uint32_t)rowAddr))
        {
            ret = CY_EM_EEPROM_SUCCESS;
        }
    }
    else
    {
        /* Initiates the erase */
        if (CY_FLASH_DRV_OPERATION_STARTED == Cy_Flash_StartErase((uint32_t)rowAddr))
        {
            ret = WaitTillComplete();
        }
    }

    return(ret);
}


/*******************************************************************************
* Function Name: WaitTillComplete
****************************************************************************//**
*
* Waits until the erase or write operation is completed.
* The software watchdog counter is added to prevent the firmware.
* Returns \ref CY_EM_EEPROM_WRITE_FAIL if the watchdog timeout occurred,
* otherwise returns \ref CY_EM_EEPROM_SUCCESS.
*
* \return
* Returns the status of operation. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t WaitTillComplete(void)
{
    cy_en_flashdrv_status_t rc;
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_WRITE_FAIL;
    uint32_t countMs = CY_EM_EEPROM_MAX_WRITE_DURATION_MS;

    do
    {
        /* Waits 1ms */
    	Cy_SysLib_Delay(1u);
        /* Checks if the erase is completed */
        rc = Cy_Flash_IsWriteComplete();
        countMs--;
    }
    while ((rc == CY_FLASH_DRV_OPCODE_BUSY) && (0u != countMs));

    if (CY_FLASH_DRV_SUCCESS == rc)
    {
        ret = CY_EM_EEPROM_SUCCESS;
    }

    return(ret);
}


/*******************************************************************************
* Function Name: CalculateRowChecksum
****************************************************************************//**
*
* Calculates a checksum of the row specified by the ptrRow parameter.
* The first four bytes of the row are overwritten with zeros for calculation
* since it is the checksum location. The size of the row is defined by
* the \ref CY_EM_EEPROM_FLASH_SIZEOF_ROW macro.
*
* \param ptrRow
* The pointer to a row.
*
* \return
* The calculated value of CRC-8.
*
*******************************************************************************/
static uint32_t CalculateRowChecksum(const uint32_t * ptrRow)
{
    return ((uint32_t)CalcChecksum((const uint8_t *)((uint32_t)ptrRow + 1u),
            CY_EM_EEPROM_FLASH_SIZEOF_ROW - CY_EM_EEPROM_U32));
}


/*******************************************************************************
* Function Name: GetStoredRowChecksum
****************************************************************************//**
*
* Returns the stored in the row checksum. The row specified by the ptrRow parameter.
* The first four bytes of the row is the checksum.
*
* \param ptrRow
* The pointer to a row.
*
* \return
* The stored row checksum.
*
*******************************************************************************/
static uint32_t GetStoredRowChecksum(const uint32_t * ptrRow)
{
    return (ptrRow[CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET_U32]);
}


/*******************************************************************************
* Function Name: CheckRowChecksum
****************************************************************************//**
*
* Checks if the specified row has a valid stored CRC.
*
* \param ptrRow
* The pointer to a row.
*
* \return
* Returns the operation status. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t CheckRowChecksum(const uint32_t * ptrRow)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_CHECKSUM;

    if (GetStoredRowChecksum(ptrRow) == CalculateRowChecksum(ptrRow))
    {
        ret = CY_EM_EEPROM_SUCCESS;
    }

    return (ret);
}


/*******************************************************************************
* Function Name: GetStoredSeqNum
****************************************************************************//**
*
* Returns the stored in the row seqNum (Sequence Number).
* The row specified by ptrRow parameter.
* The second four bytes of the row is the seqNum.
*
* \param ptrRow
* The pointer to a row.
*
* \return
* The stored sequence number.
*
*******************************************************************************/
static uint32_t GetStoredSeqNum(const uint32_t * ptrRow)
{
    return (ptrRow[CY_EM_EEPROM_HEADER_SEQ_NUM_OFFSET_U32]);
}


/*******************************************************************************
* Function Name: DefineLastWrittenRow
****************************************************************************//**
*
* Performs a search of the last written row address of the Em_EEPROM associated
* with the context structure. If there were no writes to the Em_EEPROM, the
* the Em_EEPROM start address is used.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the operation status. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t DefineLastWrittenRow(cy_stc_eeprom_context_t * context)
{
    uint32_t numRows;
    uint32_t rowIndex;
    uint32_t seqNum;
    uint32_t seqNumMax;
    uint32_t * ptrRow;
    uint32_t * ptrRowMax;
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_SUCCESS;

    context->ptrLastWrittenRow = (uint32_t *)context->userFlashStartAddr;

    if (0u == context->simpleMode)
    {
        seqNumMax = 0u;
        numRows = context->numberOfRows * context->wearLevelingFactor;
        ptrRow = (uint32_t *)context->userFlashStartAddr;
        ptrRowMax = ptrRow;

        for (rowIndex = 0u; rowIndex < numRows; rowIndex++)
        {
            seqNum = GetStoredSeqNum(ptrRow);
            /* Is it a bigger number? */
            if (seqNum > seqNumMax)
            {
                if (CY_EM_EEPROM_SUCCESS == CheckRowChecksum(ptrRow))
                {
                    seqNumMax = seqNum;
                    ptrRowMax = ptrRow;
                }
            }
            /* Switches to the next row */
            ptrRow += CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32;
        }

        /* Does the same search algorithm through the redundant copy if enabled */
        if (0u != context->redundantCopy)
        {
            for (rowIndex = 0u; rowIndex < numRows; rowIndex++)
            {
                seqNum = GetStoredSeqNum(ptrRow);
                /* Is it a bigger number? */
                if (seqNum > seqNumMax)
                {
                    if (CY_EM_EEPROM_SUCCESS == CheckRowChecksum(ptrRow))
                    {
                        seqNumMax = seqNum;
                        ptrRowMax = ptrRow;
                        ret = CY_EM_EEPROM_REDUNDANT_COPY_USED;
                    }
                }
                /* Switches to the next row */
                ptrRow += CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32;
            }
        }
        context->ptrLastWrittenRow = ptrRowMax;
    }

    return (ret);
}


/*******************************************************************************
* Function Name: CheckLastWrittenRowIntegrity
****************************************************************************//**
*
* Checks the CRC of the last written row. If CRC is valid then the function
* stores sequence number into the provided ptrSeqNum pointer. If CRC of the
* last written row is invalid, then the function searches for the row with
* the highest sequence number and valid CRC. In this case, the pointer to the
* last written row is redefined inside context structure and the sequence
* number of this row is returned.
*
* If the redundant copy is enabled, then the copy also considered at
* the correct CRC verification and sequence number searching.
*
* If Simple Mode is enabled, the sequence number is not available and
* this function returns 0.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \param ptrSeqNum
* The pointer to the sequence number.
*
* \return
* Returns the operation status. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t CheckLastWrittenRowIntegrity(
                uint32_t * ptrSeqNum,
                cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_SUCCESS;

    uint32_t * ptrRowCopy;
    uint32_t seqNum = 0u;

    if (0u == context->simpleMode)
    {
        /* Checks the row CRC */
        if (CY_EM_EEPROM_SUCCESS == CheckRowChecksum(context->ptrLastWrittenRow))
        {
            seqNum = GetStoredSeqNum(context->ptrLastWrittenRow);
        }
        else
        {
            /* CRC is bad. Checks if the redundant copy if enabled */
            if (0u != context->redundantCopy)
            {
                /* Calculates the redundant copy pointer */
                ptrRowCopy = ((context->numberOfRows * context->wearLevelingFactor) *
                    CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32) + context->ptrLastWrittenRow;

                /* Checks CRC of the redundant copy */
                if (CY_EM_EEPROM_SUCCESS == CheckRowChecksum(ptrRowCopy))
                {
                    seqNum = GetStoredSeqNum(ptrRowCopy);
                    ret = CY_EM_EEPROM_REDUNDANT_COPY_USED;
                }
                else
                {
                    (void)DefineLastWrittenRow(context);
                    if (CY_EM_EEPROM_SUCCESS == CheckRowChecksum(context->ptrLastWrittenRow))
                    {
                        seqNum = GetStoredSeqNum(context->ptrLastWrittenRow);
                    }
                    ret = CY_EM_EEPROM_BAD_CHECKSUM;
                }
            }
            else
            {
                (void)DefineLastWrittenRow(context);
                if (CY_EM_EEPROM_SUCCESS == CheckRowChecksum(context->ptrLastWrittenRow))
                {
                    seqNum = GetStoredSeqNum(context->ptrLastWrittenRow);
                }
                ret = CY_EM_EEPROM_BAD_CHECKSUM;
            }
        }
    }
    *ptrSeqNum = seqNum;

    return(ret);
}


/*******************************************************************************
* Function Name: GetNextRowPointer
****************************************************************************//**
*
* Increments the row pointer and performs out of the Em_EEPROM range verification.
* The memory range is defined as the number of rows multiplied by the wear leveling
* factor. It does not include the redundant copy area.
*
* \param ptrRow
* The pointer to the flash row.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the pointer to the next row.
*
*******************************************************************************/
static uint32_t * GetNextRowPointer(
                uint32_t * ptrRow,
                const cy_stc_eeprom_context_t * context)
{
    uint32_t wlEndAddr = ((CY_EM_EEPROM_FLASH_SIZEOF_ROW * context->numberOfRows) *
            context->wearLevelingFactor) + context->userFlashStartAddr;

    /* Gets the pointer to the next row to be processed without the range verification */
    ptrRow += CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32;

    if ((uint32_t)ptrRow >= wlEndAddr)
    {
        ptrRow = (uint32_t *)context->userFlashStartAddr;
    }
    return (ptrRow);
}


/*******************************************************************************
* Function Name: GetReadRowPointer
****************************************************************************//**
*
* Calculates the row pointer to be used to read historic and headers data.
*
* \param ptrRow
* The pointer to the flash row.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the pointer to the row where data is read from.
*
*******************************************************************************/
static uint32_t * GetReadRowPointer(
                uint32_t * ptrRow,
                const cy_stc_eeprom_context_t * context)
{
    uint32_t wlBlockSize;

    /* Adjusts the row read pointer if wear leveling is enabled */
    if (context->wearLevelingFactor > 1u)
    {
        /* The size of one wear-leveling block */
        wlBlockSize = context->numberOfRows * CY_EM_EEPROM_FLASH_SIZEOF_ROW;
        /* Checks if it is the first block of the wear leveling */
        if (((uint32_t)ptrRow) < (context->userFlashStartAddr + wlBlockSize))
        {
            /* Jumps to the last wear-leveling block */
        	ptrRow = (uint32_t *)((uint32_t)ptrRow + (wlBlockSize * (context->wearLevelingFactor - 1u)));
        }
        else
        {
            /* Decreases the read pointer for one wear-leveling block */
        	ptrRow = (uint32_t *)((uint32_t)ptrRow - wlBlockSize);
        }
    }
    return (ptrRow);
}


/*******************************************************************************
* Function Name: CopyHistoricData
****************************************************************************//**
*
* Copies relevant historic data into the specified buffer.
*
* The function includes the proper handling of the redundant copy and wear leveling
* if enabled.
*
* \param ptrRowWrite
* The pointer to the buffer to store historic data.
*
* \param ptrRow
* The pointer to the current active flash row.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
* \return
* Returns the operation status. See \ref cy_en_em_eeprom_status_t.
*
*******************************************************************************/
static cy_en_em_eeprom_status_t CopyHistoricData(
                uint32_t * ptrRowWrite,
                uint32_t * ptrRow,
                const cy_stc_eeprom_context_t * context)
{
    cy_en_em_eeprom_status_t ret = CY_EM_EEPROM_BAD_CHECKSUM;
    uint32_t * ptrRowRead = GetReadRowPointer(ptrRow, context);

    if (CY_EM_EEPROM_SUCCESS == CheckRowChecksum(ptrRowRead))
    {
        (void)memcpy((void *)&ptrRowWrite[CY_EM_EEPROM_HISTORIC_DATA_OFFSET_U32],
                     (const void *)&ptrRowRead[CY_EM_EEPROM_HISTORIC_DATA_OFFSET_U32],
                     context->byteInRow);
        ret = CY_EM_EEPROM_SUCCESS;
    }
    else
    {
        /* CRC is bad. Checks if the redundant copy if enabled */
        if (0u != context->redundantCopy)
        {
            ptrRowRead += ((context->numberOfRows * context->wearLevelingFactor) *
                    CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32);
            if (CY_EM_EEPROM_SUCCESS == CheckRowChecksum(ptrRowRead))
            {
                /* Copies the Em_EEPROM historic data from the redundant copy */
                (void)memcpy((void *)&ptrRowWrite[CY_EM_EEPROM_HISTORIC_DATA_OFFSET_U32],
                             (const void *)&ptrRowRead[CY_EM_EEPROM_HISTORIC_DATA_OFFSET_U32],
                             context->byteInRow);
                /* Reports that the redundant copy was used */
                ret = CY_EM_EEPROM_REDUNDANT_COPY_USED;
            }
        }
        if ((0u == GetStoredSeqNum(ptrRowRead)) && (0u == GetStoredRowChecksum(ptrRowRead)))
        {
            /*
            * Considers a row with a bad checksum as the row that never has never been
            * written before if the sequence number and CRC values are zeros.
            * In this case, reports zeros and does not
            * report the failed status of the write operation.
            */
            ret = CY_EM_EEPROM_SUCCESS;
        }
    }
    return (ret);
}


/*******************************************************************************
* Function Name: CopyHeadersData
****************************************************************************//**
*
* Copies relevant data located in the headers into a row specified by the
* pointer ptrRow.
* The function includes the proper handling of a redundant copy and wear leveling
* if enabled.
*
* \param ptrRowWrite
* The pointer to the buffer where to store the headers data.
*
* \param ptrRow
* The pointer to the current active flash row.
*
* \param context
* The pointer to the Em_EEPROM context structure \ref cy_stc_eeprom_context_t.
*
*******************************************************************************/
static void CopyHeadersData(
                uint32_t * ptrRowWrite,
                uint32_t * ptrRow,
                const cy_stc_eeprom_context_t * context)
{
    uint32_t i;
    uint32_t strHistAddr;
    uint32_t endHistAddr;
    uint32_t strHeadAddr;
    uint32_t endHeadAddr;
    uint32_t dstOffset;
    uint32_t srcOffset;
    uint32_t sizeToCopy;
    cy_en_em_eeprom_status_t crcStatus = CY_EM_EEPROM_SUCCESS;
    uint32_t numReads = context->numberOfRows;
    uint32_t * ptrRowWork;
    uint32_t * ptrRowRead = GetReadRowPointer(ptrRow, context);

    /* Skips unwritten rows if any */
    if (numReads > GetStoredSeqNum(ptrRowWrite))
    {
        numReads = GetStoredSeqNum(ptrRowWrite);
    }

    /* The address within the Em_EEPROM storage of historic data of the specified by the ptrRow row */
    strHistAddr = ((((uint32_t)ptrRow - context->userFlashStartAddr) /
                CY_EM_EEPROM_FLASH_SIZEOF_ROW) % context->numberOfRows) *
                (CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u);
    endHistAddr = strHistAddr + (CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u);

    for (i = 0u; i < numReads; i++)
    {
        /* No need to check the row about to write since it was checked at the last write */
        ptrRowRead = GetNextRowPointer(ptrRowRead, context);
        ptrRowWork = ptrRowRead;

        /* For the last header-read operation, checks data in the recently created header */
        if (i >= (numReads - 1u))
        {
            ptrRowWork = ptrRowWrite;
        }
        else
        {
            /* Checks CRC of the row to be read except the last row of a recently created header */
            crcStatus = CheckRowChecksum(ptrRowWork);
            if ((CY_EM_EEPROM_SUCCESS != crcStatus) && (0u != context->redundantCopy))
            {
                /* Calculates the redundant copy pointer */
                ptrRowWork += ((context->numberOfRows * context->wearLevelingFactor) *
                    CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32);
                crcStatus = CheckRowChecksum(ptrRowWork);
            }
        }

        /* Skipsthe row if CRC is bad */
        if (CY_EM_EEPROM_SUCCESS == crcStatus)
        {
            /* The address of header data */
            strHeadAddr = ptrRowWork[CY_EM_EEPROM_HEADER_ADDR_OFFSET_U32];
            sizeToCopy = ptrRowWork[CY_EM_EEPROM_HEADER_LEN_OFFSET_U32];
            endHeadAddr = strHeadAddr + sizeToCopy;

            /* Skips the row if the header data address is out of the historic data address range */
            if ((strHeadAddr < endHistAddr) && (endHeadAddr > strHistAddr))
            {
                dstOffset = 0u;
                srcOffset = 0u;

                if (strHeadAddr >= strHistAddr)
                {
                    dstOffset = strHeadAddr % context->byteInRow;
                    if (endHeadAddr > endHistAddr)
                    {
                        sizeToCopy = endHistAddr - strHeadAddr;
                    }
                }
                else
                {
                    srcOffset = context->byteInRow - (strHeadAddr % context->byteInRow);
                    sizeToCopy = endHeadAddr - strHistAddr;
                }
                (void)memcpy((void *)((uint32_t)(&ptrRowWrite[CY_EM_EEPROM_HISTORIC_DATA_OFFSET_U32]) + dstOffset),
                            (const void *)((uint32_t)(&ptrRowWork[CY_EM_EEPROM_HEADER_DATA_OFFSET_U32]) + srcOffset),
                            sizeToCopy);
            }
        }
    }
}


/* [] END OF FILE */
