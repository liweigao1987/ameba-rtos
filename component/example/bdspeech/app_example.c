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
};
