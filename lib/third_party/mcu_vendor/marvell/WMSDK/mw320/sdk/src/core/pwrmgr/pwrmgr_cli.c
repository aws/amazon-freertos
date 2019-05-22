/** @file pwrmgr_cli.c
 *
 *  @brief This file provides  Power Manager CLI
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
 *
 *  MARVELL CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Material") are owned by Marvell International Ltd or its
 *  suppliers or licensors. Title to the Material remains with Marvell
 *  International Ltd or its suppliers and licensors. The Material contains
 *  trade secrets and proprietary and confidential information of Marvell or its
 *  suppliers and licensors. The Material is protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Material may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without Marvell's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by Marvell in writing.
 *
 */


#include <stdlib.h>

#include <wmstdio.h>
#include <cli.h>
#include <cli_utils.h>
#include <wmerrno.h>
#include <mdev_rtc.h>
#include <pwrmgr.h>
#include <rtc.h>
#include <wm_os.h>

static void cmd_pm_reboot(int argc, char *argv[])
{
	pm_reboot_soc();
}


static struct cli_command commands[] = {
	{"pm-reboot", NULL, cmd_pm_reboot},
};

int pm_cli_init(void)
{
	int i;

	for (i = 0; i < sizeof(commands) / sizeof(struct cli_command); i++)
		if (cli_register_command(&commands[i]))
			return 1;
	return 0;
}



static void print_cli_usage()
{
	wmprintf("Usage :-\r\n pm-mcu-state  power_state");
	wmprintf("  time-duration \r\n");
	wmprintf("power_state :\r\n2 -> PM2\r\n");
	wmprintf("3 -> PM3\r\n");
	wmprintf("4 -> PM4\r\n");
}

/*
   Command handler for power management
    */
static void cmd_pm_mcu_state(int argc, char *argv[])
{
	uint32_t time_dur = 0;
	uint32_t state = 0;
	if (argc < 2) {
		print_cli_usage();
		return;
	}
	state = atoi(argv[1]);
	if (state > PM4) {
		print_cli_usage();
		return;
	}
	if (argc == 3) {
		time_dur = atoi(argv[2]);
	}
	if (time_dur == 0) {
		wmprintf("No Timeout specified\r\n");
		wmprintf("Wakeup will occur by Wakeup key press\r\n ");
		wmprintf("PM0->PM%d \t", state);
	} else {
		wmprintf("Timeout specified :%d millisec\r\n", time_dur);
		wmprintf("The system can be brought out of standby by an RTC "
			 "timeout or a wakeup key press\r\n");
		wmprintf(" PM0->PM%d\t", state);
		wmprintf(" For %d millisec\n\r ", time_dur);
	}
	uint32_t wakeup_reason =  pm_mcu_state(state, time_dur);
	wmprintf("\n\r PM%d->PM0", state);
	if (wakeup_reason & ERTC) {
		wmprintf("\t (Wakeup by Timeout %d millisec)\n\r",
			 time_dur);
	} else {
		wmprintf("\t (Wakeup by GPIO toggle)\n\r");
	}
}
static void print_usage()
{
	wmprintf(" Usage : pm-mcu-cfg <enable> <mode> <threshold>\r\n");
	wmprintf(" <enabled> : true to enable \r\n"
		 "             false to disable \r\n");
	wmprintf(" <mode>    : MCU power mode\r\n"
		 "             2 : PM2\r\n"
		 "             3 : PM3\r\n");
	wmprintf(" <threshold> System will not go to power save state\r\n"
		 "             if it is going to wake up in less than\r\n");
	wmprintf("             [threshold] ticks.\r\n");
}

static void cmd_pm_mcu_cfg(int argc, char *argv[])
{
	if (argc < 4) {
		print_usage();
		return;
	}
	int mode = atoi(argv[2]);
	if (mode > PM4 || mode < PM0) {
		print_usage();
		return;
	}
	int enabled = atoi(argv[1]);
	if (enabled != true && enabled != false) {
		print_usage();
		return;
	}
	pm_mcu_cfg(atoi(argv[1]),  atoi(argv[2]), atoi(argv[3]));
}

static void print_io_cfg_usage()
{
	wmprintf(" Usage : pm-mcu-io-cfg <domain numbers separated by "
		 "space>\r\n");
	wmprintf("\t <domain  numbrs> : list of domain numbers separated "
		 "by space to be kept \r\n\t ON in PM2 \r\n");
	wmprintf("For example :\r\n");
	wmprintf("pm-mcu-io-cfg 0 2\r\n");
	wmprintf("Above CLI will keep IO domain 0 and 2 on in PM2 \r\n");
}


static void cmd_pm_mcu_io_cfg(int argc, char *argv[])
{
	if (argc < 2) {
		print_io_cfg_usage();
		return;
	}
	uint32_t domains_on = 0;
	uint32_t cnt, tmp;
	for (cnt = 1; cnt < argc; cnt++) {
		tmp = atoi(argv[cnt]);
		domains_on |= (1 << tmp) ;
	}
	pm_mcu_io_cfg(domains_on);
}



static struct cli_command mcu_commands[] = {
	{"pm-mcu-cfg", "<enable> <mode> <threshold>", cmd_pm_mcu_cfg},
	{"pm-mcu-io-cfg", "<domain numbers separated by space>",
		 cmd_pm_mcu_io_cfg},
	{"pm-mcu-state", "<state> [timeout in milliseconds]",
	 cmd_pm_mcu_state},
};

int pm_mcu_cli_init()
{
	int i;

	for (i = 0; i < sizeof(mcu_commands) / sizeof(struct cli_command); i++)
		if (cli_register_command(&mcu_commands[i]))
			return -WM_FAIL;
	return WM_SUCCESS;
}


