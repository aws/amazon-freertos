/*
 * Sample code used for reference only
 */

/******************************************************
*                      Macros
******************************************************/
#define BLOCK_BUFFER_SIZE                    (1024)

/******************************************************
*                    Constants
******************************************************/

#define NVRAM_SIZE             sizeof(wifi_nvram_image)
#define NVRAM_IMAGE_VARIABLE   wifi_nvram_image

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Static Function Declarations
******************************************************/
uint32_t host_platform_resource_size(whd_driver_t whd_drv, whd_resource_type_t resource, uint32_t *size_out);
uint32_t host_get_resource_block(whd_driver_t whd_drv, whd_resource_type_t type,
                                 uint32_t blockno, const uint8_t **data, uint32_t *size_out);
uint32_t host_get_resource_no_of_blocks(whd_driver_t whd_drv, whd_resource_type_t type, uint32_t *block_count);
uint32_t host_get_resource_block_size(whd_driver_t whd_drv, whd_resource_type_t type, uint32_t *size_out);
resource_result_t resource_read(const resource_hnd_t *resource, uint32_t offset, uint32_t maxsize, uint32_t *size,
                                void *buffer);

/******************************************************
*               Variable Definitions
******************************************************/

extern const resource_hnd_t wifi_firmware_image;
extern const resource_hnd_t wifi_firmware_clm_blob;
unsigned char r_buffer[BLOCK_BUFFER_SIZE];

/******************************************************
*               Function Definitions
******************************************************/

resource_result_t resource_read(const resource_hnd_t *resource, uint32_t offset, uint32_t maxsize, uint32_t *size,
                                void *buffer)
{
    if (offset > resource->size)
    {
        return RESOURCE_OFFSET_TOO_BIG;
    }

    *size = MIN(maxsize, resource->size - offset);

    if (resource->location == RESOURCE_IN_MEMORY)
    {
        memcpy(buffer, &resource->val.mem.data[offset], *size);
    }
    return RESOURCE_SUCCESS;
}

uint32_t host_platform_resource_size(whd_driver_t whd_drv, whd_resource_type_t resource, uint32_t *size_out)
{
    if (resource == WHD_RESOURCE_WLAN_FIRMWARE)
    {

        *size_out = (uint32_t)resource_get_size(&wifi_firmware_image);
    }
    else if (resource == WHD_RESOURCE_WLAN_NVRAM)
    {
        *size_out = NVRAM_SIZE;
    }
    else
    {
        *size_out = (uint32_t)resource_get_size(&wifi_firmware_clm_blob);
    }
    return WHD_SUCCESS;
}

uint32_t host_get_resource_block(whd_driver_t whd_drv, whd_resource_type_t type,
                                 uint32_t blockno, const uint8_t **data, uint32_t *size_out)
{
    uint32_t resource_size;
    uint32_t block_size;
    uint32_t block_count;
    uint32_t read_pos;
    uint32_t result;

    if (blockno >= block_count)
    {
        return WHD_BADARG;
    }

    host_platform_resource_size(whd_drv, type, &resource_size);
    host_get_resource_block_size(whd_drv, type, &block_size);
    host_get_resource_no_of_blocks(whd_drv, type, &block_count);
    memset(r_buffer, 0, block_size);
    read_pos = blockno * block_size;

    if (type == WHD_RESOURCE_WLAN_FIRMWARE)
    {
        result = resource_read( (const resource_hnd_t *)&wifi_firmware_image, read_pos, block_size, size_out,
                                r_buffer );
        if (result != WHD_SUCCESS)
        {
            return result;
        }
        *data = (uint8_t *)&r_buffer;

    }
    else if (type == WHD_RESOURCE_WLAN_NVRAM)
    {
        if (NVRAM_SIZE - read_pos > block_size)
        {
            *size_out = block_size;
        }
        else
        {
            *size_out = NVRAM_SIZE - read_pos;
        }
        *data = ( (uint8_t *)NVRAM_IMAGE_VARIABLE ) + read_pos;
    }
    else
    {
        result = resource_read( (const resource_hnd_t *)&wifi_firmware_clm_blob, read_pos, block_size,
                                size_out,
                                r_buffer );
        if (result != WHD_SUCCESS)
        {
            return result;
        }
        *data = (uint8_t *)&r_buffer;

    }

    return WHD_SUCCESS;
}

uint32_t host_get_resource_block_size(whd_driver_t whd_drv, whd_resource_type_t type, uint32_t *size_out)
{
    *size_out = BLOCK_BUFFER_SIZE;
    return WHD_SUCCESS;
}

uint32_t host_get_resource_no_of_blocks(whd_driver_t whd_drv, whd_resource_type_t type, uint32_t *block_count)
{
    uint32_t resource_size;
    uint32_t block_size;

    host_platform_resource_size(whd_drv, type, &resource_size);
    host_get_resource_block_size(whd_drv, type, &block_size);
    *block_count = resource_size / block_size;
    if (resource_size % block_size)
        *block_count += 1;

    return WHD_SUCCESS;
}

whd_resource_source_t resource_ops =
{
    .whd_resource_size = host_platform_resource_size,
    .whd_get_resource_block_size = host_get_resource_block_size,
    .whd_get_resource_no_of_blocks = host_get_resource_no_of_blocks,
    .whd_get_resource_block = host_get_resource_block
};

