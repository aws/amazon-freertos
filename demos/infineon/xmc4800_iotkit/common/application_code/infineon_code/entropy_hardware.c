#include <stdlib.h>

#include "entropy_hardware.h"
#include "xmc_vadc.h"

const static XMC_VADC_GLOBAL_CONFIG_t g_global_handle =
{
  .clock_config =
  {
    .analog_clock_divider = 3,
  },
};

const static XMC_VADC_GROUP_CONFIG_t g_group_handle;

const static XMC_VADC_BACKGROUND_CONFIG_t g_bgn_handle;

const static XMC_VADC_CHANNEL_CONFIG_t g_g0_ch5_handle =
{
  .alias_channel = XMC_VADC_CHANNEL_ALIAS_DISABLED,
  .result_reg_number = 5,
};

static void VADC_Init(void)
{
  XMC_VADC_GLOBAL_Init(VADC, &g_global_handle);
  XMC_VADC_GROUP_Init(VADC_G0, &g_group_handle);
  XMC_VADC_GROUP_SetPowerMode(VADC_G0, XMC_VADC_GROUP_POWERMODE_NORMAL);
  XMC_VADC_GLOBAL_StartupCalibration(VADC);

  XMC_VADC_GLOBAL_BackgroundInit(VADC, &g_bgn_handle);
  XMC_VADC_GROUP_ChannelInit(VADC_G0, 5, &g_g0_ch5_handle);
  XMC_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, 0, 5);
  XMC_VADC_GLOBAL_BackgroundEnableContinuousMode(VADC);
  XMC_VADC_GLOBAL_BackgroundTriggerConversion(VADC);
}

static void VADC_Deinit(void)
{
  XMC_VADC_GLOBAL_BackgroundDisableContinuousMode(VADC);
  XMC_VADC_GROUP_SetPowerMode(VADC_G0, XMC_VADC_GROUP_POWERMODE_OFF);
}

void ENTROPY_HARDWARE_Init(void)
{
  uint32_t idchip = 0;
  uint32_t adc_result = 0;

  uint32_t *g_chipid_ptr = (uint32_t *)&g_chipid[0];

  VADC_Init();

  for (int32_t i = 0; i < 4; ++i)
  {
    idchip ^= g_chipid_ptr[i];
  }


  for (int32_t i = 0; i < 16; ++i)
  {
	while (XMC_VADC_GLOBAL_BackgroundIsChannelPending(VADC, 0, 5));
    uint16_t result = XMC_VADC_GROUP_GetResult(VADC_G0, 5);
    adc_result = (adc_result << 4) | (result & 0xfU);
  }

  VADC_Deinit();

  srand48(idchip ^ adc_result);
}

