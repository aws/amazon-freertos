/**
 * \file
 *
 * \brief  Microchip Crypto Auth device object
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef ATCA_DEVICE_H
#define ATCA_DEVICE_H
/*lint +flb */

#include "atca_iface.h"
/** \defgroup device ATCADevice (atca_)
   @{ */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ATCA_NO_PRAGMA_PACK
#pragma pack(push, 1)
#define ATCA_PACKED
#else
#define ATCA_PACKED     __attribute__ ((packed))
#endif

typedef struct ATCA_PACKED _atsha204a_config
{
    uint32_t SN03;
    uint32_t RevNum;
    uint32_t SN47;
    uint8_t  SN8;
    uint8_t  Reserved0;
    uint8_t  I2C_Enable;
    uint8_t  Reserved1;
    uint8_t  I2C_Address;
    uint8_t  Reserved2;
    uint8_t  OTPmode;
    uint8_t  ChipMode;
    uint16_t SlotConfig[16];
    uint16_t Counter[8];
    uint8_t  LastKeyUse[16];
    uint8_t  UserExtra;
    uint8_t  Selector;
    uint8_t  LockValue;
    uint8_t  LockConfig;
} atsha204a_config_t;

typedef struct ATCA_PACKED _atecc508a_config
{
    uint32_t SN03;
    uint32_t RevNum;
    uint32_t SN47;
    uint8_t  SN8;
    uint8_t  Reserved0;
    uint8_t  I2C_Enable;
    uint8_t  Reserved1;
    uint8_t  I2C_Address;
    uint8_t  Reserved2;
    uint8_t  OTPmode;
    uint8_t  ChipMode;
    uint16_t SlotConfig[16];
    uint8_t  Counter0[8];
    uint8_t  Counter1[8];
    uint8_t  LastKeyUse[16];
    uint8_t  UserExtra;
    uint8_t  Selector;
    uint8_t  LockValue;
    uint8_t  LockConfig;
    uint16_t SlotLocked;
    uint16_t RFU;
    uint32_t X509format;
    uint16_t KeyConfig[16];
} atecc508a_config_t;

typedef struct ATCA_PACKED _atecc608_config
{
    uint32_t SN03;
    uint32_t RevNum;
    uint32_t SN47;
    uint8_t  SN8;
    uint8_t  AES_Enable;
    uint8_t  I2C_Enable;
    uint8_t  Reserved1;
    uint8_t  I2C_Address;
    uint8_t  Reserved2;
    uint8_t  CountMatch;
    uint8_t  ChipMode;
    uint16_t SlotConfig[16];
    uint8_t  Counter0[8];
    uint8_t  Counter1[8];
    uint8_t  UseLock;
    uint8_t  VolatileKeyPermission;
    uint16_t SecureBoot;
    uint8_t  KdflvLoc;
    uint16_t KdflvStr;
    uint8_t  Reserved3[9];
    uint8_t  UserExtra;
    uint8_t  UserExtraAdd;
    uint8_t  LockValue;
    uint8_t  LockConfig;
    uint16_t SlotLocked;
    uint16_t ChipOptions;
    uint32_t X509format;
    uint16_t KeyConfig[16];
} atecc608_config_t;

#ifndef ATCA_NO_PRAGMA_PACK
#pragma pack(pop)
#endif

/** \brief ATCADeviceState says about device state
 */
typedef enum
{
    ATCA_DEVICE_STATE_UNKNOWN = 0,
    ATCA_DEVICE_STATE_SLEEP,
    ATCA_DEVICE_STATE_IDLE,
    ATCA_DEVICE_STATE_ACTIVE
} ATCADeviceState;


/** \brief atca_device is the C object backing ATCADevice.  See the atca_device.h file for
 * details on the ATCADevice methods
 */
struct atca_device
{
    atca_iface_t mIface;                /**< Physical interface */
    uint8_t      device_state;          /**< Device Power State */

    uint8_t  clock_divider;
    uint16_t execution_time_msec;

    uint8_t  session_state;             /**< Secure Session State */
    uint16_t session_counter;           /**< Secure Session Message Count */
    uint16_t session_key_id;            /**< Key ID used for a secure sesison */
    uint8_t* session_key;               /**< Session Key */
    uint8_t  session_key_len;           /**< Length of key used for the session in bytes */

    uint16_t options;                   /**< Nested command details parameter */

};

typedef struct atca_device * ATCADevice;

ATCA_STATUS initATCADevice(ATCAIfaceCfg* cfg, ATCADevice cadev);
ATCADevice newATCADevice(ATCAIfaceCfg *cfg);
ATCA_STATUS releaseATCADevice(ATCADevice ca_dev);
void deleteATCADevice(ATCADevice *ca_dev);

ATCAIface atGetIFace(ATCADevice dev);



#ifdef __cplusplus
}
#endif
#define ATCA_AES_ENABLE_EN_SHIFT                (0)
#define ATCA_AES_ENABLE_EN_MASK                 (0x01u << ATCA_AES_ENABLE_EN_SHIFT)

/* I2C */
#define ATCA_I2C_ENABLE_EN_SHIFT                (0)
#define ATCA_I2C_ENABLE_EN_MASK                 (0x01u << ATCA_I2C_ENABLE_EN_SHIFT)

/* Counter Match Feature */
#define ATCA_COUNTER_MATCH_EN_SHIFT             (0)
#define ATCA_COUNTER_MATCH_EN_MASK              (0x01u << ATCA_COUNTER_MATCH_EN_SHIFT)
#define ATCA_COUNTER_MATCH_KEY_SHIFT            (4)
#define ATCA_COUNTER_MATCH_KEY_MASK             (0x0Fu << ATCA_COUNTER_MATCH_KEY_SHIFT)
#define ATCA_COUNTER_MATCH_KEY(v)               (ATCA_COUNTER_MATCH_KEY_MASK & (v << ATCA_COUNTER_MATCH_KEY_SHIFT))

/* ChipMode */
#define ATCA_CHIP_MODE_I2C_EXTRA_SHIFT          (0)
#define ATCA_CHIP_MODE_I2C_EXTRA_MASK           (0x01u << ATCA_CHIP_MODE_I2C_EXTRA_SHIFT)
#define ATCA_CHIP_MODE_TTL_EN_SHIFT             (1)
#define ATCA_CHIP_MODE_TTL_EN_MASK              (0x01u << ATCA_CHIP_MODE_TTL_EN_SHIFT)
#define ATCA_CHIP_MODE_WDG_LONG_SHIFT           (2)
#define ATCA_CHIP_MODE_WDG_LONG_MASK            (0x01u << ATCA_CHIP_MODE_WDG_LONG_SHIFT)
#define ATCA_CHIP_MODE_CLK_DIV_SHIFT            (3)
#define ATCA_CHIP_MODE_CLK_DIV_MASK             (0x1Fu << ATCA_CHIP_MODE_CLK_DIV_SHIFT)
#define ATCA_CHIP_MODE_CLK_DIV(v)               (ATCA_CHIP_MODE_CLK_DIV_MASK & (v << ATCA_CHIP_MODE_CLK_DIV_SHIFT))

/* General Purpose Slot Config (Not ECC Private Keys) */
#define ATCA_SLOT_CONFIG_READKEY_SHIFT          (0)
#define ATCA_SLOT_CONFIG_READKEY_MASK           (0x0Fu << ATCA_SLOT_CONFIG_READKEY_SHIFT)
#define ATCA_SLOT_CONFIG_READKEY(v)             (ATCA_SLOT_CONFIG_READKEY_MASK & (v << ATCA_SLOT_CONFIG_READKEY_SHIFT))
#define ATCA_SLOT_CONFIG_NOMAC_SHIFT            (4)
#define ATCA_SLOT_CONFIG_NOMAC_MASK             (0x01u << ATCA_SLOT_CONFIG_NOMAC_SHIFT)
#define ATCA_SLOT_CONFIG_LIMITED_USE_SHIFT      (5)
#define ATCA_SLOT_CONFIG_LIMITED_USE_MASK       (0x01u << ATCA_SLOT_CONFIG_LIMITED_USE_SHIFT)
#define ATCA_SLOT_CONFIG_ENCRYPTED_READ_SHIFT   (6)
#define ATCA_SLOT_CONFIG_ENCRYPTED_READ_MASK    (0x01u << ATCA_SLOT_CONFIG_ENCRYPTED_READ_SHIFT)
#define ATCA_SLOT_CONFIG_IS_SECRET_SHIFT        (7)
#define ATCA_SLOT_CONFIG_IS_SECRET_MASK         (0x01u << ATCA_SLOT_CONFIG_IS_SECRET_SHIFT)
#define ATCA_SLOT_CONFIG_WRITE_KEY_SHIFT        (8)
#define ATCA_SLOT_CONFIG_WRITE_KEY_MASK         (0x0Fu << ATCA_SLOT_CONFIG_WRITE_KEY_SHIFT)
#define ATCA_SLOT_CONFIG_WRITE_KEY(v)           (ATCA_SLOT_CONFIG_WRITE_KEY_MASK & (v << ATCA_SLOT_CONFIG_WRITE_KEY_SHIFT))
#define ATCA_SLOT_CONFIG_WRITE_CONFIG_SHIFT     (12)
#define ATCA_SLOT_CONFIG_WRITE_CONFIG_MASK      (0x0Fu << ATCA_SLOT_CONFIG_WRITE_CONFIG_SHIFT)
#define ATCA_SLOT_CONFIG_WRITE_CONFIG(v)        (ATCA_SLOT_CONFIG_WRITE_CONFIG_MASK & (v << ATCA_SLOT_CONFIG_WRITE_CONFIG_SHIFT))

/* Slot Config for ECC Private Keys */
#define ATCA_SLOT_CONFIG_EXT_SIG_SHIFT          (0)
#define ATCA_SLOT_CONFIG_EXT_SIG_MASK           (0x01u << ATCA_SLOT_CONFIG_EXT_SIG_SHIFT)
#define ATCA_SLOT_CONFIG_INT_SIG_SHIFT          (1)
#define ATCA_SLOT_CONFIG_INT_SIG_MASK           (0x01u << ATCA_SLOT_CONFIG_INT_SIG_SHIFT)
#define ATCA_SLOT_CONFIG_ECDH_SHIFT             (2)
#define ATCA_SLOT_CONFIG_ECDH_MASK              (0x01u << ATCA_SLOT_CONFIG_ECDH_SHIFT)
#define ATCA_SLOT_CONFIG_WRITE_ECDH_SHIFT       (3)
#define ATCA_SLOT_CONFIG_WRITE_ECDH_MASK        (0x01u << ATCA_SLOT_CONFIG_WRITE_ECDH_SHIFT)
#define ATCA_SLOT_CONFIG_GEN_KEY_SHIFT          (8)
#define ATCA_SLOT_CONFIG_GEN_KEY_MASK           (0x01u << ATCA_SLOT_CONFIG_GEN_KEY_SHIFT)
#define ATCA_SLOT_CONFIG_PRIV_WRITE_SHIFT       (9)
#define ATCA_SLOT_CONFIG_PRIV_WRITE_MASK        (0x01u << ATCA_SLOT_CONFIG_PRIV_WRITE_SHIFT)

/* Use Lock */
#define ATCA_USE_LOCK_ENABLE_SHIFT              (0)
#define ATCA_USE_LOCK_ENABLE_MASK               (0x0Fu << ATCA_USE_LOCK_ENABLE_SHIFT)
#define ATCA_USE_LOCK_KEY_SHIFT                 (4)
#define ATCA_USE_LOCK_KEY_MASK                  (0x0Fu << ATCA_USE_LOCK_KEY_SHIFT)

/* Voltatile Key Permission */
#define ATCA_VOL_KEY_PERM_SLOT_SHIFT            (0)
#define ATCA_VOL_KEY_PERM_SLOT_MASK             (0x0Fu << ATCA_VOL_KEY_PERM_SLOT_SHIFT)
#define ATCA_VOL_KEY_PERM_SLOT(v)               (ATCA_VOL_KEY_PERM_SLOT_MASK & (v << ATCA_VOL_KEY_PERM_SLOT_SHIFT))
#define ATCA_VOL_KEY_PERM_EN_SHIFT              (7)
#define ATCA_VOL_KEY_PERM_EN_MASK               (0x01u << ATCA_VOL_KEY_PERM_EN_SHIFT)

/* Secure Boot */
#define ATCA_SECURE_BOOT_MODE_SHIFT             (0)
#define ATCA_SECURE_BOOT_MODE_MASK              (0x03u << ATCA_SECURE_BOOT_MODE_SHIFT)
#define ATCA_SECURE_BOOT_MODE(v)                (ATCA_SECURE_BOOT_MODE_MASK & (v << ATCA_SECURE_BOOT_MODE_SHIFT))
#define ATCA_SECURE_BOOT_PERSIST_EN_SHIFT       (3)
#define ATCA_SECURE_BOOT_PERSIST_EN_MASK        (0x01u << ATCA_SECURE_BOOT_PERSIST_EN_SHIFT)
#define ATCA_SECURE_BOOT_RAND_NONCE_SHIFT       (4)
#define ATCA_SECURE_BOOT_RAND_NONCE_MASK        (0x01u << ATCA_SECURE_BOOT_RAND_NONCE_SHIFT)
#define ATCA_SECURE_BOOT_DIGEST_SHIFT           (8)
#define ATCA_SECURE_BOOT_DIGEST_MASK            (0x0Fu << ATCA_SECURE_BOOT_DIGEST_SHIFT)
#define ATCA_SECURE_BOOT_DIGEST(v)              (ATCA_SECURE_BOOT_DIGEST_MASK & (v << ATCA_SECURE_BOOT_DIGEST_SHIFT))
#define ATCA_SECURE_BOOT_PUB_KEY_SHIFT          (12)
#define ATCA_SECURE_BOOT_PUB_KEY_MASK           (0x0Fu << ATCA_SECURE_BOOT_PUB_KEY_SHIFT)
#define ATCA_SECURE_BOOT_PUB_KEY(v)             (ATCA_SECURE_BOOT_PUB_KEY_MASK & (v << ATCA_SECURE_BOOT_PUB_KEY_SHIFT))

/* Slot Locked */
#define ATCA_SLOT_LOCKED(v)                     ((0x01 << v) & 0xFFFFu)

/* Chip Options */
#define ATCA_CHIP_OPT_POST_EN_SHIFT             (0)
#define ATCA_CHIP_OPT_POST_EN_MASK              (0x01u << ATCA_CHIP_OPT_POST_EN_SHIFT)
#define ATCA_CHIP_OPT_IO_PROT_EN_SHIFT          (1)
#define ATCA_CHIP_OPT_IO_PROT_EN_MASK           (0x01u << ATCA_CHIP_OPT_IO_PROT_EN_SHIFT)
#define ATCA_CHIP_OPT_KDF_AES_EN_SHIFT          (2)
#define ATCA_CHIP_OPT_KDF_AES_EN_MASK           (0x01u << ATCA_CHIP_OPT_KDF_AES_EN_SHIFT)
#define ATCA_CHIP_OPT_ECDH_PROT_SHIFT           (8)
#define ATCA_CHIP_OPT_ECDH_PROT_MASK            (0x03u << ATCA_CHIP_OPT_ECDH_PROT_SHIFT)
#define ATCA_CHIP_OPT_ECDH_PROT(v)              (ATCA_CHIP_OPT_ECDH_PROT_MASK & (v << ATCA_CHIP_OPT_ECDH_PROT_SHIFT))
#define ATCA_CHIP_OPT_KDF_PROT_SHIFT            (10)
#define ATCA_CHIP_OPT_KDF_PROT_MASK             (0x03u << ATCA_CHIP_OPT_KDF_PROT_SHIFT)
#define ATCA_CHIP_OPT_KDF_PROT(v)               (ATCA_CHIP_OPT_KDF_PROT_MASK & (v << ATCA_CHIP_OPT_KDF_PROT_SHIFT))
#define ATCA_CHIP_OPT_IO_PROT_KEY_SHIFT         (12)
#define ATCA_CHIP_OPT_IO_PROT_KEY_MASK          (0x0Fu << ATCA_CHIP_OPT_IO_PROT_KEY_SHIFT)
#define ATCA_CHIP_OPT_IO_PROT_KEY(v)            (ATCA_CHIP_OPT_IO_PROT_KEY_MASK & (v << ATCA_CHIP_OPT_IO_PROT_KEY_SHIFT))

/* Key Config */
#define ATCA_KEY_CONFIG_PRIVATE_SHIFT           (0)
#define ATCA_KEY_CONFIG_PRIVATE_MASK            (0x01u << ATCA_KEY_CONFIG_PRIVATE_SHIFT)
#define ATCA_KEY_CONFIG_PUB_INFO_SHIFT          (1)
#define ATCA_KEY_CONFIG_PUB_INFO_MASK           (0x01u << ATCA_KEY_CONFIG_PUB_INFO_SHIFT)
#define ATCA_KEY_CONFIG_KEY_TYPE_SHIFT          (2)
#define ATCA_KEY_CONFIG_KEY_TYPE_MASK           (0x07u << ATCA_KEY_CONFIG_KEY_TYPE_SHIFT)
#define ATCA_KEY_CONFIG_KEY_TYPE(v)             (ATCA_KEY_CONFIG_KEY_TYPE_MASK & (v << ATCA_KEY_CONFIG_KEY_TYPE_SHIFT))
#define ATCA_KEY_CONFIG_LOCKABLE_SHIFT          (5)
#define ATCA_KEY_CONFIG_LOCKABLE_MASK           (0x01u << ATCA_KEY_CONFIG_LOCKABLE_SHIFT)
#define ATCA_KEY_CONFIG_REQ_RANDOM_SHIFT        (6)
#define ATCA_KEY_CONFIG_REQ_RANDOM_MASK         (0x01u << ATCA_KEY_CONFIG_REQ_RANDOM_SHIFT)
#define ATCA_KEY_CONFIG_REQ_AUTH_SHIFT          (7)
#define ATCA_KEY_CONFIG_REQ_AUTH_MASK           (0x01u << ATCA_KEY_CONFIG_REQ_AUTH_SHIFT)
#define ATCA_KEY_CONFIG_AUTH_KEY_SHIFT          (8)
#define ATCA_KEY_CONFIG_AUTH_KEY_MASK           (0x0Fu << ATCA_KEY_CONFIG_AUTH_KEY_SHIFT)
#define ATCA_KEY_CONFIG_AUTH_KEY(v)             (ATCA_KEY_CONFIG_AUTH_KEY_MASK & (v << ATCA_KEY_CONFIG_AUTH_KEY_SHIFT))
#define ATCA_KEY_CONFIG_PERSIST_DISABLE_SHIFT   (12)
#define ATCA_KEY_CONFIG_PERSIST_DISABLE_MASK    (0x01u << ATCA_KEY_CONFIG_PERSIST_DISABLE_SHIFT)
#define ATCA_KEY_CONFIG_RFU_SHIFT               (13)
#define ATCA_KEY_CONFIG_RFU_MASK                (0x01u << ATCA_KEY_CONFIG_RFU_SHIFT)
#define ATCA_KEY_CONFIG_X509_ID_SHIFT           (14)
#define ATCA_KEY_CONFIG_X509_ID_MASK            (0x03u << ATCA_KEY_CONFIG_X509_ID_SHIFT)
#define ATCA_KEY_CONFIG_X509_ID(v)              (ATCA_KEY_CONFIG_X509_ID_MASK & (v << ATCA_KEY_CONFIG_X509_ID_SHIFT))
/** @} */
/*lint -flb*/
#endif
