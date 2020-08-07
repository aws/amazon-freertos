/****************************************************************************
**
** Name:         wiced_data_types.h
**
** Description:  wiced data types header file for VS2010 projects
**
** Copyright (c) Cypress Semiconductor
**
******************************************************************************/

#ifndef WICED_DATA_TYPES_H
#define WICED_DATA_TYPES_H


#include <stdint.h>
#include <string.h>

/**
 *  @ingroup     gentypes
 *
 *  @{
 */

#define WICED_FALSE 0   /**< Wiced false */
#define WICED_TRUE  1   /**< Wiced true */

#ifndef FALSE
#define FALSE 0         /**< false */
#endif

#ifndef TRUE
#define TRUE  1         /**< true */
#endif

#ifdef __ARM__
#define WICED_BT_STRUCT_PACKED  __packed() struct               /**< packed structure */
#define WICED_BT_UNION_PACKED  __packed() union                 /**< packed union */
#elif defined(TOOLCHAIN_gnu) || defined(COMPILER_GNU)
#define WICED_BT_STRUCT_PACKED struct __attribute__((packed))   /**< packed structure */
#define WICED_BT_UNION_PACKED  union  __attribute__((packed))   /**< packed union */
#else
#define WICED_BT_STRUCT_PACKED  struct                          /**< packed structure */
#define WICED_BT_UNION_PACKED   union                           /**< packed union */
#endif

/** Surpress Warnings */
#define WICED_SUPPRESS_WARNINGS(m) if((m)){;}

/* Suppress unused variable warning */
#ifndef UNUSED_VARIABLE
/** Unused Variable */
#define UNUSED_VARIABLE(x) /*@-noeffect@*/ ( (void)(x) ) /*@+noeffect@*/
#endif

/* To prevent complier to optimize with LDM and STM instructions */
#define WICED_MEMCPY(a, b, c)       memcpy((void*)(a), (const void*)(b), c)     /**< Wiced Memory copy*/
#define WICED_MEMSET(a, b, c)       memset((void*)(a), b, c)                    /**< Wiced Memory set */
#define WICED_MEMMOVE(a, b, c)      memmove((void*)(a), (const void*)(b), c)    /**< Wiced Memory move*/
#define WICED_MEMCMP(a, b, c)       memcmp((void*)(a), (const void*)(b), c)     /**< Wiced Memory compare*/

/** Wiced Boolean */
typedef unsigned int   wiced_bool_t;

/** Function prototypes to lock and unlock (typically using a mutex). The context
** pointer may be NULL, depending on implementation.
*/
typedef struct {
    void* p_lock_context;                           /**< lock context pointer */
    void (*pf_lock_func)(void * p_lock_context);    /**< Lock function pointer */
    void (*pf_unlock_func)(void * p_lock_context);  /**< Unlock function pointer */
}wiced_bt_lock_t;

 /**
  * Exception callback : 
  *
  * Called by stack in case of unhandled exceptions and critical errors.
  *
  * @param[in] code    : Exception code
  * @param[in] msg     : Exception string
  * @param[in] p_tr    : Pointer to the data (based on the exception)
  *
  * @return void
  */
typedef void (*pf_wiced_exception)(uint16_t code, char* msg, void* ptr);
/**@} gentypes */
#endif
