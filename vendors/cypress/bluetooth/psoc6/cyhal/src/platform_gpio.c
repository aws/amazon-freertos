/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 * PSoC 6 platform GPIO driver
 */
#include "stdint.h"
#include "string.h"
#include "platform_peripheral.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define PLATFORM_MAX_PORT_NUM           (15)
#define PLATFORM_MAX_PINS_ON_PORT       (8)
#define PLATFORM_GPIO_TOTAL_PIN_NUMBERS (PLATFORM_MAX_PORT_NUM * PLATFORM_MAX_PINS_ON_PORT)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
 /*
  * Internal data structure describing the PSoC 6 possible chip variations for each Pin
  * for its expected use as GPIO. The other HSIOM modes and implications are Not addressed here currently
  */

typedef struct
{
    wiced_bool_t        valid_on_chip;          /* Boolean indication if Port is Valid on PSoC 6 chip */
    uint32_t            valid_pin_mask;         /* Pin Number within Port */

    /* Valid HSIOM Configuration for GPIO operation for this Pin */
    en_hsiom_sel_t      valid_hsiom_val[PLATFORM_MAX_PINS_ON_PORT];
} platform_gpio_table_entry;

/*
 * Data structure for saving current configuration of GPIO pin based on platform APIs. This is a shadow copy,
 * the PSoC 6 does allow read of registers to get the current value but the s/w ia maintaining a shadow copy.
 */

typedef struct
{
    /* Flag to indicate the platform APIs were called to configure the GPIO */
    wiced_bool_t                 valid_user_cfg;
    en_hsiom_sel_t               user_hsiom;            /* currently configured HSIOM Configuration for GPIO */
    platform_pin_config_t        config;                /* Pin Pull Up/Down configuration */
} platform_gpio_setting_t;


/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/
/*
 * Table for all valid gpio pins maintained Per PSoC 6 chip describing which IP Pins are brought out
 * in chip package and can be used by users. The expectation is that more tables will be compiled with
 * ifdef macros when newer PSoC 6 based CPUs are supported.
 */

static const platform_gpio_table_entry platform_gpio_table[] =
{

     /* Port 0 */
     { .valid_on_chip = 1, .valid_pin_mask = 0x3f,
                           {P0_0_GPIO, P0_1_GPIO, P0_2_GPIO, P0_3_GPIO,
                            P0_4_GPIO, P0_5_GPIO}},

     /* Port 1 */
     { .valid_on_chip = 1, .valid_pin_mask = 0x3f,
                           {P1_0_GPIO, P1_1_GPIO, P1_2_GPIO, P1_3_GPIO,
                            P1_4_GPIO, P1_5_GPIO}},

     /* Port 2 */
     { .valid_on_chip = 1, .valid_pin_mask = 0xff,
                           {P2_0_GPIO, P2_1_GPIO, P2_2_GPIO, P2_3_GPIO,
                            P2_4_GPIO, P2_5_GPIO, P2_6_GPIO, P2_7_GPIO}},

     /* Port 3 */
     { .valid_on_chip = 1, .valid_pin_mask = 0x3f,
                           {P3_0_GPIO, P3_1_GPIO, P3_2_GPIO, P3_3_GPIO,
                            P3_4_GPIO, P3_5_GPIO}},

     /* Port 4 */
     { .valid_on_chip = 1, .valid_pin_mask = 0x03,
                           {P4_0_GPIO, P4_1_GPIO}},

     /* Port 5 */
     { .valid_on_chip = 1, .valid_pin_mask = 0xff,
                           {P5_0_GPIO, P5_1_GPIO, P5_2_GPIO, P5_3_GPIO,
                            P5_4_GPIO, P5_5_GPIO, P5_6_GPIO, P5_7_GPIO}},

     /* Port 6 */
     { .valid_on_chip = 1, .valid_pin_mask = 0xff,
                           {P6_0_GPIO, P6_1_GPIO, P6_2_GPIO, P6_3_GPIO,
                            P6_4_GPIO, P6_5_GPIO, P6_6_GPIO, P6_7_GPIO}},

     /* Port 7 */
     { .valid_on_chip = 1, .valid_pin_mask = 0xff,
                           {P7_0_GPIO, P7_1_GPIO, P7_2_GPIO, P7_3_GPIO,
                            P7_4_GPIO, P7_5_GPIO, P7_6_GPIO, P7_7_GPIO}},

     /* Port 8 */
     { .valid_on_chip = 1, .valid_pin_mask = 0xff,
                           {P8_0_GPIO, P8_1_GPIO, P8_2_GPIO, P8_3_GPIO,
                            P8_4_GPIO, P8_5_GPIO, P8_6_GPIO, P8_7_GPIO}},

     /* Port 9 */
     { .valid_on_chip =  1, .valid_pin_mask = 0xff,
                            {P9_0_GPIO, P9_1_GPIO, P9_2_GPIO, P9_3_GPIO,
                            P9_4_GPIO, P9_5_GPIO, P9_6_GPIO, P9_7_GPIO}},

     /* Port 10 */
     { .valid_on_chip = 1, .valid_pin_mask = 0xff,
                           {P10_0_GPIO, P10_1_GPIO, P10_2_GPIO, P10_3_GPIO,
                            P10_4_GPIO, P10_5_GPIO, P10_6_GPIO, P10_7_GPIO}},

     /* Port 11 */
     { .valid_on_chip = 1, .valid_pin_mask = 0xff,
                           {P11_0_GPIO, P11_1_GPIO, P11_2_GPIO, P11_3_GPIO,
                            P11_4_GPIO, P11_5_GPIO, P11_6_GPIO, P11_7_GPIO}},

     /* Port 12 */
     { .valid_on_chip = 1, .valid_pin_mask = 0xff,
                           {P12_0_GPIO, P12_1_GPIO, P12_2_GPIO, P12_3_GPIO,
                            P12_4_GPIO, P12_5_GPIO, P12_6_GPIO, P12_7_GPIO}},

     /* Port 13 */
     { .valid_on_chip = 1, .valid_pin_mask = 0xff,
                           {P13_0_GPIO, P13_1_GPIO, P13_2_GPIO, P13_3_GPIO,
                            P13_4_GPIO, P13_5_GPIO, P13_6_GPIO, P13_7_GPIO}},

};

#define PLATFORM_GPIO_TABLE_SIZE  ( sizeof( platform_gpio_table ) / sizeof( platform_gpio_table[0] ) )

/*
 * Table of structures for each Pin in GPIO for chip
 */
static platform_gpio_setting_t gpio_pin_cfg[PLATFORM_GPIO_TOTAL_PIN_NUMBERS] = {{0}};


/******************************************************
 *            Platform Function Definitions
 ******************************************************/


static  wiced_bool_t is_valid_pin( uint32_t port_num, uint32_t pin_num, en_hsiom_sel_t hsiom, GPIO_PRT_Type** port_base )
{
    if ( port_num >= PLATFORM_GPIO_TABLE_SIZE )
    {
       /* PortNum out of range of valid Port numbers */
       return ( WICED_FALSE );
    }

    if ( ! ( platform_gpio_table[port_num].valid_on_chip ) )
    {
       /* Not valid GPIO port_num on selected Chip */
       return ( WICED_FALSE );
    }

    if ( ! ( ( 1 << pin_num ) & platform_gpio_table[port_num].valid_pin_mask ) )
    {
       /* Not valid GPIO pin_num on port_num */
       return ( WICED_FALSE );
    }

    if ( ! ( platform_gpio_table[port_num].valid_hsiom_val[pin_num] == hsiom ) )
    {
       return ( WICED_FALSE );
    }

    *port_base = Cy_GPIO_PortToAddr( port_num );
    return ( WICED_TRUE );
}

static wiced_bool_t get_gpio_port_pin_index ( const platform_gpio_t* gpio, uint32_t* index )
{
    GPIO_PRT_Type *base = NULL;

    if ( gpio == NULL )
    {
        return ( WICED_FALSE );
    }

    if ( is_valid_pin( gpio->port_num, gpio->pin_num, gpio->hsiom, &base ) == WICED_FALSE )
    {
        return ( WICED_FALSE );
    }

    *index = ( gpio->port_num * PLATFORM_MAX_PINS_ON_PORT ) + gpio->pin_num;

    return ( WICED_TRUE );

}


platform_result_t platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config )
{
    uint32_t drive_mode = CY_GPIO_DM_STRONG_IN_OFF;
    uint32_t init_outval = 0;
    uint32_t index = 0;

    GPIO_PRT_Type *base = NULL;

    if ( gpio == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( is_valid_pin( gpio->port_num, gpio->pin_num, gpio->hsiom, &base ) == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    switch ( config )
    {
        case INPUT_PULL_UP:
            init_outval = 1;
            drive_mode = CY_GPIO_DM_PULLUP;
            break;

        case INPUT_PULL_DOWN:
            init_outval = 0;
            drive_mode = CY_GPIO_DM_PULLDOWN;
            break;

        case INPUT_HIGH_IMPEDANCE:
            init_outval = 0;
            drive_mode = CY_GPIO_DM_HIGHZ;
            break;

        case OUTPUT_PUSH_PULL:
            init_outval = 0;
            drive_mode = CY_GPIO_DM_STRONG_IN_OFF;
            break;

        case OUTPUT_OPEN_DRAIN_NO_PULL:
            init_outval = 0;
            drive_mode = CY_GPIO_DM_OD_DRIVESLOW_IN_OFF;
            break;

        case OUTPUT_OPEN_DRAIN_PULL_UP:
            init_outval = 1;
            drive_mode = CY_GPIO_DM_OD_DRIVESHIGH_IN_OFF;
            break;

        default:
            wiced_assert( "Not supported", 0 );
            return PLATFORM_UNSUPPORTED;
    }


    Cy_GPIO_Pin_FastInit( base, gpio->pin_num, drive_mode, init_outval, gpio->hsiom );

    if (get_gpio_port_pin_index ( gpio, &index ) == WICED_FALSE ) {
        return PLATFORM_UNSUPPORTED;
    }

    gpio_pin_cfg[index].valid_user_cfg = WICED_TRUE;
    gpio_pin_cfg[index].user_hsiom = gpio->hsiom;
    gpio_pin_cfg[index].config = config;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_deinit( const platform_gpio_t* gpio )
{
    uint32_t index = 0;
    uint32_t drive_mode = CY_GPIO_DM_HIGHZ;
    uint32_t init_outval = 0;
    uint32_t init_hsiom = 0;      /* Should be default hsiom value for generic GPIO, which is 0 for PSoC 6 */

    GPIO_PRT_Type *base = NULL;

    if ( gpio == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( is_valid_pin( gpio->port_num, gpio->pin_num, gpio->hsiom, &base ) == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if (get_gpio_port_pin_index ( gpio, &index ) == WICED_FALSE ) {
        return PLATFORM_UNSUPPORTED;
    }

    gpio_pin_cfg[index].valid_user_cfg = WICED_FALSE;
    gpio_pin_cfg[index].user_hsiom = 0;
    gpio_pin_cfg[index].config = 0;

    Cy_GPIO_Pin_FastInit( base, gpio->pin_num, drive_mode, init_outval, init_hsiom );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_output_high( const platform_gpio_t* gpio )
{
    GPIO_PRT_Type *base = NULL;

    if ( gpio == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( is_valid_pin( gpio->port_num, gpio->pin_num, gpio->hsiom, &base ) == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    Cy_GPIO_Set( base, gpio->pin_num );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_output_low( const platform_gpio_t* gpio )
{
    GPIO_PRT_Type *base = NULL;

    if ( gpio == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( is_valid_pin( gpio->port_num, gpio->pin_num, gpio->hsiom, &base ) == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    Cy_GPIO_Clr( base, gpio->pin_num );

    return PLATFORM_SUCCESS;
}

wiced_bool_t platform_gpio_input_get( const platform_gpio_t* gpio )
{
    GPIO_PRT_Type *base = NULL;
    uint32_t val = 0;

    wiced_assert( "bad argument", ( gpio != NULL ) );

    base = Cy_GPIO_PortToAddr( gpio->port_num );

    val = Cy_GPIO_Read( base, gpio->pin_num );

    if ( val )
    {
       return ( WICED_TRUE );
    }
    return ( WICED_FALSE );
}
