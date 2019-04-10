#include <string.h>
#include <stdint.h>

#include "hal_types.h"
#include "hci.h"
#include "gp_timer.h"
#include "hal.h"
#include "osal.h"
#include "hci_const.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"

#include "SDK_EVAL_Gpio.h"
#include "SDK_EVAL_Spi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

// Bluetooth device and local name
typedef struct {
	char * local_name;
	uint8_t local_name_size;
} LocalName_t;
