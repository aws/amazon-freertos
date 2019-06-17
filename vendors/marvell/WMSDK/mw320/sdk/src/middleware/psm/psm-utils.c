/** @file psm-utils.c
*
*  @brief PSM Utils
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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

#include <ctype.h>
#include <wm_os.h>
#include <cli.h>
#include <wmstdio.h>
#include <psm-v2.h>
#include <psm-utils.h>
#include <wmlist.h>
#include <partition.h>

#include <wmlog.h>
#define psmu_e(...)				\
	wmlog_e("psmu", ##__VA_ARGS__)
#define psmu_w(...)				\
	wmlog_w("psmu", ##__VA_ARGS__)
#ifdef CONFIG_PSM_UTIL_DEBUG
#define psmu_d(...)				\
	wmlog("psmu", ##__VA_ARGS__)
#else
#define psmu_d(...)
#endif /* ! CONFIG_PSM_UTIL_DEBUG */

static int dumped_object_count;
static psm_hnd_t psm_hnd;

#define READ_BUFFER_SIZE 80

static int dump_full_variable(psm_hnd_t psm_hnd, const uint8_t *name)
{
	/*
	 * This is invoked from the cli thread context and so we can take
	 * the risk of bigger stack size.
	 */
	uint8_t value[READ_BUFFER_SIZE];

	psm_object_handle_t ohandle;
	int obj_size = psm_object_open(psm_hnd, (char *)name, PSM_MODE_READ,
				       0 /* don't care */, NULL, &ohandle);
	if (obj_size < 0) {
		psmu_w("<invalid length of data: %d>", obj_size);
		return -WM_FAIL;
	}

	/* Print prefix */
	if (is_psm_encrypted(psm_hnd))
		wmprintf("(enc) ");

	/* Print the name */
	wmprintf("%s = ", name);

	int remaining = obj_size;
	bool first_iteration_done = false, binary_data = false;
	while (remaining) {
		int read_size = remaining <= READ_BUFFER_SIZE ? remaining
			: READ_BUFFER_SIZE;

		if (read_size) {
			int rv = psm_object_read(ohandle, value, read_size);
			if (rv != read_size) {
				psmu_w("Could not read req. bytes: %d", rv);
				psm_object_close(&ohandle);
				return -WM_FAIL;
			}
		}

		int index;
		/*
		 * This loop will run only once and determine if the data
		 * is binary or printable ascii. This result of this loop
		 * will decide how the data will be dumped to console. The
		 * caveat is that we determine the format by checking only
		 * first 'READ_BUFFER_SIZE' bytes. We could do a complete
		 * scan of the data to determine but that would involve the
		 * entire variable flash read twice.
		 */
		if (!first_iteration_done) {
			for (index = 0; index < read_size; index++) {
				if (!isprint(value[index]) &&
				    (value[index] != 0x0A) &&
				    (value[index] != 0x0D)) {
					wmprintf("(hex) ");
					binary_data = true;
					break;
				}
			}
			first_iteration_done = true;
		}

		for (index = 0; index < read_size;) {
			if (binary_data) {
				wmprintf("%x", value[index++]);
				if (!(index % 16))
					wmprintf("\r\n");
			} else {
				wmprintf("%c", value[index]);
				if (value[index] == 0x0A)
					wmprintf("%c", 0x0D);
				index++;
			}
		}

		remaining -= read_size;
	}

	wmprintf(" (%d bytes)\r\n", obj_size);

	psm_object_close(&ohandle);
	return WM_SUCCESS;
}

static int psm_util_dump_cb(const uint8_t *name)
{
	uint8_t value[READ_BUFFER_SIZE];


	psm_object_handle_t ohandle;
	int obj_size = psm_object_open(psm_hnd, (char *)name, PSM_MODE_READ,
				       sizeof(value) - 1, NULL, &ohandle);
	if (obj_size < 0) {
		psmu_w("<invalid length of data: %d>\r\n", obj_size);
		return 1;
	}

	int read_size = obj_size <= READ_BUFFER_SIZE ? obj_size
		: READ_BUFFER_SIZE;

	if (read_size) {
		int rv = psm_object_read(ohandle, value, read_size);
		if (rv != read_size) {
			psmu_w("Could not read requested bytes: %d>\r\n", rv);
			psm_object_close(&ohandle);
			return 1;
		}
	}

	/* Print prefix */
	if (is_psm_encrypted(psm_hnd))
			wmprintf("(enc) ");

	/* Print the name */
	wmprintf("%s = ", name);

	bool binary_data = false;
	int index;
	for (index = 0; index < read_size; index++) {
		if (!isprint(value[index])) {
			wmprintf("(hex) ");
			binary_data = true;
			break;
		}
	}

	for (index = 0; index < read_size;) {
		if (binary_data) {
			wmprintf("%x", value[index++]);
			if (!(index % 8) && (index != read_size))
				wmprintf("\r\n");
		} else {
			wmprintf("%c", value[index++]);
		}
	}

	if (read_size < obj_size)
		wmprintf("..(contd) (%d bytes)\r\n", obj_size);
	else
		wmprintf(" (%d bytes)\r\n", obj_size);

	dumped_object_count++;

	psm_object_close(&ohandle);
	return WM_SUCCESS;
}

void psm_util_dump(psm_hnd_t hnd)
{
	if (!hnd)
		return;

	if (is_psm_encrypted(hnd))
		wmprintf("Note: Entries prepended with \"(enc)\" "
			 "are encrypted\n\r");

	psm_hnd = hnd;
	dumped_object_count = 0;
	psm_objects_list(hnd, psm_util_dump_cb);

	if (!dumped_object_count)
		wmprintf("No objects found\r\n");
	else
		wmprintf("\r\n%d total objects.\r\n", dumped_object_count);
}


typedef struct {
	const char *part_name;
	psm_hnd_t hnd;
	struct list_head node;
} psm_cli_handle_node_t;

static struct list_head psm_cli_list;

static psm_hnd_t psm_cli_get_handle_from_partname(const char *part_name)
{
	psm_cli_handle_node_t *hnode;
	list_for_each_entry_type(hnode, psm_cli_handle_node_t, &psm_cli_list,
			node, struct list_head) {
		if (!strcmp(part_name, hnode->part_name))
			return hnode->hnd;
	}

	return NULL;
}

/* Extracts the "-p" and its parameter and modifies the argument list */
static psm_hnd_t psm_cli_get_handle(int *p_argc, char **argv, bool allow_empty)
{
	int argc = *p_argc;

	const char *part_name = NULL;
	int idx;
	for (idx = 1; idx < argc; idx++) {
		if (!strcmp(argv[idx], "-p")) {
			if ((idx + 1) >= argc)
				return NULL;
			part_name = argv[idx + 1];

			/* Shift the remaining parameters to left */
			memmove(&argv[idx], &argv[idx + 2],
				argc - (idx + 2));
			*p_argc -= 2;
		}
	}

	if (!part_name) {
		if (allow_empty)
			part_name = ""; /* If no part_name use empty */
		else
			return NULL;
	}

	return psm_cli_get_handle_from_partname(part_name);
}

/** The maximum size of the fully qualified variable name including the
 * trailing \\0. Fully qualified variable name is:
 * \<product_name\>.\<module_name\>.\<variable_name\>
 */
#define FULL_VAR_NAME_SIZE 64

static void psm_generate_full_name(int argc, char **argv, char *full_name,
				   int full_name_len)
{
	if (argc == 3)
		snprintf(full_name, full_name_len, "%s.%s", argv[1], argv[2]);
	else if (argc == 2)
		snprintf(full_name, full_name_len, "%s", argv[1]);
	else
		snprintf(full_name, full_name_len, "unknown");
}

static void psm_cli_get(int argc, char **argv)
{
	psm_hnd_t hnd = psm_cli_get_handle(&argc, argv, true);
	if (!hnd) {
		wmprintf("Invalid arguments\r\n");
		return;
	}

	char full_name[FULL_VAR_NAME_SIZE];

	if (argc != 2 && argc != 3) {
		wmprintf("[psm] Usage: %s <variable>\r\n", argv[0]);
		wmprintf("[psm] Error: invalid number of arguments\r\n");
		return;
	}

	psm_generate_full_name(argc, argv, full_name, sizeof(full_name));
	dump_full_variable(hnd, (uint8_t *)full_name);
}

void psm_cli_delete(int argc, char **argv)
{
	psm_hnd_t hnd = psm_cli_get_handle(&argc, argv, true);
	if (!hnd) {
		wmprintf("Invalid arguments\r\n");
		return;
	}

	int rv;
	char full_name[FULL_VAR_NAME_SIZE];

	if (argc != 2 && argc != 3) {
		wmprintf("[psm] Usage: %s <variable>\r\n", argv[0]);
		wmprintf("[psm] Error: invalid number of arguments\r\n");
		return;
	}

	psm_generate_full_name(argc, argv, full_name, sizeof(full_name));

	rv = psm_object_delete(hnd, full_name);
	if (rv != 0) {
		wmprintf("Variable not found (%d)\r\n", rv);
		return;
	}
}

void psm_cli_set(int argc, char **argv)
{
	psm_hnd_t hnd = psm_cli_get_handle(&argc, argv, true);
	if (!hnd) {
		wmprintf("Invalid arguments\r\n");
		return;
	}

	int rv;
	char full_name[FULL_VAR_NAME_SIZE];

	if (argc != 3 && argc != 4) {
		wmprintf("[psm] Usage: %s <variable> <value>\r\n", argv[0]);
		wmprintf("[psm] Error: invalid number of arguments\r\n");
		return;
	}

	psm_generate_full_name(argc - 1, argv, full_name, sizeof(full_name));

	char *value = NULL;
	if (argc == 4)
		value = argv[3];
	else if (argc == 3)
		value = argv[2];

	rv = psm_set_variable_str(hnd, full_name, value);
	if (rv != 0) {
		wmprintf("psm_set_variable() failed with: %d\r\n", rv);
		return;
	}
}

void psm_erase(int argc, char **argv)
{
	psm_hnd_t hnd = psm_cli_get_handle(&argc, argv, true);
	if (!hnd) {
		wmprintf("Invalid arguments\r\n");
		return;
	}

	int rv = psm_format(hnd);
	if (rv != WM_SUCCESS) {
		wmprintf("Could not erase\r\n");
		return;
	}
}

void psm_dump(int argc, char **argv)
{
	psm_hnd_t hnd = psm_cli_get_handle(&argc, argv, false);
	if (hnd) {
		psm_util_dump(hnd);
		return;
	}

	/* Dump all */
	int part_cnt = 0;
	psm_cli_handle_node_t *hnode;
	list_for_each_entry_type(hnode, psm_cli_handle_node_t, &psm_cli_list,
			node, struct list_head) {
		wmprintf("--------\r\n");
		wmprintf("Partition: %s\r\n", hnode->part_name[0] ?
			 hnode->part_name : "<null>");
		wmprintf("--------\r\n");
		psm_util_dump(hnode->hnd);

		part_cnt++;
	}

	wmprintf("Dumped total %d partition(s)\r\n", part_cnt);
}

struct cli_command psm_commands[] = {
	{"psm-get", "-p [optional partition name] <variable>", psm_cli_get},
	{"psm-set", "-p [optional partition name] <variable> <value>",
	 psm_cli_set},
	{"psm-delete", "-p [optional partition name] <variable>",
	 psm_cli_delete},
	{"psm-erase", "-p [optional partition name]", psm_erase},
	{"psm-dump", "-p [optional partition name]", psm_dump},
};

static int psm_cli_check_params(psm_hnd_t hnd, const char *part_name)
{
	if (!hnd)
		return -WM_E_INVAL;

	psm_cli_handle_node_t *hnode;
	list_for_each_entry_type(hnode, psm_cli_handle_node_t, &psm_cli_list,
			node, struct list_head) {
		if (!strcmp(part_name, hnode->part_name))
			return -WM_E_EXIST;

		if (hnd == hnode->hnd)
			return -WM_E_EXIST;
	}

	return WM_SUCCESS;
}

int psm_cli_init(psm_hnd_t hnd, const char *part_name)
{
	if (!psm_cli_list.next)
		INIT_LIST_HEAD(&psm_cli_list);

	char *part_name_tmp;
	if (part_name)
		part_name_tmp = strdup(part_name);
	else
		part_name_tmp = strdup(""); /* Init to empty string */

	if (!part_name_tmp)
			return -WM_E_NOMEM;

	int rv = psm_cli_check_params(hnd, part_name_tmp);
	if (rv != WM_SUCCESS) {
		os_mem_free(part_name_tmp);
		return rv;
	}

	psm_cli_handle_node_t *hnode =
		os_mem_calloc(sizeof(psm_cli_handle_node_t));
	if (!hnode) {
		os_mem_free(part_name_tmp);
		return -WM_E_NOMEM;
	}

	hnode->part_name = part_name_tmp;
	hnode->hnd = hnd;
	INIT_LIST_HEAD(&hnode->node);
	list_add_tail(&hnode->node, &psm_cli_list);

	static bool common_cli_inited;
	if (common_cli_inited)
		return WM_SUCCESS;

	int i;
	for (i = 0; i < sizeof(psm_commands) / sizeof(struct cli_command); i++)
		if (cli_register_command(&psm_commands[i]))
			return -WM_FAIL;

	common_cli_inited = true;
	return WM_SUCCESS;
}

#ifdef CONFIG_APP_FRM_CLI_HISTORY
static int _cli_name_val_get(const char *name, char *value, int max_len)
{
	return psm_get_variable_str(sys_psm_get_handle(), name, value, max_len);
}

static int _cli_name_val_set(const char *name, const char *value)
{
	return psm_set_variable_str(sys_psm_get_handle(), name, value);
}
#endif

/*
 * A 'global' PSM handle to be exposed to outside world through a getter API
 */
static psm_hnd_t sys_psm_hnd;

psm_hnd_t sys_psm_get_handle()
{
	return sys_psm_hnd;
}

int sys_psm_init()
{
	if (sys_psm_hnd)
		return WM_SUCCESS;

	int rv;
	flash_desc_t fl;
	rv = part_get_desc_from_id(FC_COMP_PSM, &fl);
	if (rv != WM_SUCCESS) {
		psmu_d("Unable to get fdesc from id");
		return rv;
	}

#ifdef CONFIG_SECURE_PSM
	psm_cfg_t psm_cfg;
	memset(&psm_cfg, 0, sizeof(psm_cfg_t));
	psm_cfg.secure = true;
	rv = psm_module_init(&fl, &sys_psm_hnd, &psm_cfg);
#else
	rv = psm_module_init(&fl, &sys_psm_hnd, NULL);
#endif /* CONFIG_SECURE_PSM */
	if (rv) {
		psmu_e("failed to init PSM status code %d", rv);
		return rv;
	}

#ifdef CONFIG_APP_FRM_CLI_HISTORY
	cli_add_history_hook(_cli_name_val_get, _cli_name_val_set);
#endif

	return WM_SUCCESS;
}

int sys_psm_init_ex(const psm_init_part_t *sys_init_part)
{
	int rv;

	if (sys_psm_hnd)
		return WM_SUCCESS;

	if (sys_init_part == NULL) {
		return -WM_E_INVAL;
	}

	rv = psm_module_init_ex(sys_init_part, &sys_psm_hnd);
	if (rv) {
		psmu_e("failed to init PSM status code %d", rv);
		return rv;
	}

#ifdef CONFIG_APP_FRM_CLI_HISTORY
	cli_add_history_hook(_cli_name_val_get, _cli_name_val_set);
#endif

	return WM_SUCCESS;
}
