#include <assert.h>
#include "cyobjstore.h"
#include <stdbool.h>
#include "cy_pdl.h"
#include "cy_em_eeprom.h"
#include "cy_result.h"

#ifndef CY_OBJSTORE_THREADSAFE
#define CY_OBJSTORE_THREADSAFE 1
#endif

#if CY_OBJSTORE_THREADSAFE
#include "cyabs_rtos.h"
#endif

/*
 * Defines private to this file
 */
#define CY_EEPROM_STATE_MAGIC_NUMBER            (0xFACEFACE)
#define CY_INVALID_KEY                          (0xFF)

#ifndef CY_OBJSTORE_BUFFER_SIZE
#define CY_OBJSTORE_BUFFER_SIZE                 (128)
#endif

/*
 * Typedefs private to this fiel
 */
typedef struct key_data_pair
{
    uint8_t key_ ;
    uint32_t data_size_ ;
    uint32_t data_offset_ ;
} key_data_pair_t ;

typedef uint32_t SignatureType_t ;

/*
 * Variables private to this file
 */
static bool is_initialized = false ;
static bool is_valid = false ;
static cy_stc_eeprom_context_t context ;
static cy_stc_eeprom_config_t config ;
static uint32_t eeprom_size = CY_EM_EEPROM_SIZE ;
static uint32_t eeprom_addr = CY_EM_EEPROM_BASE ;
static uint8_t buffer[CY_OBJSTORE_BUFFER_SIZE] ;
#if CY_OBJSTORE_THREADSAFE
// Requires recursive mutex
static cy_mutex_t cy_objstore_mutex;

static void cy_objstore_initlock()
{
    if (CY_RSLT_SUCCESS != cy_rtos_init_mutex(&cy_objstore_mutex))
        abort();
}

static void cy_objstore_lock()
{
    if (CY_RSLT_SUCCESS != cy_rtos_get_mutex(&cy_objstore_mutex, CY_RTOS_NEVER_TIMEOUT))
        abort();
}

static void cy_objstore_unlock()
{
    if (CY_RSLT_SUCCESS != cy_rtos_set_mutex(&cy_objstore_mutex))
        abort();
}
#else
static void cy_objstore_initlock() { /* Nothing to do */ }
static void cy_objstore_lock() { /* Nothing to do */ }
static void cy_objstore_unlock() { /* Nothing to do */ }
#endif

static cy_rslt_t bytestore_read(uint32_t addr, uint8_t *data, uint32_t size)
{
    assert(addr < config.eepromSize && addr + size < config.eepromSize);
    return Cy_Em_EEPROM_Read(addr, data, size, &context) ;
}

static cy_rslt_t bytestore_write(uint32_t addr, const uint8_t *data, uint32_t size)
{
    assert(addr < config.eepromSize && addr + size < config.eepromSize);
    return Cy_Em_EEPROM_Write(addr, (void *)data, size, &context) ;
}

static cy_rslt_t chk_is_valid()
{
    cy_rslt_t res ;

    if (is_valid)
        return true ;

    cy_objstore_lock();
    res = bytestore_read(0, buffer, sizeof(SignatureType_t)) ;
    cy_objstore_unlock();
    if (res != CY_RSLT_SUCCESS)
        return res ;

    is_valid = (*((SignatureType_t *)&buffer[0]) == CY_EEPROM_STATE_MAGIC_NUMBER) ;

    return is_valid ? CY_RSLT_SUCCESS : CY_OBJSTORE_STORE_UNFORMATTED ;
}

/*
 * This is designed to move data to lower addresses in the storage.  As
 * such there is no need to worry about overlap.
 */
static cy_rslt_t move_data_left(uint32_t dest, uint32_t src, uint32_t size)
{
    uint32_t tocopy = size ;
    cy_rslt_t res ;

    while (tocopy > 0)
    {
        uint32_t copysize = tocopy ;
        if (copysize > sizeof(buffer))
            copysize = sizeof(buffer) ;

        res = bytestore_read(src, buffer, copysize) ;
        if (res != CY_RSLT_SUCCESS)
            return res ;

        res = bytestore_write(dest, buffer, copysize) ;
        if (res != CY_RSLT_SUCCESS)
            return res ;

        tocopy -= copysize ;
        dest += copysize ;
        src += copysize ;        
    }

    return CY_RSLT_SUCCESS ;
}

static cy_rslt_t store_newobject(Key_t hand, const uint8_t *data, uint32_t size)
{
    Key_t key ;
    uint32_t index , len ;
    cy_rslt_t res ;

    index = sizeof(SignatureType_t) ;

    cy_objstore_lock();

    do {
        res = bytestore_read(index, buffer, sizeof(uint32_t) + sizeof(Key_t)) ;
        if (res != CY_RSLT_SUCCESS)
        {
            cy_objstore_unlock();
            return res ;
        }

        key = buffer[0] ;
        len = (buffer[1] << 0) |
                (buffer[2] << 8) |
                (buffer[3] << 16) |
                (buffer[4] << 24) ;   

        if (key == CY_INVALID_KEY)
        {
            //
            // This spot is empty, add it here if there is room
            //
            if (index + sizeof(Key_t) + sizeof(uint32_t) + size >= config.eepromSize)
            {
                cy_objstore_unlock();
                return CY_OBJSTORE_NO_SPACE ;
            }

            buffer[0] = hand ;
            buffer[1] = (size >> 0) & 0xff ;
            buffer[2] = (size >> 8) & 0xff ;
            buffer[3] = (size >> 16) & 0xff ;
            buffer[4] = (size >> 24) & 0xff ;  

            res = bytestore_write(index, buffer, sizeof(uint32_t) + sizeof(Key_t)) ;
            if (res != CY_RSLT_SUCCESS)
            {
                cy_objstore_unlock();
                return res ;
            }

            index += sizeof(uint32_t) + sizeof(Key_t) ;
            res = bytestore_write(index, data, size);
            if (res != CY_RSLT_SUCCESS)
            {
                cy_objstore_unlock();
                return res ;
            }

            index += size ;
            if (index + sizeof(uint32_t) + sizeof(Key_t) < config.eepromSize)
            {
                buffer[0] = CY_INVALID_KEY ;
                buffer[1] = 0 ;
                buffer[2] = 0 ;
                buffer[3] = 0 ;
                buffer[4] = 0 ;
                res = bytestore_write(index, buffer, sizeof(Key_t) + sizeof(uint32_t)) ;
                if (res != CY_RSLT_SUCCESS)
                {
                    cy_objstore_unlock();
                    return res ;                
                }
            }

            cy_objstore_unlock();
            return CY_RSLT_SUCCESS ;
        }

        index = index + len + sizeof(Key_t) + sizeof(uint32_t) ;

    } while(index + sizeof(uint32_t) + sizeof(Key_t) < config.eepromSize) ;

    cy_objstore_unlock();
    return CY_OBJSTORE_NO_SPACE ;
}

/*
 * Public Code
 */
cy_rslt_t cy_objstore_format()
{
    cy_rslt_t res ;

    if (!is_initialized)
        return CY_OBJSTORE_NOT_INITIALIZED ;

    *((uint32_t *)&buffer[0]) = CY_EEPROM_STATE_MAGIC_NUMBER ;
    buffer[sizeof(uint32_t)] = CY_INVALID_KEY ;

    cy_objstore_lock();
    res = bytestore_write(0, buffer, sizeof(SignatureType_t) + sizeof(Key_t)) ;
    cy_objstore_unlock();
    if (res != CY_RSLT_SUCCESS)
        return res ;

    is_valid = true ;
    return CY_RSLT_SUCCESS ;
}

cy_rslt_t cy_objstore_erase()
{
    cy_en_em_eeprom_status_t st ;   

    if (!is_initialized)
        return CY_OBJSTORE_NOT_INITIALIZED ;    

    cy_objstore_lock();
    st = Cy_Em_EEPROM_Erase(&context);
    cy_objstore_unlock();
    if (st != CY_RSLT_SUCCESS)
        return CY_OBJSTORE_EEPROM_FAILED ;

    is_valid = false ;

    return CY_RSLT_SUCCESS ;
}

cy_rslt_t cy_objstore_is_initialized()
{
    return is_initialized ? CY_RSLT_SUCCESS : CY_OBJSTORE_NOT_INITIALIZED ;
}

cy_rslt_t cy_objstore_initialize(bool redundant, int wearlevel)
{
    cy_en_em_eeprom_status_t st ;
    cy_rslt_t res ;

    if (is_initialized)
        return CY_RSLT_SUCCESS ;

    cy_objstore_initlock();

    config.redundantCopy = redundant ;
    config.blockingWrite = true ;
    config.wearLevelingFactor = wearlevel ;
    config.eepromSize = eeprom_size / (1 + config.redundantCopy) / 2 / config.wearLevelingFactor ;
    config.userFlashStartAddr = eeprom_addr ;    

    st = Cy_Em_EEPROM_Init(&config, &context) ;
    if (st != CY_EM_EEPROM_SUCCESS)
        return CY_OBJSTORE_EEPROM_FAILED ;        

    is_initialized = true ;

    res = chk_is_valid() ;
    if (res == CY_OBJSTORE_STORE_UNFORMATTED)
        res = cy_objstore_format() ;

    return res ;
}

cy_rslt_t cy_objstore_find_object(Key_t hand, uint32_t *data, uint32_t *size)
{
    cy_rslt_t res ;
    uint32_t index = sizeof(SignatureType_t) ;
    Key_t key ;
    uint32_t len ;

    if (!is_initialized)
        return CY_OBJSTORE_NOT_INITIALIZED ;    

    if (!is_valid)
        return CY_OBJSTORE_STORE_UNFORMATTED ;

    if (hand == CY_INVALID_KEY)
        return CY_OBJSTORE_INVALID_KEY;

    cy_objstore_lock();

    do {
        res = bytestore_read(index, buffer, sizeof(uint32_t) + sizeof(Key_t)) ;
        if (res != CY_RSLT_SUCCESS)
        {
            cy_objstore_unlock();
            return res ;
        }

        key = buffer[0] ;

        /* Might not be propertly aligned, so unpack by hand */
        len = (buffer[1] << 0) |
                (buffer[2] << 8) |
                (buffer[3] << 16) |
                (buffer[4] << 24) ;        
        if (key == hand)
        {
            if (data != NULL)
                *data = index + sizeof(uint32_t) + sizeof(Key_t) ;

            if (size != NULL)
                *size = len ;
            cy_objstore_unlock();
            return CY_RSLT_SUCCESS ;
        }

        index = index + len + sizeof(Key_t) + sizeof(uint32_t) ;

    } while (key != CY_INVALID_KEY ) ;

    cy_objstore_unlock();
    return CY_OBJSTORE_NO_SUCH_OBJECT ;
}

cy_rslt_t cy_objstore_store_object(Key_t hand, const uint8_t *data, uint32_t size)
{
    cy_rslt_t res ;
    uint32_t existing_index, existing_size ;

    if (!is_initialized)
        return CY_OBJSTORE_NOT_INITIALIZED ;    

    if (!is_valid)
        return CY_OBJSTORE_STORE_UNFORMATTED ;

    cy_objstore_lock();
    res = cy_objstore_find_object(hand, &existing_index, &existing_size) ;
    if (res == CY_RSLT_SUCCESS)
    {
        if (existing_size == size)
        {
            /*
             * Just store the data
             */
            res = bytestore_write(existing_index, data, existing_size) ;
        }
        else
        {
            /*
             * We delete then write, so this is not so robust.  Probably OK for a
             * system that does not store much.
             */
            res = cy_objstore_delete_object(hand) ;
            if (res != CY_RSLT_SUCCESS)
            {
                cy_objstore_unlock();
                return res ;
            }

            /*
             * One level of recursion only
             */
            res = store_newobject(hand, data, size) ;
        }
    }
    else if (res == CY_OBJSTORE_NO_SUCH_OBJECT)
    {
        res = store_newobject(hand, data, size) ;
    }
    cy_objstore_unlock();
    return res ;
}

cy_rslt_t cy_objstore_read_object(Key_t key, uint8_t *data, uint32_t objsize)
{
    uint32_t index, size , toread  ;
    cy_rslt_t res ;

    if (!is_initialized)
        return CY_OBJSTORE_NOT_INITIALIZED ;    

    if (!is_valid)
        return CY_OBJSTORE_STORE_UNFORMATTED ;    

    cy_objstore_lock();
    res = cy_objstore_find_object(key, &index, &size) ;
    if (res != CY_RSLT_SUCCESS)
    {
        cy_objstore_unlock();
        return res ;
    }

    toread = size ;
    if (objsize < toread)
        toread = objsize ;

    res = bytestore_read(index, data, toread) ;
    cy_objstore_unlock();
    if (res != CY_RSLT_SUCCESS)
        return res ;

    return objsize < size ? CY_OBJSTORE_PARTIAL_READ_ONLY : CY_RSLT_SUCCESS ;
}

cy_rslt_t cy_objstore_delete_object(Key_t hand)
{
    cy_rslt_t res ;
    uint32_t size, deleteindex;
    Key_t key = hand;
    uint32_t len ;

    if (!is_initialized)
        return CY_OBJSTORE_NOT_INITIALIZED ;    

    if (!is_valid)
        return CY_OBJSTORE_STORE_UNFORMATTED ;    

    cy_objstore_lock();
    res = cy_objstore_find_object(hand, &deleteindex, &size) ;
    if (res != CY_RSLT_SUCCESS)
    {
        cy_objstore_unlock();
        return res ;
    }

    // First entry after deleted entry
    uint32_t nextindex = deleteindex + size;
    uint32_t copysize = 0;
    uint32_t currentindex = nextindex;
    while (key != CY_INVALID_KEY)
    {
        res = bytestore_read(currentindex, buffer, sizeof(Key_t) + sizeof(uint32_t));
        if (res != CY_RSLT_SUCCESS)
        {
            cy_objstore_unlock();
            return res ;
        }
        key = buffer[0];
        /* Might not be propertly aligned, so unpack by hand */
        len = key == CY_INVALID_KEY ? 0 :
            ((buffer[1] << 0) |
             (buffer[2] << 8) |
             (buffer[3] << 16) |
             (buffer[4] << 24)) ;
        copysize += sizeof(Key_t) + sizeof(uint32_t) + len;
        currentindex += sizeof(Key_t) + sizeof(uint32_t) + len;
    }

    res = move_data_left(deleteindex - sizeof(Key_t) - sizeof(uint32_t), nextindex, copysize);

    cy_objstore_unlock();
    return res;
}
