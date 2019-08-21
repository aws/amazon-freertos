/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/*
 *
 * Almost code are derived from uboot in MTK APSoC(MT762x) SDK.
 *
 */
#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mem_util.h"
#include "sfc.h"
#include "spi_flash.h"

#include "timer.h"
#include "_mtk_hal_debug.h"
#include "hal_nvic_internal.h"
#include "flash_sfc.h"
#include "hal_log.h"
#include "memory_attribute.h"
#include "bsp_flash_config.h"

#define DIRECT_READ_SPI_WINBOND		0x0B0B7000
#define DIRECT_READ_SPIQ_WINBOND	0xEBFF5000
#define DIRECT_READ_QPI_WINBOND		0x0BFF1000

#define DIRECT_READ_SPIQ_MICRON	    0xEBFF9000

/*
 *   this is SFC controller H/W GPRAM size define
 */
unsigned int gSFC_max_data_size = MAX_SFC_GPRAM_SIZE - 32; /* 160 actually in MT7687 HW design */

/* Winbond specific flash command sequences */
const unsigned char cmdlist_switchqpi_winbond[] = {
    SPI, 1, 0x06, SPI, 3, 0x01, 0x00, 0x02, SPI, 1, 0x38, SF_UNDEF
};
const unsigned char cmdlist_switchspi_winbond[] = {
    QPI, 1, 0xFF, SF_UNDEF
};
const unsigned char cmdlist_switchspiq_winbond[] = {
    SPI, 1, 0x06, SPI, 3, 0x01, 0x00, 0x02, SF_UNDEF
};
ATTR_RODATA_IN_TCM unsigned char cmdlist_suspend_winbond[] = {
    SPI, 1, 0x75, SF_UNDEF
};
ATTR_RODATA_IN_TCM unsigned char cmdlist_resume_winbond[] = {
    SPI, 1, 0x7A, SF_UNDEF
};

//don't support QPI mode on MX25L3233F
const unsigned char cmdlist_switchspiq_mxic[] = {
    SPI, 1, 0x06, SPI, 2, 0x01, 0x40, SF_UNDEF
};

const unsigned char cmdlist_switchqpi_mxic[] = {
    SPI, 1, 0x06, SPI, 2, 0x01, 0x40, SF_UNDEF
};

const unsigned char cmdlist_switchspi_mxic[] = {
    SPI, 1, 0x06, SPI, 2, 0x01, 0x00, SF_UNDEF
};

//don't support QPI mode on MX25L3223F 
const unsigned char cmdlist_switchspiq_gd[] = {
    SPI, 1, 0x06, SPI, 3, 0x01, 0x0, 0x2, SF_UNDEF
};

const unsigned char cmdlist_switchqpi_gd[] = {
    SPI, 1, 0x06, SPI, 3, 0x01, 0x0, 0x2, SF_UNDEF
};

const unsigned char cmdlist_switchspi_gd[] = {
    SPI, 1, 0x06, SPI, 3, 0x01, 0x00, 0x00, SF_UNDEF
};

const unsigned char cmdlist_switchspi_gd25q32c[] = {
    SPI, 1, 0x06, SPI, 2, 0x31, 0x0, SF_UNDEF
};
const unsigned char cmdlist_switchspiq_gd25q32c[] = {
    SPI, 1, 0x06, SPI, 2, 0x31, 0x2, SF_UNDEF
};

const unsigned char cmdlist_switchqpi_gd25q32c[] = {
    SPI, 1, 0x06, SPI, 2, 0x31, 0x2, SF_UNDEF
};

const unsigned char cmdlist_switchspi_micron[] = {
    SPI, 1, 0x06, SPI, 2, 0xB1, 0x1, SF_UNDEF
};
const unsigned char cmdlist_switchspiq_micron[] = {
    SPI, 1, 0x06, SPI, 2, 0xB1, 0x0, SF_UNDEF
};

const unsigned char cmdlist_switchqpi_micron[] = {
    SPI, 1, 0x06, SPI, 2, 0xB1, 0x0, SF_UNDEF
};

/* Generic Flash reset sequence --> 0x66 , 0x99 */
const unsigned char cmdlist_flash_reset_spimode[] = {
    SPI, 1, 0x66, SPI, 1, 0x99, SF_UNDEF
};
const unsigned char cmdlist_flash_reset_qpimode[] = {
    QPI, 1, 0x66, QPI, 1, 0x99, SF_UNDEF
};

const struct chip_info *spi_chip_info = NULL;

const static struct chip_info chips_data[] = {
    /* REVISIT: fill in JEDEC ids, for parts that have them */
    { "DEF_FLASH",  0xff, 0xFFFFFFFF, 4096,  256, 0x00, 0x00, 0x00},
    { "W25Q32JV",   0xef, 0x40160000, 16384, 256, 0x32, 0x6b, 0xeb},
    { "W25Q16DV",   0xef, 0x40150000, 8192,  256, 0x32, 0x6b, 0xeb},
    { "W25Q80DV",   0xef, 0x40140000, 4096,  256, 0x32, 0x6b, 0xeb},
    { "MX25R3235F", 0xC2, 0x28160000, 16384, 256, 0x32, 0x6B, 0xEB},
    { "MX25L3233F", 0XC2, 0X20160000, 16384, 256, 0X38, 0X6B, 0XEB},
    { "MX25L1635F", 0XC2, 0X20150000, 8192,  256, 0X38, 0X6B, 0XEB},
    { "MX25V1635F", 0XC2, 0X20150000, 8192,  256, 0X38, 0X6B, 0XEB},
    { "MX25V8035F", 0XC2, 0X20140000, 4096,  256, 0X38, 0X6B, 0XEB},
    { "KH25V1635F", 0XC2, 0X23150000, 8192,  256, 0X38, 0X6B, 0XEB},
    { "GD25Q32CSIG",0XC8, 0X40160000, 16384, 256, 0X32, 0X6B, 0XEB},
    { "GD25Q16CSIG",0XC8, 0X40150000, 8192,  256, 0X32, 0X6B, 0XEB},
    { "GD25VE32C",  0XC8, 0X42160000, 8192,  256, 0X32, 0X6B, 0XEB},
    { "N25Q128A",   0X20, 0XBA180000, 65536, 256, 0X32, 0X6B, 0XEB},
};

int gd_write_sr2_1 = 0;
SF_TYPT support_flash_id = SF_TYPE_NONE;
extern uint32_t gucFlashSFCMode;
extern SF_DRV_STATE sf_drvier_status;


/*
 *	Reset Flash to init state
 */
ATTR_TEXT_IN_TCM static void flash_reset(void)
{
    sfc_Command_List(cmdlist_flash_reset_qpimode);
    sfc_delay_ms(1);


    sfc_Command_List(cmdlist_flash_reset_spimode);
    sfc_delay_ms(1);

    return;
}


/*
 *      Suspend Winbond flash programming/erasing
 */
ATTR_TEXT_IN_TCM void flash_suspend_Winbond(void)
{
    sfc_Command_List((const unsigned char *)cmdlist_suspend_winbond);
    return;
}

/*
 *      Resume Winbond flash programming/erasing
 */
ATTR_TEXT_IN_TCM void flash_resume_Winbond(void)
{
    sfc_Command_List((const unsigned char *)cmdlist_resume_winbond);
    return;
}

#if 0

/*
 *	Reset Spansion flash QPI mode back to SPI mode
 */
static void flash_reset_SST(void)	/* spansion */
{
    sfc_Command_List(sf_cmd_sw_reset_SST);
    sfc_delay_ms(1);
    return;
}

#endif

ATTR_TEXT_IN_TCM static void flash_Switch_Device_Mode(uint32_t mode)
{
    const unsigned char *ptr;

    /* TODO: winbond only currently !! */
    switch (mode) {
        case SPI:
            ptr = cmdlist_switchspi_winbond;
            if (support_flash_id == SF_TYPE_MXIC) {
                ptr = cmdlist_switchspi_mxic;
            } else if(support_flash_id == SF_TYPE_GD) {
                ptr = cmdlist_switchspi_gd;
                if ( gd_write_sr2_1 == 0x31) {
                    ptr = cmdlist_switchspi_gd25q32c;
                }
            } else if (support_flash_id == SF_TYPE_MICRON){
                ptr = cmdlist_switchspi_micron;
            } else if (support_flash_id == SF_TYPE_CUSTOMER){
                ptr = customer_switch_spi_mode_command();
            }            
            break;

        case SPIQ:
            ptr = cmdlist_switchspiq_winbond;
            if (support_flash_id == SF_TYPE_MXIC) {
                ptr = cmdlist_switchspiq_mxic;
            } else if(support_flash_id == SF_TYPE_GD) {
                ptr = cmdlist_switchspiq_gd;
                if ( gd_write_sr2_1 == 0x31) {
                    ptr = cmdlist_switchspiq_gd25q32c;
                }
            } else if (support_flash_id == SF_TYPE_MICRON){
                    ptr = cmdlist_switchspiq_micron;
            } else if (support_flash_id == SF_TYPE_CUSTOMER){
                ptr = customer_switch_spiq_mode_command();
            }
            break;
        case QPI:
            ptr = cmdlist_switchqpi_winbond;
            if (support_flash_id == SF_TYPE_MXIC) {
                ptr = cmdlist_switchspiq_mxic;
            } else if(support_flash_id == SF_TYPE_GD) {
                ptr = cmdlist_switchspiq_gd;
                if ( gd_write_sr2_1 == 0x31) {
                    ptr = cmdlist_switchqpi_gd25q32c;
                }
            } else if (support_flash_id == SF_TYPE_MICRON){
                ptr = cmdlist_switchspiq_micron;
            } else if (support_flash_id == SF_TYPE_CUSTOMER){
                ptr = customer_switch_spiq_mode_command();
            }
            break;
        default:
            ASSERT(0);
    }

    //SF_DVT_Print_Command_List(ptr);
    sfc_Command_List(ptr);
}


ATTR_TEXT_IN_TCM int flash_check_device(void)
{
    int i;
    unsigned long jedec;
    unsigned char buf[5];
    const struct chip_info *info;

    flash_read_jedec_id(buf, 3);
    jedec = (unsigned long)(((unsigned long)buf[1] << 24) | ((unsigned long)buf[2] << 16));
    //log_hal_info("spi device id: %x %x %x (%x)\n\r", buf[0], buf[1], buf[2], (unsigned int)jedec);
    gucFlashSFCMode = sfc_GetDirectMode();

    if (support_flash_id == SF_TYPE_CUSTOMER) {
    	   info = spi_chip_info;
        if (info->id == buf[0] && info->jedec_id == jedec) {
            if (info->id == SF_TYPE_MXIC) {
                support_flash_id = SF_TYPE_MXIC;
            } else if (info->id == SF_TYPE_WINBOND) {
                support_flash_id = SF_TYPE_WINBOND;
            } else if (info->id == SF_TYPE_GD) {
                gd_write_sr2_1 = 0x1;
                if(0x40160000 == info->jedec_id || 0x42160000 == info->jedec_id) {
                    //GD25Q32CSIG or GD25VE32C
                    gd_write_sr2_1 = 0x31;
                }
                support_flash_id = SF_TYPE_GD;
            } else if (info->id == SF_TYPE_MICRON) {
                support_flash_id = SF_TYPE_MICRON;
            }
        } else {
            log_hal_info(" Check flash device Failed !! \n\r");
            log_hal_info("spi device id: %x %x %x (%x)\n\r", buf[0], buf[1], buf[2], (unsigned int)jedec);
        }
    }
    
    for (i = 1; i < sizeof(chips_data) / sizeof(chips_data[0]); i++) {
        info = &chips_data[i];
        if (info->id == buf[0] && info->jedec_id == jedec) {
            spi_chip_info = info;
            if (info->id == SF_TYPE_MXIC) {
                support_flash_id = SF_TYPE_MXIC;
            } else if (info->id == SF_TYPE_WINBOND) {
                support_flash_id = SF_TYPE_WINBOND;
            } else if (info->id == SF_TYPE_GD) {
                gd_write_sr2_1 = 0x1;
                if(0x40160000 == info->jedec_id || 0x42160000 == info->jedec_id) {
                    //GD25Q32CSIG or GD25VE32C
                    gd_write_sr2_1 = 0x31;
                }
                support_flash_id = SF_TYPE_GD;
            } else if (info->id == SF_TYPE_MICRON) {
                support_flash_id = SF_TYPE_MICRON;
            }

            //log_hal_info("flash_check_device: %x \n\r\n\r", support_flash_id);
            sf_drvier_status = SF_DRV_READY;
            return 1;
        }
    }

    spi_chip_info = &chips_data[1];
    sf_drvier_status = SF_DRV_READY;
    return 0;
}

ATTR_TEXT_IN_TCM void flash_switch_mode(unsigned long mode)
{
    if (customer_flash_register()!= NULL) {
    	   //configured external flash
        support_flash_id = SF_TYPE_CUSTOMER;
    }
    
    flash_check_device();
    flash_Switch_Device_Mode(mode);

    if (mode == SPI) {
        sfc_direct_read_setup(DIRECT_READ_SPI_WINBOND, mode);
    } else if (mode == SPIQ) {
        if(support_flash_id == SF_TYPE_MICRON) {
        	   sfc_direct_read_setup(DIRECT_READ_SPIQ_MICRON, mode);
        } else {
            sfc_direct_read_setup(DIRECT_READ_SPIQ_WINBOND, mode);
        }
    } else if (mode == QPI) {
        sfc_direct_read_setup(DIRECT_READ_QPI_WINBOND, mode);
    } else {
        ASSERT(0);
    }
    gucFlashSFCMode = mode;
}

ATTR_TEXT_IN_TCM void flash_config()
{
    // copy from flash_init() without flash_reset
    int i;
    unsigned long jedec;
    unsigned char buf[5];
    const struct chip_info *info;

#if 0
    /*
     * Serial flash controller init.
     * restore it to default setting
     */
    sfc_Init(SFC_SECURITY_DISABLE);

    /* 1, SFC and flash had been configured in BROM and bootloader;
       2, can't do sfc_init as it reset flash and sfc to default;
       3, flash_config only check used flash is invalid or not; */
#endif

    gSFC_max_data_size = sfc_getMaxGPRamSize();

    //flash_reset();

    spi_chip_info = NULL;

    flash_read_jedec_id(buf, 3);
    jedec = (unsigned long)(((unsigned long)buf[1] << 24) | ((unsigned long)buf[2] << 16));

    log_hal_info("spi device id: %x %x %x (%x)\n", buf[0], buf[1], buf[2], (unsigned int)jedec);

    if (support_flash_id == SF_TYPE_CUSTOMER) {
    	   info = spi_chip_info;
        if (info->id == buf[0] && info->jedec_id == jedec) {
            //device is ok
            if (info->id == SF_TYPE_MXIC) {
                support_flash_id = SF_TYPE_MXIC;
            } else if (info->id == SF_TYPE_WINBOND) {
                support_flash_id = SF_TYPE_WINBOND;
            } else if (info->id == SF_TYPE_GD) {
                gd_write_sr2_1 = 0x1;
                if(0x40160000 == info->jedec_id || 0x42160000 == info->jedec_id) {
                    //GD25Q32CSIG or GD25VE32C
                    gd_write_sr2_1 = 0x31;
                }
                support_flash_id = SF_TYPE_GD;
            } else if (info->id == SF_TYPE_MICRON) {
                support_flash_id = SF_TYPE_MICRON;
            }
           
        } else {
            log_hal_info(" Check flash device Failed !! \n\r");
            log_hal_info("spi device id: %x %x %x (%x)\n\r", buf[0], buf[1], buf[2], (unsigned int)jedec);
        }
    }
    
    for (i = 1; i < sizeof(chips_data) / sizeof(chips_data[0]); i++) {
        info = &chips_data[i];
        if (info->id == buf[0] && info->jedec_id == jedec) {
            log_hal_info("find flash: %s\n", info->name);
            spi_chip_info = info;
            if (info->id == SF_TYPE_MXIC) {
                support_flash_id = SF_TYPE_MXIC;
            } else if (info->id == SF_TYPE_WINBOND) {
                support_flash_id = SF_TYPE_WINBOND;
            } else if (info->id == SF_TYPE_GD) {
                support_flash_id = SF_TYPE_GD;
            } else if (info->id == SF_TYPE_MICRON) {
                support_flash_id = SF_TYPE_MICRON;
            }
        }
    }

    if (!spi_chip_info) {
        log_hal_info("Don't support flash: %s\n", (uint8_t *)jedec);
        //assert(0);
        spi_chip_info = &chips_data[1];
    }
    sf_drvier_status = SF_DRV_READY;
}


ATTR_TEXT_IN_TCM void flash_init(int security_enable)
{
    int i;
    unsigned long jedec;
    unsigned char buf[5];
    const struct chip_info *info;
    
    /*
     * Serial flash controller init.
     * restore it to default setting
     */
    sfc_Init(security_enable);

    gSFC_max_data_size = sfc_getMaxGPRamSize();

    flash_reset();

    spi_chip_info = NULL;

    flash_read_jedec_id(buf, 3);
    jedec = (unsigned long)(((unsigned long)buf[1] << 24) | ((unsigned long)buf[2] << 16));

    log_hal_info("spi device id: %x %x %x (%x)\n", buf[0], buf[1], buf[2], (unsigned int)jedec);

    for (i = 1; i < sizeof(chips_data) / sizeof(chips_data[0]); i++) {
        info = &chips_data[i];
        if (info->id == buf[0] && info->jedec_id == jedec) {
            log_hal_info("find flash: %s\n", info->name);
            spi_chip_info = info;
        }
    }

    if (!spi_chip_info) {
        log_hal_info("Don't support flash: %s\n", (uint8_t *)jedec);
        //assert(0);
        spi_chip_info = &chips_data[1];
    }
}


/*
 * Service routine to read status register until ready, or timeout occurs.
 * Returns non-zero if error.
 */
ATTR_TEXT_IN_TCM int flash_wait_ready(int sleep_ms)
{
    int count;
    unsigned char sr = 0;

    //udelay(1000 * sleep_ms);

    /* one chip guarantees max 5 msec wait here after page writes,
     * but potentially three seconds (!) after page erase.
     */
    for (count = 0;  count < ((sleep_ms + 1) * 1000); count++) {
        if ((flash_read_sr(&sr)) < 0) {
            break;
        } else if (!(sr & SR_WIP)) {
            return 0;
        }

        sfc_delay_ms(1);
        /* REVISIT sometimes sleeping would be best */
    }

    //log_hal_info("%s: read_sr fail: %x\n", __func__, sr);
    assert(0);
    return -1;
}

ATTR_TEXT_IN_TCM int flash_read_jedec_id(unsigned char *buf, int buf_size)
{
    unsigned char code;
    int rc;
    code = OPCODE_RD_JEDEC_ID;
    rc = sfc_read(&code, 1, buf, buf_size);

    return rc;
}

ATTR_TEXT_IN_TCM int flash_read_manu_id(unsigned char *buf, int buf_size)
{
    unsigned char code[4] = {0};
    int rc;
    code[0] = OPCODE_RD_MANU_ID;
    rc = sfc_read(code, 4, buf, buf_size);

    return rc;
}

ATTR_TEXT_IN_TCM int flash_read_manu_id_quad(unsigned char *buf, int buf_size)
{
    unsigned char code[4] = {0};
    int rc;
    code[0] = OPCODE_RD_MANU_ID;
    rc = sfc_read(code, 4, buf, buf_size);

    return rc;
}


ATTR_TEXT_IN_TCM int flash_read(unsigned char *buf, unsigned int from, int len)
{
    unsigned char cmd[4];
    int rdlen;

    /* sanity checks */
    if (len == 0) {
        return 0;
    }

    /* Wait till previous write/erase is done. */
    if (flash_wait_ready(1)) {
        /* REVISIT status return?? */
        return -1;
    }

    sf_drvier_status = SF_DRV_READING;
    cmd[0] = OPCODE_READ;
    cmd[1] = from >> 16;
    cmd[2] = from >> 8;
    cmd[3] = from;

    rdlen = sfc_read(cmd, 4, buf, len);

    //if (rdlen != len)
    //log_hal_info("warning: rdlen != len\n");

    return rdlen;
}


ATTR_TEXT_IN_TCM int flash_fast_read(unsigned char *buf, unsigned int from, int len, int dummy_cycle)
{
    unsigned char cmd[32];
    int cmd_len;
    int rdlen, i;

    /* sanity checks */
    if (len == 0) {
        return 0;
    }

    /* Wait till previous write/erase is done. */
    if (flash_wait_ready(1)) {
        /* REVISIT status return?? */
        return -1;
    }

    cmd[0] = OPCODE_FAST_READ;
    cmd[1] = from >> 16;
    cmd[2] = from >> 8;
    cmd[3] = from;
    cmd_len = 4;

    for (i = 0; i < dummy_cycle && i < sizeof(cmd) - 4; i++) {
        cmd[4 + i] = 0x0;
        cmd_len++;
    }

    rdlen = sfc_read(cmd, cmd_len, buf, len);

    //if (rdlen != len)
    //log_hal_info("warning: rdlen != len\n");

    return rdlen;
}


ATTR_TEXT_IN_TCM inline int  flash_direct_read(unsigned char *buf, unsigned int from, int len)
{
    sf_drvier_status = SF_DRV_DIRECT_READING;
    memcpy(buf, (void *)(from + CM4_EXEC_IN_PLACE_BASE) , len);
    sf_drvier_status = SF_DRV_READY;
    return len;
}

ATTR_TEXT_IN_TCM inline int flash_direct_read_nocache(unsigned char *buf, unsigned int from, int len)
{
    memcpy(buf, (void *)(from + CM4_SPIFLASH_BASE) , len);
    return len;
}


ATTR_TEXT_IN_TCM static int flash_write_sr(unsigned char *val)
{
    int retval;
    unsigned  char code = OPCODE_WRSR;

    retval = sfc_write(&code, 1, val, 1);

    if (retval != 1) {
        //log_hal_info("%s: ret: %x\n", __func__, retval);
        return -1;
    }
    return 0;
}

ATTR_TEXT_IN_TCM int flash_read_sr(unsigned char *val)
{
    int retval;
    unsigned char code = OPCODE_RDSR;

    retval = sfc_read(&code, 1, val, 1);
    if (retval != 1) {
        //log_hal_info("%s: ret: %x\n", __func__, retval);
        return -1;
    }
    return 0;
}

ATTR_TEXT_IN_TCM int flash_read_sr2(unsigned char *val)
{
    int retval;
//    int length = 1;
    unsigned char code = OPCODE_RDSR_2;

    if (support_flash_id == SF_TYPE_WINBOND || support_flash_id == SF_TYPE_GD) {
        //windbond is 0x35
        code = OPCODE_RDSR_2;
    } else if (support_flash_id == SF_TYPE_MXIC) {
        //mxic is 0x2B
        code = OPCODE_RD_SECST;
    } else if (support_flash_id == SF_TYPE_MICRON) {
        //uint8_t temp[2];
        //flag register 0x70
        code = 0x70;
    } else if (support_flash_id == SF_TYPE_CUSTOMER) {
        //flag register
        code = customer_flash_read_SR2_comand();
    }
    else {
        log_hal_info("flash_read_sr2: no init flash !\n");
        code = OPCODE_RDSR;
    }

    retval = sfc_read(&code, 1, val, 1);
    if (retval != 1) {
        log_hal_info("%s: ret: %x\n", __func__, retval);
        return -1;
    }
    return 0;
}

ATTR_TEXT_IN_TCM int flash_unprotect(void)
{
    unsigned char sr = 0;

    if (flash_read_sr(&sr) < 0) {
        //log_hal_info("%s: read_sr fail: %x\n", __func__, sr);
        return -1;
    }

    if ((sr & (SR_BP0 | SR_BP1 | SR_BP2)) != 0) {
        sr = 0;
        flash_write_sr(&sr);
    }
    return 0;
}

/*
 * write all -- BP0, BP1, BP2 to 1
 */
ATTR_TEXT_IN_TCM int flash_protect(void)
{
    unsigned char sr = 0;

    if (flash_read_sr(&sr) < 0) {
        //log_hal_info("%s: read_sr fail: %x\n", __func__, sr);
        return -1;
    }

    sr |= (SR_BP0 | SR_BP1 | SR_BP2);
    flash_write_sr(&sr);
    return 0;
}


ATTR_TEXT_IN_TCM int flash_write_enable(void)
{
    unsigned char code = OPCODE_WREN;
    return sfc_write(&code, 1, NULL, 0);
}

ATTR_TEXT_IN_TCM int flash_write_disable(void)
{
    unsigned char code = OPCODE_WRDI;
    return sfc_write(&code, 1, NULL, 0);
}

ATTR_TEXT_IN_TCM int flash_erase_sector(unsigned int offset)
{
    unsigned char buf[5];
    uint32_t savedMask;
    int32_t result = 0;

    /* Wait until finished previous write command. */
    if (flash_wait_ready(3)) {
        return -1;
    }

    sf_drvier_status = SF_DRV_SECTOR_64K_ERASING;
    do {
        result = SF_DAL_CheckDeviceReady(0, 0);
    } while (-121 == result);

    savedMask = save_and_set_interrupt_mask();

    /* Send write enable, then erase commands. */
    buf[0] = OPCODE_SE;
    buf[1] = offset >> 16;
    buf[2] = offset >> 8;
    buf[3] = offset;
    SF_DAL_FLAG_BUSY_SET();
    sfc_write(buf, 4, 0 , 0);
    restore_interrupt_mask(savedMask);

    do {
        result = SF_DAL_CheckDeviceReady(0, 0);
    } while (-121 == result);

    flash_write_disable();
    return 0;
}

ATTR_TEXT_IN_TCM int flash_erase_sector_32k(unsigned int offset)
{
    unsigned char buf[5];
    uint32_t savedMask;
    int32_t result = 0;

    /* Wait until finished previous write command. */
    if (flash_wait_ready(3)) {
        return -1;
    }

    sf_drvier_status = SF_DRV_SECTOR_32K_ERASING;
    do {
        result = SF_DAL_CheckDeviceReady(0, 0);
    } while (-121 == result);

    savedMask = save_and_set_interrupt_mask();
    buf[0] = OPCODE_SE_32K;
    buf[1] = offset >> 16;
    buf[2] = offset >> 8;
    buf[3] = offset;
    SF_DAL_FLAG_BUSY_SET();
    sfc_write(buf, 4, 0 , 0);
    do {
        result = SF_DAL_CheckDeviceReady(0, 0);
    } while (-121 == result);

    restore_interrupt_mask(savedMask);

    flash_write_disable();
    return 0;
}

ATTR_TEXT_IN_TCM int flash_erase_page(unsigned int offset)
{
    unsigned char buf[5];
    uint32_t savedMask;
    int32_t result = 0;

    /* Wait until finished previous write command. */
    if (flash_wait_ready(3)) {
        return -1;
    }

    sf_drvier_status = SF_DRV_SECTOR_4K_ERASING;
    do {
        result = SF_DAL_CheckDeviceReady(0, 0);
    } while (-121 == result);

    /* Send write enable, then erase commands. */
    savedMask = save_and_set_interrupt_mask();
    buf[0] = OPCODE_PE;
    buf[1] = offset >> 16;
    buf[2] = offset >> 8;
    buf[3] = offset;
    SF_DAL_FLAG_BUSY_SET();

    sfc_write(buf, 4, 0 , 0);
    restore_interrupt_mask(savedMask);

    do {
        result = SF_DAL_CheckDeviceReady(0, 0);
    } while (-121 == result);

    flash_write_disable();
    return 0;
}

ATTR_TEXT_IN_TCM int flash_erase_chip(void)
{
    unsigned char buf[5];

    /* Wait until finished previous write command. */
    if (flash_wait_ready(3)) {
        return -1;
    }

    sf_drvier_status = SF_DRV_CHIP_ERASING;
    /* Send write enable, then erase commands. */
    flash_write_enable();
    flash_unprotect();

    buf[0] = OPCODE_CE;

    sfc_write(buf, 1, 0 , 0);

    if (flash_wait_ready(30000)) {
        return -1;
    }

    flash_write_disable();
    sf_drvier_status = SF_DRV_READY;
    return 0;
}


ATTR_TEXT_IN_TCM int flash_erase(unsigned int offs, int len)
{
    //ra_dbg("%s: offs:%x len:%x\n", __func__, offs, len);

    ///* sanity checks */
    if (len == 0) {
        return 0;
    }

    /* now erase those pages */
    while (len > 0) {
        if (flash_erase_page(offs)) {
            return -1;
        }

        offs += spi_chip_info->page_size;
        len -=  spi_chip_info->page_size;
    }

    return 0;
}



ATTR_TEXT_IN_TCM int flash_write(const unsigned char *buf, unsigned int to, int len)
{
    unsigned long page_offset, /*max_size,*/ page_size;
    int rc = 0, retlen = 0;
    int mode;
    unsigned char cmd[5];
    int32_t result;
    uint32_t savedMask;

    uint32_t To  = (uint32_t)to;
    const uint8_t *Buf = buf;
    const uint8_t *p_data_first;

    /* sanity checks */
    if (len == 0) {
        return 0 ;
    }

    if (to + len > spi_chip_info->page_size * spi_chip_info->n_pages) {
        return -1;
    }

    if (buf==0) {
        return -1;
    }

    /* Wait until finished previous write command. */
    if (flash_wait_ready(2)) {
        return -1;
    }

    do {
        result = SF_DAL_CheckDeviceReady(0, 0);
    } while (-121 == result);

    sf_drvier_status = SF_DRV_PROGRAMMING;

    mode = sfc_GetDirectMode();
    /* Set up the opcode in the write buffer. */
    if ((mode == SPIQ) && spi_chip_info-> quad_program_cmd) {
        cmd[0] = spi_chip_info->quad_program_cmd;
    } else {
        cmd[0] = OPCODE_PP;
    }

    /* what page do we start with? */
    page_offset = to % gSFC_max_data_size;

    /* write everything in PAGESIZE chunks */
    while (len > 0) {
        page_size = min(len, (gSFC_max_data_size - page_offset));
        page_offset = 0;
        p_data_first = Buf;

        /* write the next page to flash */
        cmd[1] = To >> 16;
        cmd[2] = To >> 8;
        cmd[3] = To;

        if (flash_wait_ready(3)) {
            return -1;
        }

        flash_write_enable();
        savedMask = save_and_set_interrupt_mask();
        SF_DAL_FLAG_BUSY_SET();
        rc = sfc_write(cmd, 4, p_data_first, page_size);
        restore_interrupt_mask(savedMask);

        do {
            result = SF_DAL_CheckReadyAndResume(0, (kal_uint32)to + page_offset - 1, *(buf + page_offset - 1));
        } while (0 == result);

        if (rc > 0) {
            retlen += rc;
            if (rc < page_size) {
                //log_hal_info("%s: rc:%x page_size:%x\n", __func__, rc, (unsigned int)page_size);
                flash_write_disable();
                assert(0);
                return retlen;
            }
        }

        len -= page_size;
        To += page_size;
        Buf += page_size;
    }

    do {
        result = SF_DAL_CheckDeviceReady(0, 0);
    } while (-121 == result);

    flash_write_disable();
    return retlen;
}

/*
 * Currently we assume the size of loader & freertos/mbed is 4k align.
 */
ATTR_TEXT_IN_TCM int flash_erase_write_security_crc(unsigned int to)
{
    int len = 4, rc = 0;
    unsigned char cmd[5];

    if (to % FLASH_ERASE_4K) {
        return -1;
    }

    if (to + len > spi_chip_info->page_size * spi_chip_info->n_pages) {
        return -1;
    }

    sf_drvier_status = SF_DRV_ERASE_WRITE_CRC;

    /* Wait until finished previous write command. */
    if (flash_wait_ready(2)) {
        return -1;
    }

    flash_erase_page(to);

    cmd[0] = OPCODE_PP;
    cmd[1] = to >> 16;
    cmd[2] = to >> 8;
    cmd[3] = to;

    flash_write_enable();
    flash_unprotect();

    rc = sfc_security_write_crc(cmd, 4);
    sf_drvier_status = SF_DRV_READY;
    return rc;
}

ATTR_TEXT_IN_TCM int flash_erase_write(const unsigned char *buf, unsigned int offs, int count)
{
    unsigned char block[FLASH_ERASE_4K];

    int blocksize = FLASH_ERASE_4K;
    int blockmask = blocksize - 1;

    while (count > 0) {
#define BLOCK_ALIGNE(a) (((a) & blockmask))
        if (BLOCK_ALIGNE(offs) || (count < blocksize)) {
            unsigned int piece, blockaddr;
            int piece_size;

            blockaddr = offs & ~blockmask;

            if (flash_direct_read_nocache(block, blockaddr, blocksize) != blocksize) {
                return -2;
            }

            sf_drvier_status = SF_DRV_ERASE_WRITE;
            piece = offs & blockmask;
            piece_size = min(count, blocksize - piece);
            memcpy(block + piece, buf, piece_size);

            if (flash_erase(blockaddr, blocksize) != 0) {
                return -3;
            }

            if (flash_write(block, blockaddr, blocksize) != blocksize) {
                return -4;
            }

            buf += piece_size;
            offs += piece_size;
            count -= piece_size;
        } else {
            unsigned int aligned_size = count & ~blockmask;

            if (flash_erase(offs, aligned_size) != 0) {
                sf_drvier_status = SF_DRV_READY;
                return -1;
            }
            if (flash_write(buf, offs, aligned_size) != aligned_size) {
                sf_drvier_status = SF_DRV_READY;
                return -1;
            }

            buf += aligned_size;
            offs += aligned_size;
            count -= aligned_size;
        }
    }
    sf_drvier_status = SF_DRV_READY;
    return 0;
}


ATTR_TEXT_IN_TCM FLASH_DRV_STATE query_spi_flash_status(void)
{
    if (sf_drvier_status == SF_DRV_NOT_READY) {
        return FLASH_NOT_READY;
    }

    if (sf_drvier_status == SF_DRV_READY) {
        return FLASH_READY;
    }

    if (sf_drvier_status == SF_DRV_PROGRAMMING ||
            sf_drvier_status == SF_DRV_ERASE_WRITE ||
            sf_drvier_status == SF_DRV_ERASE_WRITE_CRC) {
        return FLASH_PROGRAMMING;
    } else {
        return FLASH_ERASING;
    }
}


#if defined(MTK_FLASH_POWER_SUPPLY_LOCK_DOWN)
ATTR_TEXT_IN_TCM int flash_software_lockdown_from_top(uint32_t size)
{
    uint8_t cmd[3];
    uint8_t sr1,sr2;
    int retval;
    unsigned  char code = OPCODE_WRSR;
    uint8_t data[2];

    cmd[0] = code;
    flash_read_sr(&sr1);

    switch (size)
    {
        case HAL_FLASH_LOCK_DOWN_4K:
            data[0] = ((sr1 & 0x11) | 0x64);  // set SEC and TB and  BP0
            break;

        case HAL_FLASH_LOCK_DOWN_8K:
            data[0] = ((sr1 & 0x11) | 0x68);  // set SEC and TB and  BP1
            break;

        case HAL_FLASH_LOCK_DOWN_16K:
            data[0] = ((sr1 & 0x11) | 0x6C);  // set SEC and TB and  BP0
            break;

        case HAL_FLASH_LOCK_DOWN_32K:
            data[0] = ((sr1 & 0x11) | 0x70);  // set SEC and TB and  BP0
            break;
        case HAL_FLASH_LOCK_DOWN_64K:
            data[0] = ((sr1 & 0x11) | 0x24);  // set TB and  BP0
            break;
        case HAL_FLASH_LOCK_DOWN_128K:
            data[0] = ((sr1 & 0x11) | 0x28);  // set TB and  BP1
            break;
        case HAL_FLASH_LOCK_DOWN_256K:
            data[0] = ((sr1 & 0x11) | 0x2C);  // set TB and  BP1 & BP0
            break;
        case HAL_FLASH_LOCK_DOWN_512K:
            data[0] = ((sr1 & 0x11) | 0x30);  // set TB and  BP2
            break;

        case HAL_FLASH_LOCK_DOWN_1M:
            data[0] = ((sr1 & 0x11) | 0x34);  // set TB and  BP2 & BP0
            break;

        case HAL_FLASH_LOCK_DOWN_2M:
            data[0] = ((sr1 & 0x11) | 0x38);  // set TB and  BP2 & BP1
            break;

        case HAL_FLASH_LOCK_DOWN_BT_4K:
            data[0] = ((sr1 & 0x11) | 0x44);  // set SEC  and  BP0
            break;

        case HAL_FLASH_LOCK_DOWN_BT_8K:
            data[0] = ((sr1 & 0x11) | 0x48);   // set SEC  and  BP1
            break;

        case HAL_FLASH_LOCK_DOWN_BT_16K:
            data[0] = ((sr1 & 0x11) | 0x4C);   // set SEC  and   BP1 & BP0
            break;

        case HAL_FLASH_LOCK_DOWN_BT_32K:
            data[0] = ((sr1 & 0x11) | 0x50);   // set SEC  and  BP2
            break;

        case HAL_FLASH_LOCK_DOWN_4M:
            data[0] = ((sr1 & 0x11) | 0x3C);  // set TB and  BP2 & BP1 & BP0
            break;

        default:
            return (retval=-7);
    }

    flash_read_sr2(&sr2);
    data[1] = (sr2 | 0x1);    //set s8
    flash_write_enable();
    retval = sfc_write(cmd, 1, data, 2);

    if (flash_wait_ready(20)) {
        return (retval = -8);
    };

    return retval;
}

#endif
#endif

