/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  The macros defines herein are intended for use by applications which
 *  directly include this header. These macros should NOT be hard coded or
 *  copied into library source code.
 *
 *  Symbols declared as const are intended for use with libraries.
 *  Library source code must extern the correct symbol--which is resolved
 *  when the application is linked.
 *
 *  DO NOT EDIT - This file is generated for the CC3220SF_LAUNCHXL
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_CC3220SF_LAUNCHXL
#ifndef DeviceFamily_CC3220
#define DeviceFamily_CC3220
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  ======== GPIO ========
 */

/* P53 */
extern const uint_least8_t              CONFIG_GPIO_0_CONST;
#define CONFIG_GPIO_0                   0
/* P63 */
extern const uint_least8_t              CONFIG_GPIO_1_CONST;
#define CONFIG_GPIO_1                   1
/* P50 */
extern const uint_least8_t              CONFIG_GPIO_2_CONST;
#define CONFIG_GPIO_2                   2
/* P08 */
extern const uint_least8_t              CONFIG_GPIO_3_CONST;
#define CONFIG_GPIO_3                   3
#define CONFIG_TI_DRIVERS_GPIO_COUNT    4

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)


/*
 *  ======== I2C ========
 */

/*
 *  SCL: P01
 *  SDA: P02
 *  LaunchPad I2C
 */
extern const uint_least8_t              CONFIG_I2C_0_CONST;
#define CONFIG_I2C_0                    0
#define CONFIG_TI_DRIVERS_I2C_COUNT     1

/* ======== I2C Addresses and Speeds ======== */
#include <ti/drivers/I2C.h>

/* ---- CONFIG_I2C_0 I2C bus components ---- */

/* BMA222E address and max speed */
#define CONFIG_I2C_0_BMA222E_ADDR     (0x18)
#define CONFIG_I2C_0_BMA222E_MAXSPEED (400U) /* Kbps */

/* TMP006 address and max speed */
#define CONFIG_I2C_0_TMP006_ADDR     (0x41)
#define CONFIG_I2C_0_TMP006_MAXSPEED (3400U) /* Kbps */

/* CONFIG_I2C_0 max speed (supported by all components) */
#define CONFIG_I2C_0_MAXSPEED   (3400U) /* Kbps */
#define CONFIG_I2C_0_MAXBITRATE ((I2C_BitRate)I2C_3400kHz)


/*
 *  ======== NVS ========
 */

/*
 *  MOSI: P07
 *  MISO: P06
 *  SCLK: P05
 *  SS: P08
 */
extern const uint_least8_t              CONFIG_NVS_0_CONST;
#define CONFIG_NVS_0                    0
#define CONFIG_TI_DRIVERS_NVS_COUNT     1


/*
 *  ======== PWM ========
 */

/* P64, LaunchPad LED D10 (Red) */
extern const uint_least8_t              CONFIG_PWM_0_CONST;
#define CONFIG_PWM_0                    0
#define CONFIG_TI_DRIVERS_PWM_COUNT     1


/*
 *  ======== SPI ========
 */

extern const uint_least8_t              CONFIG_NWP_SPI_CONST;
#define CONFIG_NWP_SPI                  0
/*
 *  MOSI: P07
 *  MISO: P06
 *  SCLK: P05
 */
extern const uint_least8_t              CONFIG_SPI_0_CONST;
#define CONFIG_SPI_0                    1
#define CONFIG_TI_DRIVERS_SPI_COUNT     2


/*
 *  ======== UART ========
 */

/*
 *  TX: P55
 *  RX: P57
 *  XDS110 UART
 */
extern const uint_least8_t              CONFIG_UART_0_CONST;
#define CONFIG_UART_0                   0
/*
 *  TX: P03
 *  RX: P04
 */
extern const uint_least8_t              CONFIG_UART_1_CONST;
#define CONFIG_UART_1                   1
#define CONFIG_TI_DRIVERS_UART_COUNT    2


/*
 *  ======== Watchdog ========
 */

extern const uint_least8_t                  CONFIG_WATCHDOG_0_CONST;
#define CONFIG_WATCHDOG_0                   0
#define CONFIG_TI_DRIVERS_WATCHDOG_COUNT    1


/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
