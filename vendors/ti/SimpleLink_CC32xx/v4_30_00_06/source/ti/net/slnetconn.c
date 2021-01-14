/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
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

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

/* POSIX Header files */
#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>

#include <ti/net/slnetconn.h>
#include <ti/net/slnetif.h>

/* Common interface includes                                                 */

/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/

#define CONN_MNG_MQ_MAXMSG                  (10)
#define SLNETCONN_SERVICE_LVL_MAX            (SLNETCONN_SERVICE_LVL_INTERNET+1)
#define SLNETCONN_STATUS_MAX                 (SLNETCONN_STATUS_CONNECTED_INTERNET+1)

#define VERIFY_INITIALIZED()                if(g_ctx.status == 0) { return SLNETERR_API_ABORTED; }
#define VERIFY_NOT_INITIALIZED()            if(g_ctx.status > 0)  { return SLNETERR_RET_CODE_DEV_ALREADY_STARTED; }
#define VERIFY_INPUT_SERVICE_LEVEL(SRVLEVL) if((uint32_t)SRVLEVL > (uint32_t)SLNETCONN_SERVICE_LVL_INTERNET) \
                                                                  { return SLNETERR_RET_CODE_INVALID_INPUT; }

/* Critical Section Lock */
#define CS_INIT()                           sem_init(&g_ctx.csLock, 0, 1)
#define CS_LOCK()                           sem_wait(&g_ctx.csLock)
#define CS_UNLOCK()                         sem_post(&g_ctx.csLock)
#define CS_DEINIT()                         sem_destroy(&g_ctx.csLock)

/*****************************************************************************/
/* Structure/Enum declarations                                               */
/*****************************************************************************/

/* Blocking Semaphore List Node */
typedef struct SemEntry
{
    sem_t    sem;
    struct   SemEntry *next;
} SemEntry_t;

/* User (callback) List Node */
typedef struct
{
    SlNetConn_AppEvent_f    userCallback;
    SlNetConnStatus_e       targetStatus;
    uint8_t                 next;
} UserEntry_t;

typedef struct
{
    UserEntry_t             db[SLNETCONN_USERS_NUM];
    uint8_t                 freeFirst;
    uint8_t                 usersFirst;
    uint8_t                 numPerLevel[SLNETCONN_SERVICE_LVL_MAX]; // Per level
    uint8_t                 totalNum;
} Users_t;

/* User (callback) List Node */
typedef struct NetIfEntry
{
    uint16_t                ifId;
    SlNetConnStatus_e       status;
} NetIfEntry_t;

/* SlNetConn MsgQ message */
typedef struct
{
    uint16_t                ifId;
    SlNetIfEventId_e        netIfEvent;
    void                    *pData;
} slNetConnMsg_t;

/* SlNetConn Context */
typedef struct
{
    uint32_t                ifBitmap;
    SlNetConnStatus_e       status;
    SlNetConnStatus_e       targetStatus;

    /* Message Queue */
    mqd_t                   msgQ;

    /* SlNetConn - Crtitical Section protection */
    sem_t                   csLock;

    /* Array of interface for managing interface specific status */
    NetIfEntry_t            NetIfList[SLNETCONN_NETWORK_INTERFACES_NUM];
    int                     NetIfCount; /* Number of active interfaces */

    /* Linked list of semaphores to control each connection level separately */
    SemEntry_t              *semList[SLNETCONN_STATUS_MAX];

    /* User (callback) Linked list - storing user request info */
    Users_t                 users;

    /* Internet Server Parameters */
    union
    {
        uint16_t            addFamily;
        SlNetSock_AddrIn_t  ipV4Addr;
        SlNetSock_AddrIn6_t ipV6Addr;
        SlNetSock_Addr_t    ipAddr;
    };
    SlNetSocklen_t          addrLen;
} SlNetConn_t;


/*****************************************************************************/
/* Static Variables                                                          */
/*****************************************************************************/

/* SlNetConn control block variables, keeps all current configurations
   Variable is NULL when the layer isn't initialized.                        */
static SlNetConn_t g_ctx = {0};

#if SLNETCONN_DEBUG_ENABLE
static void DummyPrintf(const char *pFormat, ...) {};
SlNetConn_Debug_f g_fDebugPrint = DummyPrintf;

/* slnetconn status names (for debug messages) */
static const char *strStatus[] =
{
 "ERROR",
 "DISCONNECTED",
 "WAITING_FOR_CONN",
 "CONNECTED",
 "CONNECTED_IP",
 "CONNECTED_INTERNET"
};

/* slnetconn events names (for debug messages) */
static const char *strEvent[] =
{
 "DISC",
 "CONN",
 "IP"
};
#else
#define g_fDebugPrint(...)
#endif

/*****************************************************************************/
/* Internal function prototypes                                              */
/*****************************************************************************/
static int32_t SlNetIfEventHandler(SlNetIfEventId_e status, uint16_t ifID, void* pData);
static int32_t RegisterSem(SlNetConnStatus_e targetStatus , SemEntry_t* SemNode);
static int32_t UnRegisterSem(SlNetConnStatus_e targetStatus, SemEntry_t* SemNode);
static int32_t ReleaseSem(SlNetConnStatus_e targetStatus);
static int32_t WaitForStatus(SlNetConnStatus_e targetStatus, SemEntry_t *pSemNode, uint32_t timeout);
static int32_t EnableIF(uint32_t ifBitmap);
static int32_t DisableIF();
static SlNetConnStatus_e CheckInternetConnection(void);
static void    SetStatus(SlNetConnStatus_e status, uint16_t ifID);
static int32_t UsersInit();
static int32_t UsersRegister(SlNetConnStatus_e status, SlNetConn_AppEvent_f userCallback);
static int32_t UsersUnRegister(SlNetConn_AppEvent_f userCallback);
static int32_t UsersNotify(uint16_t ifID, SlNetConnStatus_e status, void *pData);


/*****************************************************************************/
/* SlNetConn Internal (static) function                                      */
/*****************************************************************************/

//*****************************************************************************
//
// SetStatus - Update the slnetconn main status
//
//*****************************************************************************
static void    SetStatus(SlNetConnStatus_e status, uint16_t ifId)
{
    if(ifId == 0)
    {
        /* (Initial) DISCONNECTED and CONNECTED_INTERNET only impact the global
         * status but not the specific interfaces */
        g_ctx.status = status;
    }
    else
    {
        int i;

        g_ctx.status = SLNETCONN_STATUS_DISCONNECTED;
        for(i=0; i<g_ctx.NetIfCount; i++)
        {
            /* Update the interface status */
            if(g_ctx.NetIfList[i].ifId == ifId)
            {
                g_ctx.NetIfList[i].status = status;
            }
            /* Now Update the global status (max of all interfaces) */
            if(g_ctx.NetIfList[i].status > g_ctx.status)
            {
                g_ctx.status = g_ctx.NetIfList[i].status;
            }
        }
    }
    g_fDebugPrint("SlNetConn(SM):: %s(%d) ==> %s\n\r", strStatus[status], ifId, strStatus[g_ctx.status]);
}

//*****************************************************************************
//
// ConvertServiceLevelToStatus - convert Service Level (from user request)
//                               to slnetconn status (to wait for).
//
//*****************************************************************************
static SlNetConnStatus_e ConvertServiceLevelToStatus(SlNetConnServiceLevel_e targetServiceLevel)
{
    return (SlNetConnStatus_e)((uint32_t)SLNETCONN_STATUS_CONNECTED + (uint32_t)targetServiceLevel);
}

//*****************************************************************************
//
// RegisterSem - Add semaphore node to relevance (connection level)
//                         list
//
// NOTE: This should be called with slNetConn mutex is locked
//
//*****************************************************************************
static int32_t RegisterSem(SlNetConnStatus_e targetStatus , SemEntry_t *pSemNode)
{
    int32_t retVal = sem_init(&pSemNode->sem, 0, 0);
    if (0 != retVal)
    {
        return SLNETERR_RET_CODE_MUTEX_CREATION_FAILED;
    }
    pSemNode->next = g_ctx.semList[targetStatus];
    g_ctx.semList[targetStatus] = pSemNode;

    return SLNETERR_RET_CODE_OK;
}

//*****************************************************************************
//
// UnRegisterSem - Remove semaphore node from relevance (connection level)
//                       list
//
// NOTE: This should be called with slNetConn mutex is locked
//
//*****************************************************************************
static int32_t UnRegisterSem(SlNetConnStatus_e targetStatus, SemEntry_t *pSemNode)
{
    SemEntry_t* pPrev = NULL;
    SemEntry_t* pCurr = g_ctx.semList[targetStatus];

    while(pCurr != NULL)
    {
        if(pSemNode == pCurr)
        {
            if(pPrev == NULL)
            {
                g_ctx.semList[targetStatus] = pSemNode->next;
            }
            else
            {
                pPrev->next = pSemNode->next;
            }
            sem_destroy(&pSemNode->sem);
            return SLNETERR_RET_CODE_OK;
        }
        pPrev = pCurr;
        pCurr = pCurr->next;
    }
    return SLNETERR_RET_CODE_EVENT_LINK_NOT_FOUND;
}

//*****************************************************************************
//
// ReleaseSem - Release all semaphore nodes of connection level
//                        and above
//
// NOTE: This should be called with slNetConn mutex is locked
//
//*****************************************************************************
static int32_t ReleaseSem(SlNetConnStatus_e targetStatus)
{
    SemEntry_t* pNode = g_ctx.semList[targetStatus];

    while(pNode != NULL)
    {
        g_fDebugPrint("SlNetConn: SEM Post (%x)\n\r", (int32_t)&pNode->sem);
        sem_post(&pNode->sem);
        pNode = pNode->next;
    }
    return SLNETERR_RET_CODE_OK;
}

//*****************************************************************************
//
// RegisterSem - Add semaphore node to relevance (connection level)
//                         list
//
// NOTE: This should be called with slNetConn mutex is locked
//
//*****************************************************************************
static int32_t UsersInit()
{
    int i;

    for(i=0; i<SLNETCONN_USERS_NUM; i++)
    {
        g_ctx.users.db[i].next = i+1;
    }
    g_ctx.users.freeFirst = 0;
    g_ctx.users.usersFirst = SLNETCONN_USERS_NUM;
    return 0;
}

#if SLNETCONN_DEBUG_ENABLE
void UsersDebug(char *prefix)
{
    int i;
    g_fDebugPrint("%s:: Users(%d)=", prefix, g_ctx.users.totalNum);
    i=g_ctx.users.usersFirst;
    while(i<SLNETCONN_USERS_NUM)
    {
        g_fDebugPrint("%d, ", i);
        i = g_ctx.users.db[i].next;
    }
    g_fDebugPrint("   Free=");
    i=g_ctx.users.freeFirst;
    while(i<SLNETCONN_USERS_NUM)
    {
        g_fDebugPrint("%d, ", i);
        i = g_ctx.users.db[i].next;
    }
    g_fDebugPrint("\n\r   target status=%s\n\r", g_ctx.targetStatus);
}
#endif

//*****************************************************************************
//
// UsersRegister - Register a user's connection request
//
// NOTE: This should be called with slNetConn mutex is locked
//
//*****************************************************************************
static int32_t UsersRegister(SlNetConnStatus_e targetStatus , SlNetConn_AppEvent_f userCallback)
{
    uint8_t userId;
    UserEntry_t *pUserEntry;
    int retVal = SLNETERR_RET_CODE_OK;

    /* Detach from free list */
    userId = g_ctx.users.freeFirst;
    pUserEntry = &g_ctx.users.db[userId];
    assert(userId < SLNETCONN_USERS_NUM);
    g_ctx.users.freeFirst = pUserEntry->next;

    /* Attach to (head of) User list */
    pUserEntry->next = g_ctx.users.usersFirst;
    g_ctx.users.usersFirst = userId;

    /* Set user context */
    pUserEntry->userCallback = userCallback;
    pUserEntry->targetStatus = targetStatus;


    /* Update number of users */
    g_ctx.users.numPerLevel[targetStatus-SLNETCONN_STATUS_CONNECTED]++;
    g_ctx.users.totalNum ++;

    /* Update global target status */
    if(targetStatus >= g_ctx.targetStatus)
    {
        g_ctx.targetStatus = targetStatus;
    }
    return retVal;
}


//*****************************************************************************
//
// UsersUnRegister - Unregister a user's connection request
//
// NOTE: This should be called with slNetConn mutex is locked
//
//*****************************************************************************
static int32_t UsersUnRegister(SlNetConn_AppEvent_f userCallback)
{
    uint8_t curr = g_ctx.users.usersFirst;
    UserEntry_t *pPrev = NULL;
    int found = 0;

    while(curr != SLNETCONN_USERS_NUM)
    {
        UserEntry_t *pCurr = &g_ctx.users.db[curr];

        if(userCallback == pCurr->userCallback)
        {
            int i;
            found = 1;
            /* detach from user list */
            if(pPrev)
            {
                /* Remove an item from the middle of the list */
                pPrev->next = pCurr->next;
            }
            else
            {
                /* Remove the first item of the list */
                g_ctx.users.usersFirst = pCurr->next;
            }
            /* add item to the (head of free list */
            pCurr->next = g_ctx.users.freeFirst;
            g_ctx.users.freeFirst = curr;

            /* add item to the (head of free list */
            pCurr->userCallback = NULL;

            /* Update number of users */
            g_ctx.users.totalNum --;
            g_ctx.users.numPerLevel[pCurr->targetStatus - SLNETCONN_STATUS_CONNECTED]--;

            /* Update global target status */
            g_ctx.targetStatus = SLNETCONN_STATUS_DISCONNECTED;
            for(i=SLNETCONN_SERVICE_LVL_INTERNET; i>=SLNETCONN_SERVICE_LVL_MAC; i--)
            {
                if(g_ctx.users.numPerLevel[i])
                {
                    g_ctx.targetStatus = (SlNetConnStatus_e)(SLNETCONN_STATUS_CONNECTED + i);
                }
            }
        }
        pPrev = pCurr;
        curr = pCurr->next;
    }
    if(found)
    {
        return SLNETERR_RET_CODE_OK;
    }
    else
    {
        return SLNETERR_RET_CODE_EVENT_LINK_NOT_FOUND;
    }
}

//*****************************************************************************
//
// UsersNotify - Invoke users callbacks (registered to the status event)
//
//*****************************************************************************
static int32_t UsersNotify(uint16_t ifID, SlNetConnStatus_e status, void *pData)
{
    int i = 0;
    SlNetConn_AppEvent_f userCBs[SLNETCONN_USERS_NUM];

    CS_LOCK();

    uint8_t curr = g_ctx.users.usersFirst;

    while(curr != SLNETCONN_USERS_NUM)
    {
        UserEntry_t *pCurr = &g_ctx.users.db[curr];
        if((status <= pCurr->targetStatus) && (pCurr->userCallback != 0))
        {
            userCBs[i++] = pCurr->userCallback;
        }
        curr = pCurr->next;
    }
    CS_UNLOCK();

   /* Calls the application callback from a non critical section (to enable
     * re-entrency) */
    for( ; i > 0 ; i--)
    {
        (userCBs[i-1])(ifID, status, pData);
    }

    return SLNETERR_RET_CODE_OK;
}

//*****************************************************************************
//
// WaitForStatus - Waiting for
//
// NOTE: This is called form a context where the slNetConn mutex is locked
//      (the mutex gets unlocked before the wait, but will be locked again
//       before the function returns)
//
//*****************************************************************************
static int32_t WaitForStatus(SlNetConnStatus_e targetStatus, SemEntry_t *pSemNode, uint32_t timeout)
{
    int32_t retVal = 0;
    struct timespec absTime;
    int semRetVal;

    g_fDebugPrint("SlNetConn: SEM Wait (%x)\n\r", (int32_t)&pSemNode->sem);
    CS_UNLOCK();

    /* Set time variable from input timeout                                 */
    clock_gettime(CLOCK_REALTIME, &absTime);
    absTime.tv_sec += timeout;

    /* Wait for connection to be establish                                          */
    semRetVal = sem_timedwait(&pSemNode->sem, &absTime);

    CS_LOCK();
    g_fDebugPrint("SlNetConn: SEM Unregister (%x, %d)\n\r", pSemNode->sem, semRetVal);

    if (semRetVal != 0)
    {
        g_fDebugPrint("SlNetConn: Timeout (%d)\n\r", semRetVal);
        /* Timeout expired. Create and send event message to connection
               manager thread. Connection manager will free the semaphore allocations  */
        retVal = 1;
    }
    return retVal;
}

//*****************************************************************************
//
// EnableIF - Loop through the available interface and enable each of them
//
//*****************************************************************************
static int32_t EnableIF(uint32_t ifBitmap)
{
    int32_t     retVal = SLNETERR_BAD_INTERFACE;
    SlNetIf_t   *netIf;
    g_ctx.NetIfCount = 0;

    /* When ifBitmap is 0, that means automatic selection of all interfaces
       is required, enable all bits in ifBitmap                              */
    if (0 == ifBitmap)
    {
        ifBitmap = ~(ifBitmap);
    }
    /* This loop tries to create a connection on the required interface with the
       required queryFlags.
       When multiple interfaces, in addition to the queryFlags it will try
       to create the connection on the interface with the highest priority       */
    do
    {
        /* Search for the highest priority interface according to the
           ifBitmap and the queryFlags                                       */
        netIf = SlNetIf_queryIf(ifBitmap, 0);
        /* Check if the function returned NULL or the requested interface
           exists                                                            */
        if(netIf)
        {
            int lRetVal;
            /* Disable the ifID bit from the ifBitmap after finding the
               netIf                                                         */
            ifBitmap &= ~(netIf->ifID);

            if(g_ctx.NetIfCount == SLNETCONN_NETWORK_INTERFACES_NUM)
            {
                /* Too many interfaces in bitmask - SLNETCONN_NETWORK_INTERFACES_NUM may need to grow */
                DisableIF();
                return SLNETERR_BSD_ENOMEM;
            }
           /* Interface exists, try to create new connection                */
            lRetVal = (netIf->ifConf)->connEnable(netIf->ifContext);

            if(lRetVal == 0)
            {
                g_ctx.NetIfList[g_ctx.NetIfCount].ifId = netIf->ifID;
                g_ctx.NetIfList[g_ctx.NetIfCount].status = SLNETCONN_STATUS_WAITING_FOR_CONNECTION;
                g_ctx.NetIfCount ++;
                g_ctx.status = SLNETCONN_STATUS_WAITING_FOR_CONNECTION;
                retVal = 0;
            }

        }
    } while (netIf && ifBitmap);
    return retVal;
}

//*****************************************************************************
//
// DisableIF - Loop through the available interface and enable each of them
//
//*****************************************************************************
static int32_t DisableIF()
{
    int32_t retVal = 0;
    SlNetIf_t   *netIf;
    int i;

    /* When ifBitmap is 0, that means automatic selection of all interfaces
       is required, enable all bits in ifBitmap                              */
    for(i = 0; retVal == 0 && i < g_ctx.NetIfCount; i++)
    {
        netIf = SlNetIf_getIfByID(g_ctx.NetIfList[i].ifId);
        assert(netIf);

        /* Interface exists, try to disconnect                          */
        retVal = (netIf->ifConf)->connDisable(netIf->ifContext);
        g_ctx.NetIfList[i].ifId = 0;
        g_ctx.NetIfList[i].status = (SlNetConnStatus_e)SLNETCONN_STATUS_DISCONNECTED;
    };
    SetStatus(SLNETCONN_STATUS_DISCONNECTED, 0);

    return retVal;
}


//*****************************************************************************
//
// CheckInternetConnection - verify internet connection (if needed) by pinging
//                           a known cloud server
//                           Internet Connection can only be set as global
//                           status, it doesn't impact a specific interfaces
//
//*****************************************************************************
static SlNetConnStatus_e CheckInternetConnection()
{
    if(g_ctx.targetStatus == SLNETCONN_STATUS_CONNECTED_INTERNET)
    {
        uint32_t sndCount = 3;
        uint32_t rcvCount = SlNetUtil_ping(&g_ctx.ipAddr, g_ctx.addrLen, sndCount, 1000, 100, 150, g_ctx.ifBitmap, 0);

        if(rcvCount > 0)
        {
            SetStatus(SLNETCONN_STATUS_CONNECTED_INTERNET, 0);
            ReleaseSem(SLNETCONN_STATUS_CONNECTED_INTERNET);
        }
    }
    return g_ctx.status;
}




//*****************************************************************************
//
// SlNetConn_EventHandler - Handle the interface events
//
//*****************************************************************************
static int32_t SlNetIfEventHandler(SlNetIfEventId_e status, uint16_t ifID, void* pData)
{
    int32_t     retVal = SLNETERR_RET_CODE_OK, rc;
    slNetConnMsg_t    sndMSg;

    sndMSg.netIfEvent = status;
    sndMSg.ifId = ifID;
    sndMSg.pData = pData;

    /* Send relevance message to connection manager thread                         */
    rc = mq_send(g_ctx.msgQ, (const char*)&sndMSg, sizeof(sndMSg), 1);
    assert(rc >= 0);

    return retVal;
}


/*****************************************************************************/
/* SlNetConn External API                                                    */
/*****************************************************************************/


//*****************************************************************************
//
// SlNetConn_init - Initialize the SlNetConn module
//
//*****************************************************************************
int32_t SlNetConn_init(int32_t flags)
{
    int32_t    retVal = SLNETERR_RET_CODE_OK;
    struct mq_attr attr;

    VERIFY_NOT_INITIALIZED();

    /* Allocate module mutex (for critical sections protection)  */
    retVal = CS_INIT();
    if (retVal == 0)
    {
        /* Register the  SlNetIf event handler */
        retVal = SlNetIf_registerEventHandler(SlNetIfEventHandler);
    }
    if (retVal == 0)
    {
        /* Create a message queue that used to pass requests between
         * the application thread to connection manager thread.              */
        attr.mq_maxmsg = CONN_MNG_MQ_MAXMSG;
        attr.mq_msgsize = sizeof(slNetConnMsg_t);
        g_ctx.msgQ = mq_open("SlNetConnMsgQ", O_CREAT, 0, &attr);

        if(g_ctx.msgQ < 0)
        {
            retVal = SLNETERR_RET_CODE_MALLOC_ERROR;
        }
    }
    if (retVal == 0)
    {
        /* Init current connection state                                      */
        UsersInit();

        g_ctx.addFamily = SLNETSOCK_AF_INET;
        g_ctx.addrLen = sizeof(SlNetSock_AddrIn_t);
        g_ctx.ipV4Addr.sin_addr.s_addr = 0x08080808; /* "8.8.8.8" */
        SetStatus(SLNETCONN_STATUS_DISCONNECTED, 0);
    }
    else
    {
        SlNetIf_unregisterEventHandler(SlNetIfEventHandler);
        CS_DEINIT();
        if (g_ctx.msgQ < 0)
        {
            mq_close(g_ctx.msgQ);
        }
        g_ctx.status = (SlNetConnStatus_e)0;
    }


    /* SlNetConn layer initialized                                    */
    return retVal;
}

//*****************************************************************************
//
// SlNetConn_setConfiguration - Set connection configurations
//
//*****************************************************************************
int32_t SlNetConn_setConfiguration(SlNetConn_config_t* pConf)
{
    int32_t  retVal = 0;

    VERIFY_INITIALIZED();

    /* Input validation                                                      */
    if (pConf == NULL)
    {
        retVal = SLNETERR_RET_CODE_INVALID_INPUT;
    }
    if(retVal == 0)
    {
        CS_LOCK();

        /* Save current parameters in CB                                     */
        g_ctx.ifBitmap = pConf->ifBitmap;
        if(pConf->serverAddrFamily == SLNETSOCK_AF_INET)
        {
            retVal  = SlNetUtil_inetPton(SLNETSOCK_AF_INET, pConf->pServerAddress, &g_ctx.ipV4Addr.sin_addr) ;
            g_ctx.addrLen = sizeof(SlNetSock_AddrIn_t);
        }
        else
        {
            retVal  = SlNetUtil_inetPton(SLNETSOCK_AF_INET6, pConf->pServerAddress, &g_ctx.ipV6Addr.sin6_addr) ;
            g_ctx.addrLen = sizeof(SlNetSock_AddrIn6_t);
        }
        if(retVal == 1)
        {
            retVal = 0;
            g_ctx.addFamily = pConf->serverAddrFamily;
        }


        CS_UNLOCK();
    }
    return retVal;
}


//*****************************************************************************
//
// SlNetConn_start - Add user connection request, wait for initial connection
//                   (of specific target level) to be establish.
//
//*****************************************************************************
int32_t SlNetConn_start(SlNetConnServiceLevel_e targetServiceLevel, SlNetConn_AppEvent_f fAppEventHdl, uint32_t timeout, uint32_t flags)
{
    int32_t             retVal = 0, rc;
    SlNetConnStatus_e   targetStatus;

    VERIFY_INITIALIZED();
    VERIFY_INPUT_SERVICE_LEVEL(targetServiceLevel);
    targetStatus = ConvertServiceLevelToStatus(targetServiceLevel);

    // Check if reached maximum number of user requests
    if(g_ctx.users.totalNum == SLNETCONN_USERS_NUM)
    {
        return SLNETERR_BSD_ENOMEM;
    }
    CS_LOCK();

    retVal = UsersRegister(targetStatus, fAppEventHdl);
    if(retVal == 0)
    {
        if(g_ctx.status == SLNETCONN_STATUS_CONNECTED_IP)
        {
            /* If IP Connection is already in place -
             * check the internet connection
             */
            CheckInternetConnection();
        }
        if (targetStatus <= g_ctx.status)
        {
            /* If connection already exist return immediately                       */
            retVal =  SLNETERR_RET_CODE_OK;
        }
        else
        {
            /* init and register the caller blocking semaphore  */
            SemEntry_t semNode;
            retVal = RegisterSem(targetStatus, &semNode);

            if (retVal == 0)
            {
                if(g_ctx.status == SLNETCONN_STATUS_DISCONNECTED)
                {
                    // Enable (createConn) the selected interfaces
                    retVal = EnableIF(g_ctx.ifBitmap);
                }
                if(retVal == 0)
                {
                    if(timeout)
                    {
                         /* Wait for connection to be establish */
                         retVal = WaitForStatus(targetStatus, &semNode, timeout);
                    }
                    else
                    {
                        /* Timeout == NULL -> don't block but set as still-in-process */
                        retVal = 1;
                    }
                }
            }
            rc = UnRegisterSem(targetStatus, &semNode);
            assert(rc == 0); // shouldn't happen
        }
    }

    CS_UNLOCK();

    return retVal;
}



//*****************************************************************************
//
// SlNetConn_waitForConnection - Wait for connection (of specific target level)
//                               to be establish.
//
//*****************************************************************************
int32_t SlNetConn_waitForConnection(SlNetConnServiceLevel_e targetServiceLevel, uint32_t timeout)
{
    int32_t             retVal = 0, rc;
    SlNetConnStatus_e   targetStatus; //

    VERIFY_INITIALIZED();
    VERIFY_INPUT_SERVICE_LEVEL(targetServiceLevel);

    targetStatus = ConvertServiceLevelToStatus(targetServiceLevel);

    CS_LOCK();

    /* If connection already exist return immediately                       */
    if (targetStatus <= g_ctx.status)
    {
        retVal = SLNETERR_RET_CODE_OK;
    }
    else
    {
        if ((retVal < 0) || (targetStatus > g_ctx.targetStatus))
        {
            /* There should be an active request with similar or higher service level */
            retVal = SLNETERR_RET_CODE_INVALID_INPUT;
        }
        else
        {
            /* init and register the caller blocking semaphore  */
            SemEntry_t semNode;
            retVal = RegisterSem(targetStatus, &semNode);

             if (retVal == 0)
             {
                 retVal = WaitForStatus(targetStatus, &semNode, timeout);
             }
             rc = UnRegisterSem(targetStatus, &semNode);
             assert(rc == 0); // shouldn't happen

        }
    }
    CS_UNLOCK();

    return retVal;
}



//*****************************************************************************
//
// SlNetConn_stop - Remove user's connection request, disconnect from all
//                   existing connections
//
//*****************************************************************************
int32_t SlNetConn_stop(SlNetConn_AppEvent_f fAppEventHdl)
{
    int32_t  retVal = SLNETERR_RET_CODE_DEV_NOT_STARTED;
    bool bDisabled = false;

    VERIFY_INITIALIZED();

    CS_LOCK();
    if(g_ctx.status > SLNETCONN_STATUS_DISCONNECTED)
    {
        retVal = UsersUnRegister(fAppEventHdl);

        if(retVal == 0 && g_ctx.users.totalNum == 0)
        {
            retVal = DisableIF();
            bDisabled = true;
        }
    }
    CS_UNLOCK();
    if(bDisabled && fAppEventHdl)
        fAppEventHdl(0, SLNETCONN_STATUS_DISCONNECTED, 0);

    return retVal;
}

//*****************************************************************************
//
// SlNetConn_getStatus - Retrieve active status
//
//*****************************************************************************
int32_t SlNetConn_getStatus(bool bCheckInternetConnection, SlNetConnStatus_e *pStatus)
{
    VERIFY_INITIALIZED();
    if(bCheckInternetConnection)
    {
        if(g_ctx.status >= SLNETCONN_STATUS_CONNECTED_IP)
            *pStatus = CheckInternetConnection();
    }
    else
    {
        *pStatus =  g_ctx.status;
    }
    return SLNETERR_RET_CODE_OK;
}



//*****************************************************************************
//
// SlNetConn_process - Connection manager thread, handles all connection requests
//                  (Thread is triggered from the application)
//
//*****************************************************************************
void* SlNetConn_process(void* pvParameters)
{
    slNetConnMsg_t    rcvMsg;
    int32_t     rc;

    while (g_ctx.status < SLNETCONN_STATUS_DISCONNECTED)
    {
        sleep(1);
    }

    while (1)
    {
        int ifId;
        /* Waiting for a message                                                */
        rc = mq_receive(g_ctx.msgQ, (char*)&rcvMsg, sizeof(slNetConnMsg_t), NULL);
        assert(rc == sizeof(slNetConnMsg_t));
        g_fDebugPrint("SlNetConn(TASK):: %s / %s (if=%d)\n\r", strStatus[g_ctx.status], strEvent[rcvMsg.netIfEvent], rcvMsg.ifId );

        ifId = rcvMsg.ifId;
        CS_LOCK();
        /* Update connection status                                               */
        switch(rcvMsg.netIfEvent)
        {
        case SLNETIF_IP_AQUIRED:
            ReleaseSem(SLNETCONN_STATUS_CONNECTED_IP);
            SetStatus(SLNETCONN_STATUS_CONNECTED_IP, ifId);
            CheckInternetConnection();
            break;

        case SLNETIF_MAC_CONNECT:
            ReleaseSem(SLNETCONN_STATUS_CONNECTED_MAC);
            SetStatus(SLNETCONN_STATUS_CONNECTED_MAC, ifId);
            break;

        case SLNETIF_DISCONNECT:
            if(g_ctx.status != SLNETCONN_STATUS_DISCONNECTED)
            {
                SetStatus(SLNETCONN_STATUS_WAITING_FOR_CONNECTION, ifId);
            }
            break;
        default:
            assert(0);
        }
        CS_UNLOCK();

        /*
         * If we established an internet connection after receiving an IP
         * notify of the IP being acquired as well
         */
        if (rcvMsg.netIfEvent == SLNETIF_IP_AQUIRED &&
                g_ctx.status == SLNETCONN_STATUS_CONNECTED_INTERNET)
        {
            rc = UsersNotify(ifId, SLNETCONN_STATUS_CONNECTED_IP,
                    rcvMsg.pData);
        }
        rc = UsersNotify(ifId, g_ctx.status, rcvMsg.pData);
        assert(rc >= 0);
   }
}

#if SLNETCONN_DEBUG_ENABLE
//*****************************************************************************
//
// SlNetConn_RegisterDebugCallback - register an application-level debug
//                                        (printf) method.
//
//*****************************************************************************
void SlNetConn_registerDebugCallback(SlNetConn_Debug_f fDebugPrint)
{
    g_fDebugPrint = fDebugPrint;
}
#endif

