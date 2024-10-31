/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "example_wifi_user_reconnect.h"
#include "example_bdspeech.h"

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE cutils_test_cmd_table[] = {
	{
		(const u8 *)"wifi",  2, test_wifi, (const u8 *)"\t wifi <ssid psd>\n"
    "\t\t eg: wifi baidu 123\n"
	},
  {
		(const u8 *)"bdspeech",  0, test_bdspeech, (const u8 *)"\t bdspeech\n"
    "\t\t eg: bdspeech\n"
	},
  {
		(const u8 *)"start_asr",  0, test_start_asr_cmd, (const u8 *)"\t start_asr\n"
    "\t\t eg: start_asr\n"
	},
  {
		(const u8 *)"cpu_monitor",  0, test_cpu_monitor_cmd, (const u8 *)"\t cpu_monitor\n"
    "\t\t eg: cpu_monitor\n"
	},
  {
		(const u8 *)"socket",  2, test_socket_cmd, (const u8 *)"\t socket <ip port>\n"
    "\t\t eg: socket 192.168.1.3 5001\n"
	},
  {
		(const u8 *)"env",  0, test_env_cmd, (const u8 *)"\t env\n"
    "\t\t eg: env\n"
	},
};
