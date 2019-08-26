/** @file rfget.c
 *
 *  @brief  This file provides the rfget API
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


/* rfget.c: The rfget API
 */

#include <string.h>

//#include <cli.h>
//#include <cli_utils.h>
#include <wmstdio.h>
//#include <wm_net.h>
#include <rfget.h>
#include <flash.h>
#include <crc32.h>
//#include <httpc.h>
//#include <httpd.h>
#include <ftfs.h>
#include <partition.h>
#include <boot_flags.h>

#include "firmware.h"

static bool update_in_progress;

#define MAX_DOWNLOAD_DATA 32

static char rfget_init_done;
#if 0
static void signal_update_start()
{
	update_in_progress = true;
}

static void signal_update_end()
{
	update_in_progress = false;
}
#endif //if 0

bool rfget_is_update_in_progress()
{
	return update_in_progress;
}
#if 0
static void _dump_resource(const char *url_str)
{
	int status, i, dsize;
	http_session_t handle;
	char buf[MAX_DOWNLOAD_DATA];

	status = httpc_get(url_str, &handle, NULL, NULL);
	if (status != 0) {
		rf_e("Unable to print resource");
		return;
	}

	rf_d("Resource dump begins");
	while (1) {
		dsize = http_read_content(handle, buf, MAX_DOWNLOAD_DATA);
		if (dsize < 0) {
			rf_e("Unable to read data on http session: %d",
				      dsize);
			break;
		}
		if (dsize == 0) {
			rf_d("********* All data read **********");
			break;
		}

		for (i = 0; i < dsize; i++) {
			if (buf[i] == '\r')
				continue;
			if (buf[i] == '\n') {
				wmprintf("\n\r");
				continue;
			}
			wmprintf("%c", buf[i]);
		}
	}

	http_close_session(&handle);
}

/**
 * This is called by the firmware/fs update function, to get next blocks of
 * data of a update file.
 * 
 * @param[in] priv Same as that was passed to the caller.
 * @param[in,out] buf Pointer to allocated buffer. Will be filled by callee
 * with the update data.
 * @param[in] max_len Maximum length of data to to be filled in buf
 *
 * @return On success returns the size of data returned in buf. On error,
 * returns -WM_FAIL.
 */
static size_t server_mode_data_fetch_cb(void *priv, void *buf, size_t max_len)
{
	int sockfd = (int)priv;
	rf_d("Server callback: Trying to read %d bytes from stream",
		max_len);
	int size_read = httpd_recv(sockfd, buf, max_len, 0);
	if (size_read < 0) {
		rf_d("Server mode: Could not read data from stream");
		return -WM_FAIL;
	}

	return size_read;
}

/**
 * This is called by the firmware/fs update function, to get next blocks of
 * data of a update file.
 * 
 * @param[in] priv Same as that was passed to the caller.
 * @param[in,out] buf Pointer to allocated buffer. Will be filled by callee
 * with the update data.
 * @param[in] max_len Maximum length of data to to be filled in buf
 *
 * @return On success returns the size of data returned in buf. If the size
 * returned is zero the caller should assume that all data is read. On error,
 * returns -WM_FAIL.
 */
static size_t client_mode_data_fetch_cb(void *priv, void *buf, size_t max_len)
{
	http_session_t handle = (http_session_t) priv;
	rf_d("Client callback: Trying to read %d bytes from stream",
		max_len);
	return http_read_content(handle, buf, max_len);
}

static int _update_system_image(data_fetch data_fetch_cb,
				void *priv, size_t firmware_length,
				struct partition_entry *passive)
{
	int status = -WM_FAIL;
	signal_update_start();

	if (passive->type == FC_COMP_FTFS) {
		status = write_ftfs_image(data_fetch_cb, priv, firmware_length,
					  passive);
		if (status != WM_SUCCESS) {
			rf_e("FS update failed");
		} else {
			rf_d("FS update succeeded");
		}
	} else if (passive->type == FC_COMP_FW) {
		status = update_firmware(data_fetch_cb, priv, firmware_length,
					 passive);
		if (status != WM_SUCCESS) {
			rf_e("Firmware update failed");
		} else {
			rf_d("Firmware update succeeded");
		}
#ifdef CONFIG_WIFI_FW_UPGRADE
	} else if (passive->type == FC_COMP_WLAN_FW) {
		status = update_wifi_firmware(data_fetch_cb, priv,
			firmware_length, passive);
		if (status != WM_SUCCESS) {
			rf_e("WiFi Firmware update failed");
		} else {
			rf_d("WiFi Firmware update succeeded");
		}
#endif
#ifdef CONFIG_BT_FW_UPGRADE
	} else if (passive->type == FC_COMP_BT_FW) {
		status = update_and_validate_bt_firmware(data_fetch_cb, priv,
			firmware_length, passive, NULL);
		if (status != WM_SUCCESS) {
			rf_e("BT Firmware update failed");
		} else {
			rf_d("BT Firmware update succeeded");
		}
#endif
	}

	signal_update_end();
	return status;
}

/*
 * We have two seperate functions for client mode and server mode update
 * because in server mode the socket is already opened and we just have to
 * read 'filelength' number of bytes from the stream. In client mode we
 * have to handle HTTP protcol with help from HTTP client library.
 */
int rfget_server_mode_update(int sockfd, size_t filelength,
			     struct partition_entry *passive)
{
	int status;

	if (passive->type == FC_COMP_FTFS) {
		rf_d("Server mode update filesystem");
	} else if (passive->type == FC_COMP_FW) {
		rf_d("Server mode update firmware");
#ifdef CONFIG_WIFI_FW_UPGRADE
	} else if (passive->type == FC_COMP_WLAN_FW) {
		rf_d("Server mode update wifi firmware");
#endif
#ifdef CONFIG_BT_FW_UPGRADE
	} else if (passive->type == FC_COMP_BT_FW) {
		rf_d("Server mode update BT firmware");
#endif
	} else {
		rf_e("%s: Unknown type: %d", __func__, passive->type);
		return -WM_FAIL;
	}
	rfget_init();
	status = _update_system_image(server_mode_data_fetch_cb,
				      (void *)sockfd, filelength,
		passive);

	if (status != WM_SUCCESS)
		return -WM_E_HTTPD_HANDLER_400;
	else
		return status;
}

int rfget_client_mode_update(const char *url_str,
			     struct partition_entry *passive,
			     const httpc_cfg_t *cfg)
{
	int status;
	http_session_t handle;
	http_resp_t *resp = NULL;

	if (!passive)
		return -WM_FAIL;

	rfget_init();

#ifdef CONFIG_ENABLE_HTTPC_SECURE
	httpc_cfg_t httpc_cfg;
	if (!cfg) {
		/* Set TLS receive buffer size to 16KB (max as per TLS spec). If
		 * not set to max, firmware upgrade fails for big file sizes */
		rf_d("Setting TLS rx buffer size to 16K");

		memset(&httpc_cfg, 0x00, sizeof(httpc_cfg_t));
		httpc_cfg.retry_cnt = DEFAULT_RETRY_COUNT;
		httpc_cfg.in_buf_len = 1024 * 16;
		cfg = &httpc_cfg;
	} else if (cfg->in_buf_len != (1024 * 16)) {
		/* Force TLS receive buffer size to 16KB (max as per TLS spec).
		 * If not set to max, firmware upgrade fails for big file sizes
		 */
		rf_d("Forcing TLS rx buffer size to 16K");

		memcpy(&httpc_cfg, cfg, sizeof(httpc_cfg_t));
		httpc_cfg.in_buf_len = 1024 * 16;
		cfg = &httpc_cfg;
	}
#endif /* CONFIG_ENABLE_HTTPC_SECURE */

	status = httpc_get(url_str, &handle, &resp, cfg);
	if (status != 0) {
		rf_e("Unable to connect to server");
		return status;
	}
	if (resp->status_code != 200) {
		rf_e("HTTP Error %d", resp->status_code);
		status = -WM_FAIL;
		goto skip_update;
	}

	if (resp->chunked) {
		rf_e("HTTP chunked fs update is not supported");
		goto skip_update;
	}

	if (passive->type == FC_COMP_FTFS) {
		rf_d("Client mode update filesystem: %s", url_str);
	} else if (passive->type == FC_COMP_FW) {
		rf_d("Client mode update firmware: %s", url_str);
#ifdef CONFIG_WIFI_FW_UPGRADE
	} else if (passive->type == FC_COMP_WLAN_FW) {
		rf_d("Client mode update wifi firmware: %s", url_str);
#endif
#ifdef CONFIG_BT_FW_UPGRADE
	} else if (passive->type == FC_COMP_BT_FW) {
		rf_d("Client mode update BT firmware: %s", url_str);
#endif
	} else {
		rf_e("%s: Unknown type: %d", __func__, passive->type);
		return -WM_FAIL;
	}
	status = _update_system_image(client_mode_data_fetch_cb,
				      (void *)handle, resp->content_length,
				      passive);

skip_update:
	http_close_session(&handle);
	return status;
}
#endif // if 0
struct partition_entry *rfget_get_passive_ftfs(void)
{
	short history = 0;
	struct partition_entry *f1, *f2;

	f1 = part_get_layout_by_id(FC_COMP_FTFS, &history);
	f2 = part_get_layout_by_id(FC_COMP_FTFS, &history);

	if (f1 == NULL || f2 == NULL) {
		rf_e("Unable to retrieve flash layout");
		return NULL;
	}

	return f1->gen_level >= f2->gen_level ? f2 : f1;
}

struct partition_entry *rfget_get_passive_wifi_firmware(void)
{
	short history = 0;
	struct partition_entry *f1, *f2;

	f1 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);
	f2 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);

	if (f1 == NULL || f2 == NULL) {
		rf_e("Unable to retrieve flash layout");
		return NULL;
	}

	return f1->gen_level >= f2->gen_level ? f2 : f1;
}

struct partition_entry *rfget_get_passive_bt_firmware(void)
{
	short history = 0;
	struct partition_entry *f1, *f2;

	f1 = part_get_layout_by_id(FC_COMP_BT_FW, &history);
	f2 = part_get_layout_by_id(FC_COMP_BT_FW, &history);

	if (f1 == NULL || f2 == NULL) {
		rf_e("Unable to retrieve flash layout");
		return NULL;
	}

	return f1->gen_level >= f2->gen_level ? f2 : f1;
}

struct partition_entry *rfget_get_passive_firmware(void)
{
	short history = 0;
	struct partition_entry *f1, *f2;

	f1 = part_get_layout_by_id(FC_COMP_FW, &history);
	f2 = part_get_layout_by_id(FC_COMP_FW, &history);

	if (f1 == NULL || f2 == NULL) {
		rf_e("Unable to retrieve flash layout");
		return NULL;
	}

	if (boot_get_partition_no() == 0) {
		/* If this is 0, it means the first entry is the booted
		 * firmware. Which means we want to write to the second entry
		 * and make it the active partition hence forth
		 */
		return f2;
	} else {
		return f1;
	}
}
# if 0
static void cmd_updatefw(int argc, char **argv)
{
	const char *url_str = argv[1];
	struct partition_entry *p = rfget_get_passive_firmware();

	if (argc != 2) {
		wmprintf("Usage: %s <http-url>\r\n", argv[0]);
		wmprintf("Invalid number of arguments.\r\n");
		return;
	}

	if (rfget_client_mode_update(url_str, p, NULL) == WM_SUCCESS) {
		wmprintf("[rfget] Firmware update succeeded\r\n");
	} else {
		wmprintf("[rfget] Firmware update failed\r\n");
	}
}

#ifdef CONFIG_WIFI_FW_UPGRADE
static void cmd_update_wififw(int argc, char **argv)
{
	const char *url_str = argv[1];
	struct partition_entry *p = rfget_get_passive_wifi_firmware();

	if (argc != 2) {
		wmprintf("Usage: %s <http-url>\r\n", argv[0]);
		wmprintf("Invalid number of arguments.\r\n");
		return;
	}

	if (rfget_client_mode_update(url_str, p, NULL) == WM_SUCCESS) {
		wmprintf("[rfget] WiFi Firmware update succeeded\r\n");
	} else {
		wmprintf("[rfget] WiFi Firmware update failed\r\n");
	}
}
#endif

static void cmd_updatefs(int argc, char **argv)
{
	const char *url_str = argv[1];
	struct partition_entry *p = rfget_get_passive_ftfs();

	if (argc != 3) {
		wmprintf("Usage: %s <http-url>\r\n", argv[0]);
		wmprintf("Invalid number of arguments.\r\n");
		return;
	}

	if (rfget_client_mode_update(url_str, p, NULL) == WM_SUCCESS) {
		wmprintf("[rfget] FTFS update succeeded\r\n");
	} else {
		wmprintf("[rfget] FTFS update failed\r\n");
	}
}

static void cmd_rfprint(int argc, char **argv)
{
	const char *url_str = argv[1];

	if (argc != 2) {
		wmprintf("Usage: %s <http-url>\r\n", argv[0]);
		wmprintf("Invalid number of arguments.\r\n");
		return;
	}

	_dump_resource(url_str);
	return;
}
static struct cli_command rfget_commands[] = {
	{"rfprint", "<http_url>", cmd_rfprint},
	{"updatefw", "<http_url>", cmd_updatefw},
#ifdef CONFIG_WIFI_FW_UPGRADE
	{"updatewififw", "<http_url>", cmd_update_wififw},
#endif
	{"updatefs", "<fs-name> <http_url>", cmd_updatefs},
};

int rfget_cli_init(void)
{
	return cli_register_commands(rfget_commands,
				     sizeof(rfget_commands) /
				     sizeof(struct cli_command));
}
#endif // if 0
int rfget_init(void)
{
	if (rfget_init_done)
		return WM_SUCCESS;

	crc32_init();
	/* Initialise internal/external flash memory */
	flash_drv_init();

	rfget_init_done = 1;

	return WM_SUCCESS;
}

struct fs *rfget_ftfs_init(struct ftfs_super *sb, int be_ver, const char *name,
			   struct partition_entry **passive)
{
	struct partition_entry *f1, *f2, *active = NULL;
	FT_HEADER sec1, sec2;
	mdev_t *fl_dev;
	short history = 0;
	flash_desc_t fd;

	rfget_init();

	int ret = part_init();
	if (ret != WM_SUCCESS) {
		rf_e("rfget_ftfs_init: could not read partition table");
		return NULL;
	}
	*passive = NULL;
	f1 = part_get_layout_by_name(name, &history);
	f2 = part_get_layout_by_name(name, &history);

	if (f1 == NULL) {
		/* There are no FTFS partitions defined */
		return NULL;
	}

	/* Processing for partition 1 */
	fl_dev = flash_drv_open(f1->device);
	if (fl_dev == NULL) {
		ftfs_e("Flash driver init is required"
			"before open");
		return NULL;
	}

	/*   Read the FTFS Header */
	if (ft_read_header(&sec1, f1->start, fl_dev) != WM_SUCCESS)
		return NULL;
	ftfs_l("part1:  be_ver: %d", sec1.backend_version);
	/*   Validate be version */
	if (ft_is_valid_magic(sec1.magic) && sec1.backend_version == be_ver) {
		active = f1;
	}

	flash_drv_close(fl_dev);

	if (f2) {
		/* Processing for partition 2 */
		fl_dev = flash_drv_open(f2->device);
		if (fl_dev == NULL) {
			ftfs_e("Flash driver init is required"
				 "before open\r\n");
			return NULL;
		}

		/*   Read the FTFS Header */
		if (ft_read_header(&sec2, f2->start, fl_dev) != WM_SUCCESS)
			return NULL;
		ftfs_l("part2:  be_ver: %d", sec2.backend_version);
		/*   Validate be version */
		if (ft_is_valid_magic(sec2.magic) && sec2.backend_version ==
		    be_ver) {
			if (active) {
				/* If f1 is also valid then pick and choose */
				struct partition_entry *p =
					part_get_active_partition(f1, f2);
				/* Given 2 partitions, one of them has to be
				 * active */
				active = p;
			} else
				active = f2;
		}
		flash_drv_close(fl_dev);

		if (active == f1)
			*passive = f2;
		else
			*passive = f1;
	}

	if (active) {
		ftfs_l("be_ver = %d active = %x", be_ver, active->start);
		part_to_flash_desc(active, &fd);
		return ftfs_init(sb, &fd);
	} else {
		return NULL;
	}
}

void purge_stream_bytes(data_fetch data_fetch_cb, void *priv,
			size_t length, unsigned char *scratch_buf,
			const int scratch_buflen)
{
	int size_read, to_read;
	while (length > 0) {
		to_read = length > scratch_buflen ? scratch_buflen : length;
		size_read = data_fetch_cb(priv, scratch_buf, to_read);
		if (size_read <= 0) {
			rf_e("Unexpected size returned");
			break;
		}
		length -= size_read;
	}
}

int rfget_update_complete(update_desc_t *ud)
{
	flash_drv_close(ud->dev);
	return part_set_active_partition(ud->pe);
}

int rfget_update_abort(update_desc_t *ud)
{
	flash_drv_close(ud->dev);
	return WM_SUCCESS;
}

int rfget_update_data(update_desc_t *ud, const char *data, size_t datalen)
{
	if (datalen > ud->remaining) {
		rf_e("Update data greater than partition len");
		return -WM_E_RFGET_INVLEN;
	}

	/* Write the data */
	if (flash_drv_write(ud->dev, (void *)data, datalen,
			    ud->addr) != 0) {
		rf_e("Failed to write update data");
		return -WM_E_RFGET_FWR;
	}

	ud->addr += datalen;
	ud->remaining -= datalen;
	return WM_SUCCESS;
}

int rfget_update_begin(update_desc_t *ud, struct partition_entry *p)
{
	ud->pe = p;
	ud->addr = p->start;
	ud->remaining = p->size;
	ud->dev = flash_drv_open(p->device);
	if (ud->dev == NULL) {
		rf_e("Flash driver init is required before open");
		return -WM_E_RFGET_FUPDATE;
	}

	if (flash_drv_erase(ud->dev, ud->addr, ud->remaining) < 0) {
		flash_drv_close(ud->dev);
		rf_e("Failed to erase partition");
		return -WM_E_RFGET_FWR;
	}

	return WM_SUCCESS;
}

