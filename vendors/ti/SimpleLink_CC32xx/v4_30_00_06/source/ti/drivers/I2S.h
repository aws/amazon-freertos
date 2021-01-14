/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
/*!****************************************************************************
 *  @file       I2S.h
 *  @brief      Inter-Integrated Circuit Sound (I2S) Bus Driver
 *
 *  The I2S header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/I2S.h>
 *  @endcode
 *
 *  @anchor ti_drivers_I2S_Overview
 *  # Overview #
 *
 *  The I2S driver facilitates the use of Inter-IC Sound (I2S), which is
 *  used to connect digital audio devices so that audio signals can be
 *  communicated between devices. The I2S driver simplifies reading and
 *  writing to any of the Multichannel Audio Serial Port (McASP) peripherals
 *  on the board with Receive and Transmit support. These include read and
 *  write characters on the McASP peripheral.
 *
 *  I2S interfaces typically consist of 4 or 5 signals. The 5th signal is not
 *  systematically used.
 *  @li <b>Serial Clock (SCK)</b> also called Bit Clock (BCLK) or Multichannel
 *      Audio Frame Synchronization (McAFSX)
 *  @li <b>Word Select (WS)</b> also called Word Clock (WCLK), Left Right
 *      Clock (LRCLK) or Multichannel Audio Clock (McACLK)
 *  @li <b>Serial Data (SD0)</b> also called AD0, AD1, McAXR0, or possibly SDI
 *  @li <b>Serial Data (SD1)</b> also called AD1, ADI, McAXR1, or possibly SDI
 *  @li <b>Master Clock (MCLK)</b>
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Usage
 *  # Usage #
 *
 *  <b>The I2S driver provides the following APIs:</b>
 *  @li I2S_init(): @copybrief I2S_init
 *  @li I2S_open(): @copybrief I2S_open
 *  @li I2S_Params_init(): @copybrief I2S_Params_init
 *  @li I2S_Transaction_init(): @copybrief I2S_Transaction_init
 *  @li I2S_setReadQueueHead(): @copybrief I2S_setReadQueueHead
 *  @li I2S_setWriteQueueHead(): @copybrief I2S_setWriteQueueHead
 *  @li I2S_startClocks(): @copybrief I2S_startClocks
 *  @li I2S_startRead(): @copybrief I2S_startRead
 *  @li I2S_startWrite(): @copybrief I2S_startWrite
 *  @li I2S_stopRead(): @copybrief I2S_stopRead
 *  @li I2S_stopWrite(): @copybrief I2S_stopWrite
 *  @li I2S_stopClocks(): @copybrief I2S_stopClocks
 *  @li I2S_close(): @copybrief I2S_close
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Driver_Transactions
 *  ### Transactions #
 *
 *  Data transfers are achieved through #I2S_Transaction structures. Application is
 *  responsible to maintain the transactions queues. The I2S driver completes the
 *  transactions one by one. When a transaction is over, the I2S driver takes in
 *  consideration the next transaction (if the next transaction is NULL, the I2S
 *  drivers signals this to the user).
 *  The I2S driver relies on the following fields of the #I2S_Transaction to
 *  complete it:
 *  - the buffer
 *  - the length of the buffer
 *  - a pointer on the next transaction to achieve (kept in a List_Elem structure)
 *  .
 *  The I2S driver provides the following elements (fields of the #I2S_Transaction):
 *  - the number of untransferred bytes: the driver is designed to avoid memory corruption and will
 *  not complete an incomplete transaction (meaning a transaction where the buffer size would not
 *  permit to send or receive a whole number of samples). In this case, the system considers the
 *  samples of the beginning of the buffer and read/write as much as possible samples and ignore the
 *  end of the buffer. The number of untransafered bytes is the number of bytes left at the end of
 *  the buffer)
 *  - the number of completions of the transaction. This value is basically incremented by one
 *  every time the transaction is completed.
 *
 *  Please note that these two fields are valid only when the transaction has been completed.
 *  Consult examples to get more details on the transaction usage.
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Driver_ProvidingData
 *  ### Providing data to the I2S driver #
 *  Application is responsible to handle the queues of transactions.
 *  Application is also responsible to provide to the driver a pointer on
 *  the first transaction to consider (considering that all the following
 *  transactions are correctly queued).
 *  #I2S_setReadQueueHead() and #I2S_setWriteQueueHead()  allow the user to
 *  set the first transaction to consider. These functions should be used only
 *  when no transaction is running on the considered interface.
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Driver_StartStopClocks
 *  ### Start and stop clocks and transactions #
 *  Clocks can be started and stopped by the application.
 *  Read and write can be started and stopped independently.
 *  To start a transfer, clocks must be running.
 *  To stop the clocks no transfer must be running.
 *  Refer to the following functions for more details:
 *  @li I2S_startClocks() @li I2S_startRead() @li I2S_startWrite()
 *  @li I2S_stopRead() @li I2S_stopWrite() @li I2S_stopClocks()
 *
 *  @note
 *  @li In #I2S_SLAVE mode, clocks must be started and stopped exactly like
 *  it is done in #I2S_MASTER mode.
 *  @li If the queue of transaction is not empty, the calls to #I2S_stopRead()
 *  and #I2S_stopWrite() are blocking and potentially long.
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Examples
 *  ## Examples #
 *
 *  @li @ref ti_drivers_I2S_Example_PlayAndStop "Play and Stop"
 *  @li @ref ti_drivers_I2S_Example_Streaming "Streaming"
 *  @li @ref ti_drivers_I2S_Example_RepeatMode "Repeat"
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Example_PlayAndStop
 *  ### Mode Play and Stop #
 *  The following example shows how to simultaneously receive and send out a given amount of data.
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Example_PlayAndStop_Code
 *  @code
 *  static I2S_Handle i2sHandle;
 *
 *  static uint16_t readBuf1[500]; // the data read will end up in this buffer
 *  static uint16_t readBuf2[500]; // the data read will end up in this buffer
 *  static uint16_t readBuf3[500]; // the data read will end up in this buffer
 *  static uint16_t writeBuf1[250] = {...some data...}; // this buffer will be sent out
 *  static uint16_t writeBuf2[250] = {...some data...}; // this buffer will be sent out
 *  static uint16_t writeBuf3[250] = {...some data...}; // this buffer will be sent out
 *
 *  static I2S_Transaction i2sRead1;
 *  static I2S_Transaction i2sRead2;
 *  static I2S_Transaction i2sRead3;
 *  static I2S_Transaction i2sWrite1;
 *  static I2S_Transaction i2sWrite2;
 *  static I2S_Transaction i2sWrite3;
 *
 *  List_List i2sReadList;
 *  List_List i2sWriteList;
 *
 *  static volatile bool readStopped = (bool)true;
 *  static volatile bool writeStopped = (bool)true;
 *
 *  static void writeCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
 *
 *      if(status & I2S_ALL_TRANSACTIONS_SUCCESS){
 *
 *          // Note: Here we do not queue new transfers or set a new queue-head.
 *          // The driver will stop sending out data on its own.
 *          writeStopped = (bool)true;
 *      }
 *  }
 *
 *  static void readCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
 *
 *      if(status & I2S_ALL_TRANSACTIONS_SUCCESS){
 *
 *          // Note: Here we do not queue new transfers or set a new queue-head.
 *          // The driver will stop receiving data on its own.
 *          readStopped = (bool)true;
 *      }
 *  }
 *
 *  static void errCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
 *
 *      // Handle the I2S error
 *  }
 *
 *  void *modePlayAndStopThread(void *arg0)
 *  {
 *      I2S_Params i2sParams;
 *
 *      I2S_init();
 *
 *      // Initialize I2S opening parameters
 *      I2S_Params_init(&i2sParams);
 *      i2sParams.fixedBufferLength     =  500; // fixedBufferLength is the greatest common
 *                                              // divisor of all the different buffers
 *                                              // (here buffers' size are 500 and 1000 bytes)
 *      i2sParams.writeCallback         =  writeCallbackFxn ;
 *      i2sParams.readCallback          =  readCallbackFxn ;
 *      i2sParams.errorCallback         =  errCallbackFxn;
 *
 *      i2sHandle = I2S_open(CONFIG_I2S0, &i2sParams);
 *
 *      // Initialize the read-transactions
 *      I2S_Transaction_init(&i2sRead1);
 *      I2S_Transaction_init(&i2sRead2);
 *      I2S_Transaction_init(&i2sRead3);
 *      i2sRead1.bufPtr            = readBuf1;
 *      i2sRead2.bufPtr            = readBuf2;
 *      i2sRead3.bufPtr            = readBuf3;
 *      i2sRead1.bufSize           = sizeof(readBuf1);
 *      i2sRead2.bufSize           = sizeof(readBuf2);
 *      i2sRead3.bufSize           = sizeof(readBuf3);
 *      List_put(&i2sReadList, (List_Elem*)&i2sRead1);
 *      List_put(&i2sReadList, (List_Elem*)&i2sRead2);
 *      List_put(&i2sReadList, (List_Elem*)&i2sRead3);
 *
 *      I2S_setReadQueueHead(i2sHandle, &i2sRead1);
 *
 *      // Initialize the write-transactions
 *      I2S_Transaction_init(&i2sWrite1);
 *      I2S_Transaction_init(&i2sWrite2);
 *      I2S_Transaction_init(&i2sWrite3);
 *      i2sWrite1.bufPtr           = writeBuf1;
 *      i2sWrite2.bufPtr           = writeBuf2;
 *      i2sWrite3.bufPtr           = writeBuf3;
 *      i2sWrite1.bufSize          = sizeof(writeBuf1);
 *      i2sWrite2.bufSize          = sizeof(writeBuf2);
 *      i2sWrite3.bufSize          = sizeof(writeBuf3);
 *      List_put(&i2sWriteList, (List_Elem*)&i2sWrite1);
 *      List_put(&i2sWriteList, (List_Elem*)&i2sWrite2);
 *      List_put(&i2sWriteList, (List_Elem*)&i2sWrite3);
 *
 *      I2S_setWriteQueueHead(i2sHandle, &i2sWrite1);
 *
 *      I2S_startClocks(i2sHandle);
 *      I2S_startWrite(i2sHandle);
 *      I2S_startRead(i2sHandle);
 *
 *      readStopped = (bool)false;
 *      writeStopped = (bool)false;
 *
 *      while (1) {
 *
 *          if(readStopped && writeStopped) {
 *              I2S_stopClocks(i2sHandle);
 *              I2S_close(i2sHandle);
 *              while (1);
 *          }
 *      }
 *  }
 *  @endcode
 *
 *  \note If you desire to put only one transaction in the queue, fixedBufferLength must be inferior to half the length (in bytes) of the buffer to transfer.
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Example_Streaming
 *  ### Writing Data in Continuous Streaming Mode #
 *  The following example shows how to read and write data in streaming mode.
 *  A dummy treatment of the data is also done.
 *  This example is not complete (semaphore and tasks creation are not shown)
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Example_Streaming_Code
 *  @code
 *  static I2S_Handle i2sHandle;
 *  static sem_t semDataReadyForTreatment;
 *
 *  // These buffers will successively be written, treated and sent out
 *  static uint16_t readBuf1[500];
 *  static uint16_t readBuf2[500];
 *  static uint16_t readBuf3[500];
 *  static uint16_t readBuf4[500];
 *  static uint16_t writeBuf1[500]={0};
 *  static uint16_t writeBuf2[500]={0};
 *  static uint16_t writeBuf3[500]={0};
 *  static uint16_t writeBuf4[500]={0};
 *
 *  // These transactions will successively be part of the
 *  // i2sReadList, the treatmentList and the i2sWriteList
 *  static I2S_Transaction i2sRead1;
 *  static I2S_Transaction i2sRead2;
 *  static I2S_Transaction i2sRead3;
 *  static I2S_Transaction i2sRead4;
 *  static I2S_Transaction i2sWrite1;
 *  static I2S_Transaction i2sWrite2;
 *  static I2S_Transaction i2sWrite3;
 *  static I2S_Transaction i2sWrite4;
 *
 *  List_List i2sReadList;
 *  List_List treatmentList;
 *  List_List i2sWriteList;
 *
 *  static void writeCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
 *
 *      // We must remove the previous transaction (the current one is not over)
 *      I2S_Transaction *transactionFinished = (I2S_Transaction*)List_prev(&transactionPtr->queueElement);
 *
 *      if(transactionFinished != NULL){
 *          // Remove the finished transaction from the write queue
 *          List_remove(&i2sWriteList, (List_Elem*)transactionFinished);
 *
 *          // This transaction must now feed the read queue (we do not need anymore the data of this transaction)
 *          transactionFinished->queueElement.next = NULL;
 *          List_put(&i2sReadList, (List_Elem*)transactionFinished);
 *
 *          // We need to queue a new transaction: let's take one in the treatment queue
 *          I2S_Transaction *newTransaction = (I2S_Transaction*)List_head(&treatmentList);
 *          if(newTransaction != NULL){
 *              List_remove(&treatmentList, (List_Elem*)newTransaction);
 *              newTransaction->queueElement.next = NULL;
 *              List_put(&i2sWriteList, (List_Elem*)newTransaction);
 *          }
 *      }
 *  }
 *
 *  static void readCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
 *
 *      // We must remove the previous transaction (the current one is not over)
 *      I2S_Transaction *transactionFinished = (I2S_Transaction*)List_prev(&transactionPtr->queueElement);
 *
 *      if(transactionFinished != NULL){
 *          // The finished transaction contains data that must be treated
 *          List_remove(&i2sReadList, (List_Elem*)transactionFinished);
 *          transactionFinished->queueElement.next = NULL;
 *          List_put(&treatmentList, (List_Elem*)transactionFinished);
 *
 *          // Start the treatment of the data
 *          sem_post(&semDataReadyForTreatment);
 *
 *          // We do not need to queue transaction here: writeCallbackFxn takes care of this :)
 *      }
 *  }
 *
 *  static void errCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
 *
 *      // Handle the I2S error
 *  }
 *
 *  void *echoExampleThread(void *arg0)
 *  {
 *      I2S_Params i2sParams;
 *
 *      I2S_init();
 *
 *      int retc = sem_init(&semDataReadyForTreatment, 0, 0);
 *      if (retc == -1) {
 *          while (1);
 *      }
 *
 *      // Initialize the treatmentList (this list is initially empty)
 *      List_clearList(&treatmentList);
 *
 *      //Initialize I2S opening parameters
 *      I2S_Params_init(&i2sParams);
 *      i2sParams.fixedBufferLength     =  1000;
 *      i2sParams.writeCallback         =  writeCallbackFxn ;
 *      i2sParams.readCallback          =  readCallbackFxn ;
 *      i2sParams.errorCallback         =  errCallbackFxn;
 *
 *      i2sHandle = I2S_open(CONFIG_I2S0, &i2sParams);
 *
 *      // Initialize the read-transactions
 *      I2S_Transaction_init(&i2sRead1);
 *      I2S_Transaction_init(&i2sRead2);
 *      I2S_Transaction_init(&i2sRead3);
 *      I2S_Transaction_init(&i2sRead4);
 *      i2sRead1.bufPtr            = readBuf1;
 *      i2sRead2.bufPtr            = readBuf2;
 *      i2sRead3.bufPtr            = readBuf3;
 *      i2sRead4.bufPtr            = readBuf4;
 *      i2sRead1.bufSize           = sizeof(readBuf1);
 *      i2sRead2.bufSize           = sizeof(readBuf2);
 *      i2sRead3.bufSize           = sizeof(readBuf3);
 *      i2sRead4.bufSize           = sizeof(readBuf4);
 *      List_clearList(&i2sReadList);
 *      List_put(&i2sReadList, (List_Elem*)&i2sRead1);
 *      List_put(&i2sReadList, (List_Elem*)&i2sRead2);
 *      List_put(&i2sReadList, (List_Elem*)&i2sRead3);
 *      List_put(&i2sReadList, (List_Elem*)&i2sRead4);
 *
 *      I2S_setReadQueueHead(i2sHandle, &i2sRead1);
 *
 *      // Initialize the write-transactions
 *      I2S_Transaction_init(&i2sWrite1);
 *      I2S_Transaction_init(&i2sWrite2);
 *      I2S_Transaction_init(&i2sWrite3);
 *      I2S_Transaction_init(&i2sWrite4);
 *      i2sWrite1.bufPtr           = writeBuf1;
 *      i2sWrite2.bufPtr           = writeBuf2;
 *      i2sWrite3.bufPtr           = writeBuf3;
 *      i2sWrite4.bufPtr           = writeBuf4;
 *      i2sWrite1.bufSize          = sizeof(writeBuf1);
 *      i2sWrite2.bufSize          = sizeof(writeBuf2);
 *      i2sWrite3.bufSize          = sizeof(writeBuf3);
 *      i2sWrite4.bufSize          = sizeof(writeBuf4);
 *      List_clearList(&i2sWriteList);
 *      List_put(&i2sWriteList, (List_Elem*)&i2sWrite1);
 *      List_put(&i2sWriteList, (List_Elem*)&i2sWrite2);
 *      List_put(&i2sWriteList, (List_Elem*)&i2sWrite3);
 *      List_put(&i2sWriteList, (List_Elem*)&i2sWrite4);
 *
 *      I2S_setWriteQueueHead(i2sHandle, &i2sWrite1);
 *
 *      I2S_startClocks(i2sHandle);
 *      I2S_startWrite(i2sHandle);
 *      I2S_startRead(i2sHandle);
 *
 *      while (1) {
 *          uint8_t k = 0;
 *          I2S_Transaction* lastAchievedReadTransaction = NULL;
 *
 *          retc = sem_wait(&semDataReadyForTreatment);
 *          if (retc == -1) {
 *              while (1);
 *          }
 *
 *          lastAchievedReadTransaction = (I2S_Transaction*) List_head(&treatmentList);
 *
 *          if(lastAchievedReadTransaction != NULL) {
 *
 *              // Need a critical section to be sure to have corresponding bufPtr and bufSize
 *              uintptr_t key = HwiP_disable();
 *              uint16_t *buf = lastAchievedReadTransaction->bufPtr;
 *              uint16_t bufLength = lastAchievedReadTransaction->bufSize / sizeof(uint16_t);
 *              HwiP_restore(key);
 *
 *              // My dummy data treatment...
 *              for(k=0; k<bufLength; k++) {
 *                  buf[k]--;
 *              }
 *              for(k=0; k<bufLength; k++) {
 *                  buf[k]++;
 *              }
 *          }
 *      }
 *  }
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Example_RepeatMode
 *  ### Writing Data in repeat Mode #
 *  The following example shows how to read and write data in repeat mode.
 *  The same buffers are continuously written and send out while the driver is not stopped.
 *  Here, we decide to only stop sending out after an arbitrary number of sending.
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Example_RepeatMode_Code
 *  @code
 *  static I2S_Handle i2sHandle;
 *
 *  // This buffer will be continuously re-written
 *  static uint16_t readBuf[500];
 *  // This data will be continuously sent out
 *  static uint16_t writeBuf[500] = {...some cool data...};
 *
 *  static I2S_Transaction i2sRead;
 *  static I2S_Transaction i2sWrite;
 *
 *  List_List i2sReadList;
 *  List_List i2sWriteList;
 *
 *  static volatile bool writeFinished = (bool)false;
 *  static void writeCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
 *
 *      // Nothing to do here: the buffer(s) are queued in a ring list, the transfers are
 *      // executed without any action from the application.
 *
 *      // We must consider the previous transaction (ok, when you have only one transaction it's the same)
 *      I2S_Transaction *transactionFinished = (I2S_Transaction*)List_prev(&transactionPtr->queueElement);
 *
 *      if(transactionFinished != NULL){
 *          // After an arbitrary number of completion of the transaction, we will stop writting
 *          if(transactionFinished->numberOfCompletions >= 10) {
 *
 *              // Note: You cannot use I2S_stopRead() / I2S_stopWrite() in the callback.
 *              // The execution of these functions is potentially blocking and can mess up the
 *              // system.
 *
 *              writeFinished = (bool)true;
 *          }
 *      }
 *  }
 *
 *  static void readCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
 *      // Nothing to do here: the buffer(s) are queued in a ring list, the transfers are
 *      // executed without any action from the application.
 *  }
 *
 *  static void errCallbackFxn(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr) {
 *
 *      // Handle the I2S error
 *  }
 *
 *  void *modeRepeat(void *arg0)
 *  {
 *      I2S_Params i2sParams;
 *
 *      // Initialize I2S opening parameters
 *      I2S_Params_init(&i2sParams);
 *      i2sParams.fixedBufferLength     =  1000; // No problem here: the driver consider
 *                                               // the list as an infinite list.
 *      i2sParams.writeCallback         =  writeCallbackFxn ;
 *      i2sParams.readCallback          =  readCallbackFxn ;
 *      i2sParams.errorCallback         =  errCallbackFxn;
 *
 *      i2sHandle = I2S_open(CONFIG_I2S0, &i2sParams);
 *
 *      // Initialize the read-transactions
 *      I2S_Transaction_init(&i2sRead);
 *      i2sRead.bufPtr            = readBuf;
 *      i2sRead.bufSize           = sizeof(readBuf);
 *      List_put(&i2sReadList, (List_Elem*)&i2sRead);
 *      List_tail(&i2sReadList)->next = List_head(&i2sReadList);// Read buffers are queued in a ring-list
 *      List_head(&i2sReadList)->prev = List_tail(&i2sReadList);
 *
 *      I2S_setReadQueueHead(i2sHandle, &i2sRead);
 *
 *      // Initialize the write-transactions
 *      I2S_Transaction_init(&i2sWrite);
 *      i2sWrite.bufPtr           = writeBuf;
 *      i2sWrite.bufSize          = sizeof(writeBuf);
 *      List_put(&i2sWriteList, (List_Elem*)&i2sWrite);
 *      List_tail(&i2sWriteList)->next = List_head(&i2sWriteList); // Write buffers are queued in a ring-list
 *      List_head(&i2sWriteList)->prev = List_tail(&i2sWriteList);
 *
 *      I2S_setWriteQueueHead(i2sHandle, &i2sWrite);
 *
 *      I2S_startClocks(i2sHandle);
 *      I2S_startWrite(i2sHandle);
 *      I2S_startRead(i2sHandle);
 *
 *      while (1) {
 *
 *          if(writeFinished){
 *              writeFinished = (bool)false;
 *              I2S_stopWrite(i2sHandle);
 *          }
 *      }
 *  }
 *  @endcode
 *
 *  @note In the case of circular lists, there is no problem to put only
 *  one buffer in the queue.
 *
 *  <hr>
 *  @anchor ti_drivers_I2S_Configuration
 *  # Configuration
 *
 *  Refer to the @ref driver_configuration "Driver's Configuration" section
 *  for driver configuration information.
 *  <hr>
 ******************************************************************************
 */

#ifndef ti_drivers_I2S__include
#define ti_drivers_I2S__include

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <ti/drivers/utils/List.h>


#ifdef __cplusplus
extern "C" {
#endif

 /**
  *  @defgroup I2S_STATUS Status Codes
  *  I2S_STATUS_* macros are general status codes used when user callback is called
  *  @{
  *  @ingroup I2S_CONTROL
  */

 /*!
  * @brief   Successful status code returned by I2S driver functions.
  *
  * I2S driver functions return I2S_ALL_TRANSACTION_SUCCESS if ALL the queued transactions
  * were executed successfully.
  */
 #define I2S_ALL_TRANSACTIONS_SUCCESS           (0x0001U)

 /*!
  * @brief   Successful status code returned by I2S driver functions.
  *
  * I2S driver functions return I2S_TRANSACTION_SUCCESS if ONE queued transaction
  * was executed successfully.
  */
 #define I2S_TRANSACTION_SUCCESS                (0x0002U)

 /*!
  * @brief   Error status code returned by I2S driver functions.
  *
  * I2S driver functions return I2S_TIMEOUT_ERROR if I2S module lost the audio clock.
  * If this error has been raised, I2S module must be reseted and restarted.
  */
 #define I2S_TIMEOUT_ERROR                      (0x0100U)

 /*!
  * @brief   Error status code returned by I2S driver functions.
  *
  * I2S driver functions return I2S_BUS_ERROR if I2S module faced problem with the DMA
  * bus (DMA transfer not completed in time).
  * If this error has been raised, I2S module must be reseted and restarted.
  */
 #define I2S_BUS_ERROR                          (0x0200U)

 /*!
  * @brief   Error status code returned by I2S driver functions.
  *
  * I2S driver functions return I2S_WS_ERROR if I2S module detect noise on the WS signal.
  * If this error has been raised, I2S module must be reseted and restarted.
  */
 #define I2S_WS_ERROR                           (0x0400U)

 /*!
  * @brief   Error status code returned by I2S driver functions.
  *
  * I2S driver functions return I2S_PTR_READ_ERROR if I2S module ran out of data
  * on the read interface (DMA pointer not loaded in time).
  * If this error has been raised, I2S module must be reseted and restarted.
  */
 #define I2S_PTR_READ_ERROR                     (0x0800U)

 /*!
  * @brief   Error status code returned by I2S driver functions.
  *
  * I2S driver functions return I2S_PTR_WRITE_ERROR if I2S module ran out of data
  * on the write interface (DMA pointer not loaded in time).
  * If this error has been raised, I2S module must be reseted and restarted.
  */
 #define I2S_PTR_WRITE_ERROR                     (0x1000U)
 /** @}*/

/*! @brief  I2S Global configuration
 *
 *  The I2S_Config structure contains a set of pointers used to characterize
 *  the I2S driver implementation.
 *
 *  This structure needs to be defined before calling I2S_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     I2S_init()
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void                   *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void          const    *hwAttrs;
} I2S_Config;

/*!
 *  @brief      A handle that is returned from a I2S_open() call.
 */
typedef I2S_Config *I2S_Handle;

/*!
 *  @brief I2S transaction descriptor.
 */
typedef struct {
    /*! Used internally to link descriptors together */
    List_Elem               queueElement;
    /*! Pointer to the buffer */
    void                    *bufPtr;
    /*! Size of the buffer. */
    size_t                  bufSize;
    /*! Internal use only. Number of bytes written to or read from the buffer. */
    size_t                  bytesTransferred;
    /*! Number of non-transfered bytes at transaction's end. */
    size_t                  untransferredBytes;
    /*! Parameter incremented each time the transaction is completed. */
    uint16_t                numberOfCompletions;
    /*! Internal argument. Application must not modify this element. */
    uintptr_t               arg;
} I2S_Transaction;

/*!
 *  @brief      The definition of a user-callback function used by the I2S driver
 *
 *  @param      I2S_Handle      I2S_Handle
 *
 *  @param      status          Status of the operation (possible values are :
 *                              :I2S_STATUS_SUCCESS, :I2S_STATUS_ERROR,
 *                              :I2S_STATUS_BUFFER_UNAVAILABLE, :I2S_STATUS_TIMEOUT)
 *
 *  @param      I2S_Transaction *transactionPtr: Pointer on the transaction that has just started.
 *                              For error callbacks, transactionPtr points on NULL.
 *
 */
typedef void (*I2S_Callback)(I2S_Handle handle, int_fast16_t status, I2S_Transaction *transactionPtr);

/*!
 *  @brief      The definition of a function used to set the I2S register
 *
 *  @param      uint32_t ui32Base: base address of the I2S module.
 *
 *  @param      uint32_t ui32NextPointer: pointer on an I2S buffer.
 *
 */
typedef void (*I2S_RegUpdate)(uint32_t ui32Base, uint32_t ui32NextPointer);

/*!
 *  @brief      The definition of a function used to stop an I2S interface
 *
 *  @param      I2S_Handle      I2S_Handle
 *
 */
typedef void (*I2S_StopInterface)(I2S_Handle handle);

/*!
 *  @brief      I2S slot memory length setting
 *
 *  The enum defines if the module uses a 16 bits or a 24 bits buffer in memory.
 *  This value has no influence on the number of bit transmitted.
 */
typedef enum {

    I2S_MEMORY_LENGTH_8BITS  =  8U,   /*!<    Buffer used is 8 bits length. Not available for CC26XX. */
    I2S_MEMORY_LENGTH_16BITS = 16U,   /*!<    Buffer used is 16 bits length. */
    I2S_MEMORY_LENGTH_24BITS = 24U,   /*!<    Buffer used is 24 bits length. */
    I2S_MEMORY_LENGTH_32BITS = 32U    /*!<    Buffer used is 32 bits length. Not available for CC26XX. */

} I2S_MemoryLength;

/*!
 *  @brief      I2S master / slave selection
 *
 *  The enum defines if the module acts like a master (clocks are internally generated)
 *  or a slave (the clocks are externally generated).
 */
typedef enum {

    I2S_SLAVE  = 0,    /*!<    Module is a slave, clocks are externally generated. */
    I2S_MASTER = 1     /*!<    Module is a master, clocks are internally generated. */

} I2S_Role;

/*!
 *  @brief      I2S sampling setting
 *
 *  The enum defines if sampling is done on BLCK rising or falling edges.
 */
typedef enum {

    I2S_SAMPLING_EDGE_FALLING  = 0,    /*!<    Sampling on falling edges. */
    I2S_SAMPLING_EDGE_RISING   = 1     /*!<    Sampling on rising edges. */

} I2S_SamplingEdge;

/*!
 *  @brief      I2S phase setting
 *
 *  The enum defines if the I2S if set with single or dual phase.
 */
typedef enum {

    I2S_PHASE_TYPE_SINGLE  = 0U,   /*!<    Single phase */
    I2S_PHASE_TYPE_DUAL    = 1U,   /*!<    Dual phase */

} I2S_PhaseType;

/*!
 *  @brief      I2S data interface configuration
 *
 *  The enum defines the different settings for the data interfaces (SD0 and SD1).
 */
typedef enum {

    I2S_SD0_DISABLED       = 0x00U,   /*!<    SD0 is disabled */
    I2S_SD0_INPUT          = 0x01U,   /*!<    SD0 is an input */
    I2S_SD0_OUTPUT         = 0x02U,   /*!<    SD0 is an output */
    I2S_SD1_DISABLED       = 0x00U,   /*!<    SD1 is disabled */
    I2S_SD1_INPUT          = 0x10U,   /*!<    SD1 is an input */
    I2S_SD1_OUTPUT         = 0x20U    /*!<    SD1 is an output */

} I2S_DataInterfaceUse;

/*!
 *  @brief      Channels used selection
 *
 *  The enum defines different settings to activate the expected channels.
 */
typedef enum {

    I2S_CHANNELS_NONE       = 0x00U,   /*!<   No channel activated */
    I2S_CHANNELS_MONO       = 0x01U,   /*!<   MONO: only channel one is activated */
    I2S_CHANNELS_MONO_INV   = 0x02U,   /*!<   MONO INVERERTED: only channel two is activated */
    I2S_CHANNELS_STEREO     = 0x03U,   /*!<   STEREO: channels one and two are activated */
    I2S_1_CHANNEL           = 0x01U,   /*!<   1 channel  is activated */
    I2S_2_CHANNELS          = 0x03U,   /*!<   2 channels are activated */
    I2S_3_CHANNELS          = 0x07U,   /*!<   3 channels are activated */
    I2S_4_CHANNELS          = 0x0FU,   /*!<   4 channels are activated */
    I2S_5_CHANNELS          = 0x1FU,   /*!<   5 channels are activated */
    I2S_6_CHANNELS          = 0x3FU,   /*!<   6 channels are activated */
    I2S_7_CHANNELS          = 0x7FU,   /*!<   7 channels are activated */
    I2S_8_CHANNELS          = 0xFFU,   /*!<   8 channels are activated */
    I2S_CHANNELS_ALL        = 0xFFU    /*!<   All the eight channels are activated */

} I2S_ChannelConfig;

/*!
 *  @brief    Basic I2S Parameters
 *
 *  I2S parameters are used to with the I2S_open() call. Default values for
 *  these parameters are set using I2S_Params_init().
 *
 *  @sa       I2S_Params_init()
 */
typedef struct {

    bool                  trueI2sFormat;
    /*!< Activate "true I2S format".
     *    false: Data are read/write on the data lines from the first SCK period of
     *           the WS half-period to the last SCK edge of the WS half-period.
     *    true:  Data are read/write on the data lines from the second SCK period of
     *           the WS half-period to the first SCK edge of the next WS half-period.
     *           If no padding is activated, this corresponds to the I2S standard. */

    bool                  invertWS;
    /*!< WS must be internally inverted when using I2S data format.
     *   false: The WS signal is not internally inverted.
     *   true:  The WS signal is internally inverted. */

    bool                  isMSBFirst;
    /*!< Endianness selection. Not available on CC26XX.
     *   false: The samples are transmitted LSB first.
     *   true:  The samples are transmitted MSB first. */

    bool                  isDMAUnused;
    /*!<  Selection between DMA transmissions and CPU transmissions.
     *   false: Transmission are performed by DMA.
     *   true:  Transmission are performed by CPU.
     *   Not available for CC26XX: all transmissions are performed by CPU. */

    I2S_MemoryLength      memorySlotLength;
    /*!< Memory buffer used.
     *   #I2S_MEMORY_LENGTH_8BITS:  Memory length is 8 bits (not available for CC26XX).
     *   #I2S_MEMORY_LENGTH_16BITS: Memory length is 16 bits.
     *   #I2S_MEMORY_LENGTH_24BITS: Memory length is 24 bits.
     *   #I2S_MEMORY_LENGTH_32BITS: Memory length is 32 bits (not available for CC26XX).*/

    uint8_t               beforeWordPadding;
    /*!< Number of SCK periods between the first WS edge and the MSB of the first audio channel data transferred during the phase.*/

    uint8_t               afterWordPadding;
    /*!< Number of SCK periods between the LSB of the an audio channel and the MSB of the next audio channel.*/

    uint8_t               bitsPerWord;
    /*!< Bits per sample (Word length): must be between 8 and 24 bits. */

    I2S_Role              moduleRole;
    /*!< Select if the I2S module is a Slave or a Master.
     *   - #I2S_SLAVE:  The device is a slave (clocks are generated externally).
     *   - #I2S_MASTER: The device is a master (clocks are generated internally). */

    I2S_SamplingEdge      samplingEdge;
    /*!< Select edge sampling type.
     *   - #I2S_SAMPLING_EDGE_FALLING: Sampling on falling edges (for DSP data format).
     *   - #I2S_SAMPLING_EDGE_RISING:  Sampling on rising edges (for I2S, LJF and RJF data formats). */

    I2S_DataInterfaceUse  SD0Use;
    /*!< Select if SD0 is an input, an output or disabled.
     *     - #I2S_SD0_DISABLED: Disabled.
     *     - #I2S_SD0_INPUT:    Input.
     *     - #I2S_SD0_OUTPUT:   Output. */

    I2S_DataInterfaceUse  SD1Use;
    /*!< Select if SD1 is an input, an output or disabled.
     *     - #I2S_SD1_DISABLED: Disabled.
     *     - #I2S_SD1_INPUT:    Input.
     *     - #I2S_SD1_OUTPUT:   Output. */

    I2S_ChannelConfig     SD0Channels;
    /*!< This parameter is a bit mask indicating which channels are valid on SD0.
     *   If phase type is "dual", maximum channels number is two.
     *   Valid channels on SD1 and SD0 can be different.
     *   For dual phase mode:
     *   - #I2S_CHANNELS_NONE: No channel activated:
     *                           read  -> I2S does not receive anything (no buffer consumption)
     *                           write -> I2S does not send anything (no buffer consumption)
     *   - #I2S_CHANNELS_MONO: Only channel 1 is activated:
     *                           read  -> I2S only reads channel 1
     *                           write -> I2S transmits the data on channel 1 and duplicates it on channel 2
     *   - #I2S_CHANNELS_MONO_INV: Only channel 2 is activated:
     *                           read  -> I2S only reads channel 2
     *                           write -> I2S transmits the data on channel 2 and duplicates it on the channel 1 of the next word
     *   - #I2S_CHANNELS_STEREO: STEREO:
     *                           read  -> I2S reads both channel 1 and channel 2
     *                           write -> I2S transmits data both on channel 1 and channel 2
     *   .
     *   For single phase mode:
     *   - Various number of channels can be activated using: #I2S_1_CHANNEL, #I2S_2_CHANNELS, #I2S_3_CHANNELS, #I2S_4_CHANNELS,
     *     #I2S_5_CHANNELS, #I2S_6_CHANNELS, #I2S_7_CHANNELS, #I2S_8_CHANNELS.
     *   - #I2S_CHANNELS_ALL: The eight channels are activated */

    I2S_ChannelConfig     SD1Channels;
    /*!< This parameter is a bit mask indicating which channels are valid on SD1.
     *   If phase type is "dual", maximum channels number is two.
     *   Valid channels on SD1 and SD0 can be different.
     *   For dual phase mode:
     *   - #I2S_CHANNELS_NONE: No channel activated:
     *                           read  -> I2S does not receive anything (no buffer consumption)
     *                           write -> I2S does not send anything (no buffer consumption)
     *   - #I2S_CHANNELS_MONO: Only channel 1 is activated:
     *                           read  -> I2S only reads channel 1
     *                           write -> I2S transmits the data on channel 1 and duplicates it on channel 2
     *   - #I2S_CHANNELS_MONO_INV: Only channel 2 is activated:
     *                           read  -> I2S only reads channel 2
     *                           write -> I2S transmits the data on channel 2 and duplicates it on the channel 1 of the next word
     *   - #I2S_CHANNELS_STEREO: STEREO:
     *                           read  -> I2S reads both channel 1 and channel 2
     *                           write -> I2S transmits data both on channel 1 and channel 2
     *   .
     *   For single phase mode:
     *   - Various number of channels can be activated using: #I2S_1_CHANNEL, #I2S_2_CHANNELS, #I2S_3_CHANNELS, #I2S_4_CHANNELS,
     *     #I2S_5_CHANNELS, #I2S_6_CHANNELS, #I2S_7_CHANNELS, #I2S_8_CHANNELS.
     *   - #I2S_CHANNELS_ALL: The eight channels are activated */

    I2S_PhaseType         phaseType;
    /*!< Select phase type.
     *   - #I2S_PHASE_TYPE_SINGLE: Single phase (for DSP format): up to eight channels are usable.
     *   - #I2S_PHASE_TYPE_DUAL:   Dual phase (for I2S, LJF and RJF data formats): up to two channels are usable.
     *   .
     *   This parameter must not be considered on CC32XX. This chip only allows dual phase formats.*/

    uint16_t              fixedBufferLength;
    /*!< Number of consecutive bytes of the samples buffers. This field must be set to a value x different from 0.
     *   All the data buffers used (both for input and output) must contain N*x bytes (with N an integer verifying N>0). */

    uint16_t              startUpDelay;
    /*!< Number of WS periods to wait before the first transfer. */

    uint16_t              MCLKDivider;
    /*!< Select the frequency divider for MCLK signal. Final value of MCLK is 48MHz/MCLKDivider. Value must be selected between 2 and 1024. */

    uint32_t              samplingFrequency;
    /*!< I2S sampling frequency configuration in samples/second.
     *  SCK frequency limits:
     *- For CC26XX, SCK frequency should be between 47 kHz and 4 MHz.
     *- For CC32XX, SCK frequency should be between 57 Hz and 8 MHz. */

    I2S_Callback          readCallback;
    /*!< Pointer to read callback. Cannot be NULL if a read interface is activated. */

    I2S_Callback          writeCallback;
    /*!< Pointer to write callback. Cannot be NULL if a write interface is activated. */

    I2S_Callback          errorCallback;
    /*!< Pointer to error callback. Cannot be NULL. */

    void                  *custom;
    /*!< Pointer to device specific custom params */
} I2S_Params;

/*!
 *  @brief Default I2S_Params structure
 *
 *  @sa     I2S_Params_init()
 */
extern const I2S_Params I2S_defaultParams;

/*!
 *  @brief  Function to close a given I2S peripheral specified by the I2S
 *  handle.
 *
 *  @pre    I2S_open() had to be called first.
 *
 *  @param  [in]    handle  An I2S_Handle returned from I2S_open
 *
 *  @sa     I2S_open()
 */
extern void I2S_close(I2S_Handle handle);

/*!
 *  @brief  Function to initializes the I2S module
 *
 *  @pre    The I2S_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other I2S driver APIs. This function call does not modify any
 *          peripheral registers.
 */
extern void I2S_init(void);

/*!
 *  @brief  Function to initialize a given I2S peripheral specified by the
 *          particular index value. The parameter specifies which mode the I2S
 *          will operate.
 *
 *  @pre    I2S controller has been initialized
 *
 *  @param  [inout] index   Logical peripheral number for the I2S indexed into
 *                          the I2S_config table
 *
 *  @param  [in]    params  Pointer to an parameter block.
 *                          All the fields in this structure are RO (read-only).
 *                          Provide a NULL pointer cannot open the module.
 *
 *  @return An I2S_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa     I2S_init()
 *  @sa     I2S_close()
 */
extern I2S_Handle I2S_open(uint_least8_t index, I2S_Params *params);

/*!
 *  @brief  Function to initialize the I2S_Params struct to its defaults
 *
 *  @param  [out]   params  An pointer to I2S_Params structure for
 *                          initialization
 *
 *  Defaults values are:
 *  @code
 *  params.samplingFrequency    = 8000;
 *  params.memorySlotLength     = I2S_MEMORY_LENGTH_16BITS;
 *  params.moduleRole           = I2S_MASTER;
 *  params.trueI2sFormat        = (bool)true;
 *  params.invertWS             = (bool)true;
 *  params.isMSBFirst           = (bool)true;
 *  params.isDMAUnused          = (bool)false;
 *  params.samplingEdge         = I2S_SAMPLING_EDGE_RISING;
 *  params.beforeWordPadding    = 0;
 *  params.bitsPerWord          = 16;
 *  params.afterWordPadding     = 0;
 *  params.fixedBufferLength    = 1;
 *  params.SD0Use               = I2S_SD0_OUTPUT;
 *  params.SD1Use               = I2S_SD1_INPUT;
 *  params.SD0Channels          = I2S_CHANNELS_STEREO;
 *  params.SD1Channels          = I2S_CHANNELS_STEREO;
 *  params.phaseType            = I2S_PHASE_TYPE_DUAL;
 *  params.startUpDelay         = 0;
 *  params.MCLKDivider          = 40;
 *  params.readCallback         = NULL;
 *  params.writeCallback        = NULL;
 *  params.errorCallback        = NULL;
 *  params.custom               = NULL;
 *  @endcode
 *
 *  @param  params  Parameter structure to initialize
 */
extern void I2S_Params_init(I2S_Params *params);

/*!
 *  @brief   Initialize an I2S_Transaction struct to known state.
 *
 *  The I2S_Transaction struct is put in a known state. The application is
 *  still responsible for populating some of the fields.
 *  For example, the user is responsible to provide the buffer containing the
 *  data and the size of it.
 *  User provided buffer's size must matche with the I2S settings.
 *  If the buffer size is not adapted, the I2S module will truncate it.
 *  Authorized buffer sizes depend on the number of activated outputs, the number
 *  of channels activated, the memory slots length (16 or 24 bits), and the
 *  fixed-buffer-size eventually provided.
 *  Authorized buffer sizes are all the multiple values of the value of
 *  handle->object->memoryStepOut.
 *
 *  @param   [out]    transaction Transaction struct to initialize.
 */
extern void I2S_Transaction_init(I2S_Transaction *transaction);

/*!
 *  @brief Function to set the first read-transaction to consider
 *
 *  At the end of each transaction, I2S driver takes in consideration the next
 *  transaction. Application is responsible to handle the queue.
 *
 *  @param  [in]    handle  An I2S_Handle.
 *
 *  @param  [in]    transaction A pointer to an I2S_Transaction object. The bufPtr
 *                              and bufSize fields must be set to a buffer and the
 *                              size of the buffer before passing to this function.
 *
 *  @return void
 *
 *  @sa I2S_setWriteQueueHead()
 */
extern void I2S_setReadQueueHead(I2S_Handle handle, I2S_Transaction *transaction);

/*!
 *  @brief Function to set the first write-transaction to consider
 *
 *  At the end of each transaction, I2S driver takes in consideration the next
 *  transaction. Application is responsible to handle the queue.
 *
 *  @param  [in]    handle  An I2S_Handle.
 *
 *  @param  [in]    transaction A pointer to an I2S_Transaction object. The bufPtr
 *                              and bufSize fields must be set to a buffer and the
 *                              size of the buffer before passing to this function.
 *
 *  @return void
 *
 *  @sa I2S_setReadQueueHead()
 */
extern void I2S_setWriteQueueHead(I2S_Handle handle, I2S_Transaction *transaction);

/*!
 *  @brief Start the WS, SCK and MCLK clocks.
 *
 *  This function enable WS, SCK and MCLK (if activated) clocks. This is required before starting
 *  any reading or a writing transaction.
 *  This function is supposed to be executed both in slave and master mode.
 *
 *  @param  [in]    handle  An I2S_Handle.
 *
 *  @return void
 *
 *  @sa I2S_stopClocks()
 */
extern void I2S_startClocks(I2S_Handle handle);

/*!
 *  @brief Stops the WS, SCK and MCLK clocks.
 *
 *  This function disable WS, SCK and MCLK clocks.
 *  This function must be executed only if no transaction is in progress.
 *  This function is supposed to be executed in a Task context (NOT in a HWI or Callback context).
 *  This function is supposed to be executed both in slave and master mode.
 *
 *  @warning This function is supposed to be executed in a Task context
 *  (NOT in a HWI or Callback context).
 *
 *  @param  [in]    handle  An I2S_Handle.
 *
 *  @return void
 *
 *  @sa I2S_stopRead()
 *  @sa I2S_stopWrite()
 */
extern void I2S_stopClocks(I2S_Handle handle);

/*!
 *  @brief Start read transactions.
 *
 *  This function starts reception of the transactions stored in the read-queue.
 *  and returns immediately. At the completion of each transaction the readCallback
 *  provided is executed.
 *
 *  If the queue for read transactions becomes empty (i.e. the read Callback is
 *  triggered with status #I2S_ALL_TRANSACTIONS_SUCCESS and the application has
 *  not queued new transactions or defined a new first read-transaction to consider
 *  using #I2S_setReadQueueHead()), the driver will stop the read interface on its
 *  own in order to avoid the occurrence of errors (such as #I2S_PTR_READ_ERROR).
 *
 *  @pre First read-transaction to consider must be set and clocks must be running
 *  before calling this function.
 *
 *  @param  [in]    handle  An I2S_Handle.
 *
 *  @return void
 *
 *  @sa I2S_startClocks()
 *  @sa I2S_setReadQueueHead()
 */
extern void I2S_startRead(I2S_Handle handle);

/*!
 *  @brief Start write transactions.
 *
 *  This function starts transmission of the transactions stored in the write-queue
 *  and returns immediately. At the completion of each transaction the write Callback
 *  provided is executed.
 *
 *  If the queue for write transactions becomes empty (i.e. the write Callback is
 *  triggered with status #I2S_ALL_TRANSACTIONS_SUCCESS and the application has not
 *  queued new transactions or defined a new first write-transaction to consider using
 *  #I2S_setWriteQueueHead()), the driver will stop the write interface on its own in
 *  order to avoid the occurrence of errors (such as #I2S_PTR_WRITE_ERROR).
 *
 *  @pre First write-transaction to consider must be set and clocks must be running
 *  before calling this function.
 *
 *  @param  [in]    handle  An I2S_Handle.
 *
 *  @return void
 *
 *  @sa I2S_startClocks()
 *  @sa I2S_setWriteQueueHead()
 */
extern void I2S_startWrite(I2S_Handle handle);

/*!
 *  @brief Stop read transactions.
 *
 *  This function stops the reception of read transactions correctly so that
 *  read operations can be safely restarted later.
 *
 *  The application can decide at any time to suspend the reception of data by calling
 *  this function. In this case (and because the transaction queue is not empty) the
 *  execution of #I2S_stopRead() is blocked until the current transaction is completed
 *  (this ensures that the I2S read interface is correctly stopped). Therefore, this
 *  function must be executed in a Task context (not in a HWI or Callback context).
 *
 *  After the transfers have been stopped (either by calling #I2S_stopRead() or because
 *  the queue has been empty), the application can resume the transfers using the
 *  function #I2S_startRead(). If the read-queue was empty application must beforehand
 *  set the first read-transaction using #I2S_setReadQueueHead().
 *
 *  @param  [in]    handle  An I2S_Handle.
 *
 *  @return void
 */
extern void I2S_stopRead(I2S_Handle handle);

/*!
 *  @brief Stop write transactions.
 *
 *  This function stops the transmission of write transactions correctly so that
 *  writing operations can be safely restarted later.
 *
 *  The application can decide at any time to suspend the sending of data by calling
 *  this function. In this case (and because the transaction queue is not empty) the
 *  execution of #I2S_stopWrite() is blocked until the current transaction is completed
 *  (this ensures that the I2S write interface is correctly stopped). Therefore, this
 *  function must be executed in a Task context (not in a HWI or Callback context).
 *
 *  After the transfers have been stopped (either by calling #I2S_stopWrite() or because
 *  the queue has been empty), the application can resume the transfers using the
 *  function #I2S_startWrite(). If the write-queue was empty application must beforehand
 *  set the first write-transaction using #I2S_setWriteQueueHead().
 *
 *  @param  [in]    handle  An I2S_Handle.
 *
 *  @return void
 */
extern void I2S_stopWrite(I2S_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_I2S__include */
