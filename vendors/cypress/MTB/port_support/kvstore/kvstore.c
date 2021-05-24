#include "cyhal.h"
#include "mtb_kvstore.h"

mtb_kvstore_t kvstore_obj;

static cyhal_flash_block_info_t block_info;


/* block device interface functions */

//--------------------------------------------------------------------------------------------------
// bd_read_size
//--------------------------------------------------------------------------------------------------
uint32_t bd_read_size(void* context, uint32_t addr)
{
    return 1;
}


//--------------------------------------------------------------------------------------------------
// bd_program_size
//--------------------------------------------------------------------------------------------------
uint32_t bd_program_size(void* context, uint32_t addr)
{
    return block_info.page_size;
}


//--------------------------------------------------------------------------------------------------
// bd_erase_size
//--------------------------------------------------------------------------------------------------
uint32_t bd_erase_size(void* context, uint32_t addr)
{
    return block_info.sector_size;
}


//--------------------------------------------------------------------------------------------------
// bd_read
//--------------------------------------------------------------------------------------------------
cy_rslt_t bd_read(void* context, uint32_t addr, uint32_t length, uint8_t* buf)
{
    memcpy(buf, (const uint8_t*)(addr), length);
    return CY_RSLT_SUCCESS;
}


//--------------------------------------------------------------------------------------------------
// bd_program
//--------------------------------------------------------------------------------------------------
cy_rslt_t bd_program(void* context, uint32_t addr, uint32_t length, const uint8_t* buf)
{
    uint32_t prog_size = bd_program_size(context, addr);
    CY_ASSERT(0 == (length % prog_size));
    cy_rslt_t result = CY_RSLT_SUCCESS;
    for (uint32_t loc = addr; (result == CY_RSLT_SUCCESS) && (loc < addr + length);
         loc += prog_size, buf += prog_size)
    {
        result = cyhal_flash_program((cyhal_flash_t*)context, loc, (const uint32_t*)buf);
    }
    return result;
}


//--------------------------------------------------------------------------------------------------
// bd_erase
//--------------------------------------------------------------------------------------------------
cy_rslt_t bd_erase(void* context, uint32_t addr, uint32_t length)
{
    uint32_t erase_size = bd_erase_size(context, addr);
    CY_ASSERT(0 == (length % erase_size));
    cy_rslt_t result = CY_RSLT_SUCCESS;
    for (uint32_t loc = addr; (result == CY_RSLT_SUCCESS) && (loc < addr + length);
         loc += erase_size)
    {
        result = cyhal_flash_erase((cyhal_flash_t*)context, loc);
    }
    return result;
}


//--------------------------------------------------------------------------------------------------
// kvstore_init
//--------------------------------------------------------------------------------------------------
cy_rslt_t kvstore_init(void)
{
    cy_rslt_t          rslt;
    cyhal_flash_info_t flash_info;

    static cyhal_flash_t    flash_obj;
    static mtb_kvstore_bd_t block_device =
    {
        .read         = bd_read,
        .program      = bd_program,
        .erase        = bd_erase,
        .read_size    = bd_read_size,
        .program_size = bd_program_size,
        .erase_size   = bd_erase_size,
        .context      = &flash_obj
    };

    /* Initialize the block device */
    rslt = cyhal_flash_init(&flash_obj);
    if (rslt != CY_RSLT_SUCCESS)
    {
        return rslt;
    }

    cyhal_flash_get_info(&flash_obj, &flash_info);
    block_info = flash_info.blocks[flash_info.block_count - 1];

    /* Initialize the kv-store library */
    rslt = mtb_kvstore_init(&kvstore_obj, block_info.start_address, block_info.size, &block_device);
    return rslt;
}
