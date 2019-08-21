/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *
 * HND SiliconBackplane PMU support.
 *
 * $Id: hndpmu.h 456199 2014-02-18 03:06:41Z sudhirbs $
 */

#ifndef _hndpmu_h_
#define _hndpmu_h_

#if !defined(BCMDONGLEHOST)
#define SET_LDO_VOLTAGE_LDO1		1
#define SET_LDO_VOLTAGE_LDO2		2
#define SET_LDO_VOLTAGE_LDO3		3
#define SET_LDO_VOLTAGE_PAREF		4
#define SET_LDO_VOLTAGE_CLDO_PWM	5
#define SET_LDO_VOLTAGE_CLDO_BURST	6
#define SET_LDO_VOLTAGE_CBUCK_PWM	7
#define SET_LDO_VOLTAGE_CBUCK_BURST	8
#define SET_LDO_VOLTAGE_LNLDO1		9
#define SET_LDO_VOLTAGE_LNLDO2_SEL	10
#define SET_LNLDO_PWERUP_LATCH_CTRL	11

#define BBPLL_NDIV_FRAC_BITS		24
#define P1_DIV_SCALE_BITS			12

#define PMUREQTIMER (1 << 0)

extern void si_pmu_init(si_t *sih, osl_t *osh);
extern void si_pmu_chip_init(si_t *sih, osl_t *osh);
extern void si_pmu_pll_init(si_t *sih, osl_t *osh, uint32 xtalfreq);
extern bool si_pmu_is_autoresetphyclk_disabled(si_t *sih, osl_t *osh);
extern void si_pmu_res_init(si_t *sih, osl_t *osh);
extern void si_pmu_swreg_init(si_t *sih, osl_t *osh);
extern uint32 si_pmu_force_ilp(si_t *sih, osl_t *osh, bool force);
extern void si_pmu_res_minmax_update(si_t *sih, osl_t *osh);

extern uint32 si_pmu_si_clock(si_t *sih, osl_t *osh);   /* returns [Hz] units */
extern uint32 si_pmu_cpu_clock(si_t *sih, osl_t *osh);  /* returns [hz] units */
extern uint32 si_pmu_mem_clock(si_t *sih, osl_t *osh);  /* returns [Hz] units */
extern uint32 si_pmu_alp_clock(si_t *sih, osl_t *osh);  /* returns [Hz] units */
extern void si_pmu_ilp_clock_set(uint32 cycles);
extern uint32 si_pmu_ilp_clock(si_t *sih, osl_t *osh);  /* returns [Hz] units */

extern void si_pmu_set_switcher_voltage(si_t *sih, osl_t *osh, uint8 bb_voltage, uint8 rf_voltage);
extern void si_pmu_set_ldo_voltage(si_t *sih, osl_t *osh, uint8 ldo, uint8 voltage);
extern void si_pmu_paref_ldo_enable(si_t *sih, osl_t *osh, bool enable);
extern uint16 si_pmu_fast_pwrup_delay(si_t *sih, osl_t *osh);
extern uint si_pll_minresmask_reset(si_t *sih, osl_t *osh);
extern void si_pmu_rcal(si_t *sih, osl_t *osh);
extern void si_pmu_spuravoid(si_t *sih, osl_t *osh, uint8 spuravoid);
/* below function are only for BBPLL parallel purpose */
extern void si_pmu_spuravoid_isdone(si_t *sih, osl_t *osh, uint32 min_res_mask,
uint32 max_res_mask, uint32 clk_ctl_st, uint8 spuravoid);
extern void si_pmu_pll_off_PARR(si_t *sih, osl_t *osh, uint32 *min_res_mask,
uint32 *max_res_mask, uint32 *clk_ctl_st);
/* below function are only for BBPLL parallel purpose */
extern void si_pmu_gband_spurwar(si_t *sih, osl_t *osh);
extern uint32 si_pmu_cal_fvco(si_t *sih, osl_t *osh);

extern bool si_pmu_is_otp_powered(si_t *sih, osl_t *osh);
extern uint32 si_pmu_measure_alpclk(si_t *sih, osl_t *osh);

extern uint32 si_pmu_chipcontrol(si_t *sih, uint reg, uint32 mask, uint32 val);
extern uint32 si_pmu_regcontrol(si_t *sih, uint reg, uint32 mask, uint32 val);
extern uint32 si_pmu_pllcontrol(si_t *sih, uint reg, uint32 mask, uint32 val);
extern void si_pmu_pllupd(si_t *sih);
extern bool si_pmu_is_sprom_enabled(si_t *sih, osl_t *osh);
extern void si_pmu_sprom_enable(si_t *sih, osl_t *osh, bool enable);

extern void si_pmu_radio_enable(si_t *sih, bool enable);
extern uint32 si_pmu_waitforclk_on_backplane(si_t *sih, osl_t *osh, uint32 clk, uint32 delay);
extern void si_pmu_set_4330_plldivs(si_t *sih, uint8 dacrate);
extern void si_pmu_pllreset(si_t *sih);
extern uint32 si_pmu_get_bb_vcofreq(si_t *sih, osl_t *osh, int xtalfreq);
typedef void (*si_pmu_callback_t)(void* arg);

extern uint32 si_mac_clk(si_t *sih, osl_t *osh);
extern void si_pmu_switch_on_PARLDO(si_t *sih, osl_t *osh);
extern int si_pmu_fvco_pllreg(si_t *sih, uint32 *fvco, uint32 *pllreg);

#endif /* !defined(BCMDONGLEHOST) */

extern void si_pmu_otp_power(si_t *sih, osl_t *osh, bool on, uint32* min_res_mask);
extern void si_sdiod_drive_strength_init(si_t *sih, osl_t *osh, uint32 drivestrength);

extern void si_pmu_minresmask_htavail_set(si_t *sih, osl_t *osh, bool set_clear);
extern void si_pmu_slow_clk_reinit(si_t *sih, osl_t *osh);

#endif /* _hndpmu_h_ */
