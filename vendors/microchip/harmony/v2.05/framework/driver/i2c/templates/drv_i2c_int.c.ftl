<#--
/*******************************************************************************
  I2C Driver Interrupt Handler Template File

  File Name:
    drv_i2c_int.c

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, configuration bits, and allocates
    any necessary global system resources, such as the systemObjects structure
    that contains the object handles to all the MPLAB Harmony module objects in
    the system.
 *******************************************************************************/

/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
 -->
<#if CONFIG_DRV_I2C_INTERRUPT_MODE == true>

<#--- I2C0 --->

  <#if CONFIG_DRV_I2C_INST_IDX0 == true>
    <#if (CONFIG_PIC32MX == true) >
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX0 == true>
            <#assign IDX0_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX0>
            <#assign IDX0_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX0>
        <#else>
            <#assign IDX0_VECTOR = CONFIG_DRV_I2C_ISR_VECTOR_IDX0>
            <#assign IDX0_PRIORITY = CONFIG_DRV_I2C_INT_PRIO_NUM_IDX0>
        </#if>
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${IDX0_VECTOR}, ipl${IDX0_PRIORITY}SOFT) _IntHandlerDrvI2CInstance0(void)
            <#else>
            <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX0_PRIORITY}AUTO), vector(${IDX0_VECTOR}))) IntHandlerDrvI2CInstance0_ISR( void );
            </#if>
void IntHandlerDrvI2CInstance0(void)
            </#if>
        <#else>
void __ISR(${IDX0_VECTOR}, ipl${IDX0_PRIORITY}AUTO) _IntHandlerDrvI2CInstance0(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
    </#if>
    <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
    </#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX0 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C0);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX0});
        <#else>
    DRV_I2C_Tasks(sysObj.drvI2C0);
        </#if>
      </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
	DRV_I2C0_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_MASTER"))>
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX0});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX0});
        </#if>
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_SLAVE"))>
    /* Dummy read from Receive register */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) )
	{
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX0});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX0});
        </#if>
      </#if> <#-- if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC"-->
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
    </#if> <#-- CONFIG_PIC32MX -->

    <#-- start of PIC32MZ for I2C0 --->

    <#if (CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true || CONFIG_PIC32MK == true) >
      <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_MASTER")>
          <#if CONFIG_DRV_I2C_BIT_BANG_IDX0 == true>
              <#assign IDX0_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX0>
              <#assign IDX0_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX0>
          <#else>
              <#assign IDX0_VECTOR = CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX0>
              <#assign IDX0_PRIORITY = CONFIG_DRV_I2C_MASTER_INT_PRIO_NUM_IDX0>
          </#if>
         <#if CONFIG_USE_3RDPARTY_RTOS>
             <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
 void __ISR(${IDX0_VECTOR}, ipl${IDX0_PRIORITY}SOFT) _IntHandlerDrvI2CMasterInstance0(void)
             <#else>
              <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX0_PRIORITY}AUTO), vector(${IDX0_VECTOR}))) IntHandlerDrvI2CMasterInstance0_ISR( void );
              </#if>
void IntHandlerDrvI2CMasterInstance0(void)
              </#if>
          <#else>
void __ISR(${IDX0_VECTOR}, ipl${IDX0_PRIORITY}AUTO) _IntHandlerDrvI2CMasterInstance0(void)
          </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
    <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
    </#if>
    <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
    </#if>
</#if>
    <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX0 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C0);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX0});
         <#else>
    DRV_I2C_Tasks(sysObj.drvI2C0);
         </#if>
    </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
	DRV_I2C0_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX0});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_MASTER" -->
      <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_SLAVE")>
         <#if CONFIG_USE_3RDPARTY_RTOS>
             <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
 void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX0}SOFT) _IntHandlerDrvI2CSlaveInstance0(void)
             <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX0}AUTO), vector(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX0}))) IntHandlerDrvI2CSlaveInstance0_ISR( void );
          </#if>
void IntHandlerDrvI2CSlaveInstance0(void)
             </#if>
        <#else>
void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX0}AUTO) _IntHandlerDrvI2CSlaveInstance0(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
        <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    DRV_I2C_Tasks(sysObj.drvI2C0);
        </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
	DRV_I2C0_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) )
    {
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX0});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_SLAVE" -->

    <#if (CONFIG_DRV_I2C_BIT_BANG_IDX0 == false)>

         <#if CONFIG_USE_3RDPARTY_RTOS>
             <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX0}SOFT) _IntHandlerDrvI2CErrorInstance0(void)
             <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX0}AUTO), vector(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX0}))) IntHandlerDrvI2CErrorInstance0_ISR( void );
          </#if>
void IntHandlerDrvI2CErrorInstance0(void)
             </#if>
      <#else>
void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX0}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX0}AUTO) _IntHandlerDrvI2CErrorInstance0(void)
      </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    SYS_ASSERT(false, "I2C Driver Instance 0 Error");
      </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX0});
      </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
    </#if>
</#if>
}
    </#if>

    </#if> <#-- CONFIG_PIC32MZ --><#-- CONFIG_PIC32MK -->
  </#if> <#-- CONFIG_DRV_I2C_INST_IDX0 -->


  <#--- I2C1 --->


  <#if CONFIG_DRV_I2C_INST_IDX1 == true>
    <#if (CONFIG_PIC32MX == true) >
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX1 == true>
            <#assign IDX1_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX1>
            <#assign IDX1_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX1>
        <#else>
            <#assign IDX1_VECTOR = CONFIG_DRV_I2C_ISR_VECTOR_IDX1>
            <#assign IDX1_PRIORITY = CONFIG_DRV_I2C_INT_PRIO_NUM_IDX1>
        </#if>
         <#if CONFIG_USE_3RDPARTY_RTOS>
             <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${IDX1_VECTOR}, ipl${IDX1_PRIORITY}SOFT) _IntHandlerDrvI2CInstance1(void)
             <#else>
            <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX1_PRIORITY}AUTO), vector(${IDX1_VECTOR}))) IntHandlerDrvI2CInstance1_ISR( void );
            </#if>
void IntHandlerDrvI2CInstance1(void)
            </#if>
        <#else>
void __ISR(${IDX1_VECTOR}, ipl${IDX1_PRIORITY}AUTO) _IntHandlerDrvI2CInstance1(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX1 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C1);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX1});
        <#else>
    DRV_I2C_Tasks(sysObj.drvI2C1);
        </#if>
      </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
	DRV_I2C1_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_MASTER"))>
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX1});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX1});
        </#if>
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_SLAVE"))>
    /* Dummy read from Receive register */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) )
	{
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX1});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX1});
        </#if>
      </#if> <#-- if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC"-->
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
    </#if> <#-- CONFIG_PIC32MX -->

    <#-- start of PIC32MZ for I2C1 --->

    <#if (CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true|| CONFIG_PIC32MK == true) >
        <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_MASTER")>
            <#if CONFIG_DRV_I2C_BIT_BANG_IDX1 == true>
                <#assign IDX1_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX1>
                <#assign IDX1_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX1>
            <#else>
                <#assign IDX1_VECTOR = CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX1>
                <#assign IDX1_PRIORITY = CONFIG_DRV_I2C_MASTER_INT_PRIO_NUM_IDX1>
            </#if>
            <#if CONFIG_USE_3RDPARTY_RTOS>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   void __ISR(${IDX1_VECTOR}, ipl${IDX1_PRIORITY}SOFT) _IntHandlerDrvI2CMasterInstance1(void)
                <#else>
                <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX1_PRIORITY}AUTO), vector(${IDX1_VECTOR}))) IntHandlerDrvI2CMasterInstance1_ISR( void );
                </#if>
void IntHandlerDrvI2CMasterInstance1(void)
                </#if>
            <#else>
void __ISR(${IDX1_VECTOR}, ipl${IDX1_PRIORITY}AUTO) _IntHandlerDrvI2CMasterInstance1(void)
            </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
        <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
            <#if CONFIG_DRV_I2C_BIT_BANG_IDX1 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C1);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX1});
            <#else>
    DRV_I2C_Tasks(sysObj.drvI2C1);
            </#if>
        </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
	DRV_I2C1_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX1});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_MASTER" -->
      <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_SLAVE")>
          <#if CONFIG_USE_3RDPARTY_RTOS>
              <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
 void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX1}SOFT) _IntHandlerDrvI2CSlaveInstance1(void)
              <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX1}AUTO), vector(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX1}))) IntHandlerDrvI2CSlaveInstance1_ISR( void );
          </#if>
void IntHandlerDrvI2CSlaveInstance1(void)
              </#if>
        <#else>
void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX1}AUTO) _IntHandlerDrvI2CSlaveInstance1(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
        <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    DRV_I2C_Tasks(sysObj.drvI2C1);
        </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
	DRV_I2C1_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC">
    /* TODO: Add code to process interrupt here */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) )
    {
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX1});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_SLAVE" -->

    <#if (CONFIG_DRV_I2C_BIT_BANG_IDX1 == false)>

          <#if CONFIG_USE_3RDPARTY_RTOS>
              <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
 void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX1}SOFT) _IntHandlerDrvI2CErrorInstance1(void)
              <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX1}AUTO), vector(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX1}))) IntHandlerDrvI2CErrorInstance1_ISR( void );
          </#if>
void IntHandlerDrvI2CErrorInstance1(void)
              </#if>
      <#else>
void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX1}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX1}AUTO) _IntHandlerDrvI2CErrorInstance1(void)
      </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    SYS_ASSERT(false, "I2C Driver Instance 1 Error");
      </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC"  || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC"  || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC" >
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX1});
      </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
    </#if>
</#if>
}

    </#if>

    </#if> <#-- CONFIG_PIC32MZ --> <#-- CONFIG_PIC32MK -->
  </#if> <#-- CONFIG_DRV_I2C_INST_IDX1 -->


  <#--- I2C2 --->

<#if CONFIG_DRV_I2C_INST_IDX2 == true>
    <#if (CONFIG_PIC32MX == true) >
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX2 == true>
            <#assign IDX2_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX2>
            <#assign IDX2_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX2>
        <#else>
            <#assign IDX2_VECTOR = CONFIG_DRV_I2C_ISR_VECTOR_IDX2>
            <#assign IDX2_PRIORITY = CONFIG_DRV_I2C_INT_PRIO_NUM_IDX2>
        </#if>
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
 void __ISR(${IDX2_VECTOR}, ipl${IDX2_PRIORITY}SOFT) _IntHandlerDrvI2CInstance2(void)
            <#else>
            <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX2_PRIORITY}AUTO), vector(${IDX2_VECTOR}))) IntHandlerDrvI2CInstance2_ISR( void );
            </#if>
void IntHandlerDrvI2CInstance2(void)
            </#if>
        <#else>
void __ISR(${IDX2_VECTOR}, ipl${IDX2_PRIORITY}AUTO) _IntHandlerDrvI2CInstance2(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX2 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C2);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX2});
        <#else>
    DRV_I2C_Tasks(sysObj.drvI2C2);
        </#if>
      </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    DRV_I2C2_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_MASTER"))>
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX2});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX2});
        </#if>
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_SLAVE"))>
    /* Dummy read from Receive register */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) )
	{
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX2});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX2});
        </#if>
      </#if> <#-- if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC"-->
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
    </#if> <#-- CONFIG_PIC32MX -->


    <#-- start of PIC32MZ for I2C2 --->

    <#if (CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true) >
      <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_MASTER")>
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX2 == true>
            <#assign IDX2_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX2>
            <#assign IDX2_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX2>
        <#else>
            <#assign IDX2_VECTOR = CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX2>
            <#assign IDX2_PRIORITY = CONFIG_DRV_I2C_MASTER_INT_PRIO_NUM_IDX2>
        </#if>
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${IDX2_VECTOR}, ipl${IDX2_PRIORITY}SOFT) _IntHandlerDrvI2CMasterInstance2(void)
            <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX2_PRIORITY}AUTO), vector(${IDX2_VECTOR}))) IntHandlerDrvI2CMasterInstance2_ISR( void );
          </#if>
void IntHandlerDrvI2CMasterInstance2(void)
            </#if>
        <#else>
void __ISR(${IDX2_VECTOR}, ipl${IDX2_PRIORITY}AUTO) _IntHandlerDrvI2CMasterInstance2(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
        <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
            <#if CONFIG_DRV_I2C_BIT_BANG_IDX2 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C2);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX2});
            <#else>
    DRV_I2C_Tasks(sysObj.drvI2C2);
            </#if>
        </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    DRV_I2C2_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX2});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_MASTER" -->
      <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_SLAVE")>
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX2}SOFT) _IntHandlerDrvI2CSlaveInstance2(void)
            <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX2}AUTO), vector(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX2}))) IntHandlerDrvI2CSlaveInstance2_ISR( void );
          </#if>
void IntHandlerDrvI2CSlaveInstance2(void)
            </#if>
        <#else>
void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX2}AUTO) _IntHandlerDrvI2CSlaveInstance2(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
        <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    DRV_I2C_Tasks(sysObj.drvI2C2);
        </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    DRV_I2C2_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
     /* Dummy read from Receive register */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) )
    {
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX2});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_SLAVE" -->

    <#if (CONFIG_DRV_I2C_BIT_BANG_IDX2 == false)>

        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX2}SOFT) _IntHandlerDrvI2CErrorInstance2(void)
            <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX2}AUTO), vector(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX2}))) IntHandlerDrvI2CErrorInstance2_ISR( void );
          </#if>
void IntHandlerDrvI2CErrorInstance2(void)
            </#if>
      <#else>
void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX2}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX2}AUTO) _IntHandlerDrvI2CErrorInstance2(void)
      </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    SYS_ASSERT(false, "I2C Driver Instance 2 Error");
      </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX2});
      </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
    </#if>
</#if>
}
    </#if>


    </#if> <#-- CONFIG_PIC32MZ --> <#-- CONFIG_PIC32MK -->
  </#if> <#-- CONFIG_DRV_I2C_INST_IDX2 -->


    <#-- I2C3 --->

  <#if CONFIG_DRV_I2C_INST_IDX3 == true>
    <#if (CONFIG_PIC32MX == true)>
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX3 == true>
            <#assign IDX3_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX3>
            <#assign IDX3_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX3>
        <#else>
            <#assign IDX3_VECTOR = CONFIG_DRV_I2C_ISR_VECTOR_IDX3>
            <#assign IDX3_PRIORITY = CONFIG_DRV_I2C_INT_PRIO_NUM_IDX3>
        </#if>
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${IDX3_VECTOR}, ipl${IDX3_PRIORITY}SOFT) _IntHandlerDrvI2CInstance3(void)
            <#else>
            <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX3_PRIORITY}AUTO), vector(${IDX3_VECTOR}))) IntHandlerDrvI2CInstance3_ISR( void );
            </#if>
void IntHandlerDrvI2CInstance3(void)
            </#if>
        <#else>
void __ISR(${IDX3_VECTOR}, ipl${IDX3_PRIORITY}AUTO) _IntHandlerDrvI2CInstance3(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX3 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C3);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX3});
        <#else>
    DRV_I2C_Tasks(sysObj.drvI2C3);
        </#if>
	  </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    DRV_I2C3_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC" >
    /* TODO: Add code to process interrupt here */
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_MASTER"))>
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX3});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX3});
        </#if>
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_SLAVE"))>
    /* Dummy read from Receive register */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) )
	{
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX3});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX3});
        </#if>
      </#if> <#-- if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC"-->
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
    </#if> <#-- CONFIG_PIC32MX -->

    <#---- start of PIC32MZ for I2C3 ---->

    <#if (CONFIG_PIC32MZ == true || CONFIG_PIC32MK == true) >
        <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_MASTER")>
            <#if CONFIG_DRV_I2C_BIT_BANG_IDX3 == true>
                <#assign IDX3_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX3>
                <#assign IDX3_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX3>
            <#else>
                <#assign IDX3_VECTOR = CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX3>
                <#assign IDX3_PRIORITY = CONFIG_DRV_I2C_MASTER_INT_PRIO_NUM_IDX3>
            </#if>
            <#if CONFIG_USE_3RDPARTY_RTOS>
                <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${IDX3_VECTOR}, ipl${IDX3_PRIORITY}SOFT) _IntHandlerDrvI2CMasterInstance3(void)
                <#else>
                <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX3_PRIORITY}AUTO), vector(${IDX3_VECTOR}))) IntHandlerDrvI2CMasterInstance3_ISR( void );
                </#if>
void IntHandlerDrvI2CMasterInstance3(void)
                </#if>
            <#else>
void __ISR(${IDX3_VECTOR}, ipl${IDX3_PRIORITY}AUTO) _IntHandlerDrvI2CMasterInstance3(void)
            </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
        <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
            <#if CONFIG_DRV_I2C_BIT_BANG_IDX3 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C3);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX3});
            <#else>
    DRV_I2C_Tasks(sysObj.drvI2C3);
            </#if>
        </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    DRV_I2C3_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC" >
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX3});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_MASTER" -->
      <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_SLAVE")>
          <#if CONFIG_USE_3RDPARTY_RTOS>
              <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
 void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX3}SOFT) _IntHandlerDrvI2CSlaveInstance3(void)
              <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX3}AUTO), vector(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX3}))) IntHandlerDrvI2CSlaveInstance3_ISR( void );
          </#if>
void IntHandlerDrvI2CSlaveInstance3(void)
              </#if>
        <#else>
void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX3}AUTO) _IntHandlerDrvI2CSlaveInstance3(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
        <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    DRV_I2C_Tasks(sysObj.drvI2C3);
        </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    DRV_I2C3_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
	/* Dummy read from Receive register */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) )
    {
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX3});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_SLAVE" -->

    <#if (CONFIG_DRV_I2C_BIT_BANG_IDX3 == false)>

        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX3}SOFT) _IntHandlerDrvI2CErrorInstance3(void)
            <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX3}AUTO), vector(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX3}))) IntHandlerDrvI2CErrorInstance3_ISR( void );
          </#if>
void IntHandlerDrvI2CErrorInstance3(void)
            </#if>
      <#else>
void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX3}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX3}AUTO) _IntHandlerDrvI2CErrorInstance3(void)
      </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    SYS_ASSERT(false, "I2C Driver Instance 3 Error");
      </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt afterwards */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX3});
      </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
    </#if>
</#if>
}
    </#if>

    </#if> <#-- CONFIG_PIC32MZ --> <#-- CONFIG_PIC32MK -->
  </#if> <#-- CONFIG_DRV_I2C_INST_IDX3 -->



  <#--- I2C4 --->


<#if CONFIG_DRV_I2C_INST_IDX4 == true>
    <#if (CONFIG_PIC32MX == true)>
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX4 == true>
            <#assign IDX4_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX4>
            <#assign IDX4_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX4>
        <#else>
            <#assign IDX4_VECTOR = CONFIG_DRV_I2C_ISR_VECTOR_IDX4>
            <#assign IDX4_PRIORITY = CONFIG_DRV_I2C_INT_PRIO_NUM_IDX4>
        </#if>
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${IDX4_VECTOR}, ipl${IDX4_PRIORITY}SOFT) _IntHandlerDrvI2CInstance4(void)
            <#else>
            <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX4_PRIORITY}AUTO), vector(${IDX4_VECTOR}))) IntHandlerDrvI2CInstance4_ISR( void );
            </#if>
void IntHandlerDrvI2CInstance4(void)
            </#if>
        <#else>
void __ISR(${IDX4_VECTOR}, ipl${IDX4_PRIORITY}AUTO) _IntHandlerDrvI2CInstance4(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX4 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C4);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX4});
        <#else>
    DRV_I2C_Tasks(sysObj.drvI2C4);
        </#if>
      </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    DRV_I2C4_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_MASTER"))>
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX4});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX4});
        </#if>
        <#if ((CONFIG_DRV_I2C_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_SLAVE"))>
    /* Dummy read from Receive register */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) )
	{
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX4});
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX4});
        </#if>
      </#if> <#-- if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC"-->
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
    </#if> <#-- CONFIG_PIC32MX -->


    <#--- start of PIC32MZ for I2C4 ---->

    <#if (CONFIG_PIC32MZ == true) >
      <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_MASTER") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_MASTER")>
        <#if CONFIG_DRV_I2C_BIT_BANG_IDX4 == true>
            <#assign IDX4_VECTOR = CONFIG_DRV_I2C_BIT_BANG_ISR_VECTOR_IDX4>
            <#assign IDX4_PRIORITY = CONFIG_DRV_I2C_BIT_BANG_INT_PRIO_NUM_IDX4>
        <#else>
            <#assign IDX4_VECTOR = CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX4>
            <#assign IDX4_PRIORITY = CONFIG_DRV_I2C_MASTER_INT_PRIO_NUM_IDX4>
        </#if>
        <#if CONFIG_USE_3RDPARTY_RTOS>
            <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${IDX4_VECTOR}, ipl${IDX4_PRIORITY}SOFT) _IntHandlerDrvI2CMasterInstance4(void)
            <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${IDX4_PRIORITY}AUTO), vector(${IDX4_VECTOR}))) IntHandlerDrvI2CMasterInstance4_ISR( void );
          </#if>
void IntHandlerDrvI2CMasterInstance4(void)
            </#if>
        <#else>
void __ISR(${IDX4_VECTOR}, ipl${IDX4_PRIORITY}AUTO) _IntHandlerDrvI2CMasterInstance4(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
        <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
            <#if CONFIG_DRV_I2C_BIT_BANG_IDX4 == true>
    DRV_I2C_BB_Tasks(sysObj.drvI2C4);
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_BIT_BANG_INT_SRC_IDX4});
            <#else>
    DRV_I2C_Tasks(sysObj.drvI2C4);
            </#if>
        </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    DRV_I2C4_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_MASTER_INT_SRC_IDX4});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_MASTER" -->
      <#if (CONFIG_DRV_I2C_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_SLAVE") || (CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_SLAVE")>
          <#if CONFIG_USE_3RDPARTY_RTOS>
              <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX4}SOFT) _IntHandlerDrvI2CSlaveInstance4(void)
              <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX4}AUTO), vector(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX4}))) IntHandlerDrvI2CSlaveInstance4_ISR( void );
          </#if>
void IntHandlerDrvI2CSlaveInstance4(void)
              </#if>
        <#else>
void __ISR(${CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_I2C_SLAVE_INT_PRIO_NUM_IDX4}AUTO) _IntHandlerDrvI2CSlaveInstance4(void)
        </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
        <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    DRV_I2C_Tasks(sysObj.drvI2C4);
        </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    DRV_I2C4_Tasks();
</#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC">
    /* TODO: Add code to process interrupt here */
	/* Dummy read from Receive register */
    if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4});
    }
    else  if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) &&
       (PLIB_I2C_SlaveAddressIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) )
    {
        PLIB_I2C_SlaveClockRelease(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4});
        PLIB_I2C_ReceivedByteGet (${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4});
    }
    else if ( (!PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) )
    {
        /* TODO: Add code to process interrupt here */
    }
    else if ( (PLIB_I2C_SlaveReadIsRequested(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) &&
       (PLIB_I2C_SlaveDataIsDetected(${CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4})) )
    {
       /* TODO: Add code to process interrupt here */
    }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX4});
        </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
        </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
      </#if><#-- End "DRV_I2C_MODE_SLAVE" -->

      <#if CONFIG_USE_3RDPARTY_RTOS>
          <#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX4}SOFT) _IntHandlerDrvI2CErrorInstance4(void)
          <#else>
          <#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
void __attribute__( (interrupt(ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX4}AUTO), vector(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX4}))) IntHandlerDrvI2CErrorInstance4_ISR( void );
          </#if>
void IntHandlerDrvI2CErrorInstance4(void)
          </#if>
      <#else>
void __ISR(${CONFIG_DRV_I2C_ERR_ISR_VECTOR_IDX4}, ipl${CONFIG_DRV_I2C_ERR_INT_PRIO_NUM_IDX4}AUTO) _IntHandlerDrvI2CErrorInstance4(void)
      </#if>
{
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
    /* Call ThreadX context save.  */
    _tx_thread_context_save();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_EnterNestableInterrupt();
</#if>
</#if>
      <#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
    SYS_ASSERT(false, "I2C Driver Instance 4 Error");
      </#if>
<#if CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BYTE_MODEL_STATIC" || CONFIG_DRV_I2C_STATIC_DRIVER_MODE == "BUFFER_MODEL_STATIC">
    /* TODO: Add code to process interrupt here */
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX4});
      </#if>
<#if CONFIG_USE_3RDPARTY_RTOS>
<#if CONFIG_3RDPARTY_RTOS_USED == "ThreadX">
   /* Call ThreadX context restore.  */
   _tx_thread_context_restore();
      </#if>
<#if CONFIG_3RDPARTY_RTOS_USED == "embOS">
    OS_LeaveNestableInterrupt();
</#if>
</#if>
}
    </#if> <#-- CONFIG_PIC32MZ -->
  </#if> <#-- CONFIG_DRV_I2C_INST_IDX4 -->




</#if> <#-- CONFIG_DRV_I2C_INTERRUPT_MODE -->
<#--
/*******************************************************************************
 End of File
*/
-->
