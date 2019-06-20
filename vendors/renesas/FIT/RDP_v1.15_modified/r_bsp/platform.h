/* As of today, we need a workaround to avoid the problem that the BSP_CFG_RTOS_USED in the r_bsp_config.h is
 * set to (0) every time of code generation by the Smart Configurator. We add a workaround modification in the
 * r_bsp_config.h which makes the setting of there ignored when the BSP_CFG_RTOS_USED is set to non-zero value
 * already and we set the BSP_CFG_RTOS_USED to (1) here so that the setting in the r_bsp_config.h is ignored. */
#if defined(BSP_CFG_RTOS_USED)
#undef BSP_CFG_RTOS_USED
#endif
#define BSP_CFG_RTOS_USED (1)

#include "r_bsp.h"
