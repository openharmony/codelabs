/* ----------------------------------------------------------------------------
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "los_config.h"

#if defined(LOSCFG_SHELL) && defined(LOSCFG_DRIVERS_HI3881)
#include "los_typedef.h"
#include "shell.h"
#include "shcmd.h"


unsigned int wal_hipriv_entry(const char *pc_buffer, unsigned int count);
unsigned int at_cmd_process(const char *at_cmd_line);

static void str_replace(char *s)
{
    while (*s != '\0') {
        if (*s == '{' || *s == '}') {
            *s = '\"';
        }
        s++;
    }
}

static int wifi_shell_cmd(int argc, char **argv)
{
    int i, ret;
    char cmd[256] = { 0 };
    int len = 0;
    static int init = 0;

    if (init == 0) {
        PRINTK("init at command\n");
        init = 1;
    }

    if (argc < 1) {
        PRINT_ERR("no arguments\n");
        return 0;
    }

    if (!strncmp(argv[0], "AT", 2) && (strlen(argv[0]) > 3)) {
        if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "%s", &argv[0][2]) == -1) {
            return 0;
        }
        str_replace(cmd);
        PRINTK("cmd:%s\n", cmd);
    } else {
        for (i = 0; i < argc; i++) {
            ret = snprintf_s(&cmd[len], sizeof(cmd) - len, sizeof(cmd) - len - 1, " %s", argv[i]);
            if (ret == -1) {
                return 0;
            }
            len += ret;
        }
        PRINTK("cmd:%s\n", cmd);
        wal_hipriv_entry(cmd, len + 1);
    }

    return 0;
}

SHELLCMD_ENTRY(hi3881_shellcmd, CMD_TYPE_EX, "hi3881", XARGS, (CmdCallBackFunc)wifi_shell_cmd);

#endif
