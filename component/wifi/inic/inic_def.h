/**
  ******************************************************************************
  * @file    inic_def.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef INIC_DEF_H
#define INIC_DEF_H

#define BASIC_API_BASE						0x0000
#define EXT_API_BASE						0x1000
#define INTER_API_BASE						0x2000
#define MISC_API_BASE						0x3000
#define NAN_API_BASE						0x4000
#define P2P_API_BASE						0x5000
#define PROXY_API_BASE						0x6000
#define SDIO_BRIDGE_BASE					0x7000
#define WTN_API_BASE						0x8000

#define INIC_API_PROCESS_DONE				0

enum INIC_WIFI_C2H_API_ID {
	/* 0x0001~0x1000 reserved for basic API */
	INIC_API_SCAN_USER_CALLBACK					= BASIC_API_BASE + 1,
	INIC_API_SCAN_EACH_REPORT_USER_CALLBACK		= BASIC_API_BASE + 2,

	INIC_API_AP_CH_SWITCH						= BASIC_API_BASE + 5,
	INIC_API_HDL								= BASIC_API_BASE + 6,
	INIC_API_PROMISC_CALLBACK					= BASIC_API_BASE + 7,
	INIC_API_GET_LWIP_INFO						= BASIC_API_BASE + 8,
	INIC_API_SET_NETIF_INFO						= BASIC_API_BASE + 9,
	INIC_API_CFG80211_SCAN_REPORT				= BASIC_API_BASE + 10,
	INIC_API_IP_TABLE_CHK						= BASIC_API_BASE + 11,
	INIC_API_IP_ACS								= BASIC_API_BASE + 12,

	/* 0x4001~0x5000 reserved for nan API */
	INIC_API_CFG80211_NAN_REPORT_MATCH_EVENT		= NAN_API_BASE + 1,
	INIC_API_CFG80211_NAN_DEL_FUNC					= NAN_API_BASE + 2,
	INIC_API_CFG80211_NAN_CFGVENDOR_EVENT			= NAN_API_BASE + 3,
	INIC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY		= NAN_API_BASE + 4,

	/* 0x5001~0x6000 reserved for p2p API */
	INIC_API_CFG80211_P2P_CH_RDY				= P2P_API_BASE + 1,
};

enum INIC_WIFI_H2C_API_ID {
	/* 0x0001~0x1000 reserved for basic API */
	INIC_API_WIFI_CONNECT						= BASIC_API_BASE + 1,
	INIC_API_WIFI_DISCONNECT					= BASIC_API_BASE + 2,
	INIC_API_WIFI_IS_CONNECTED_TO_AP			= BASIC_API_BASE + 3,
	INIC_API_WIFI_IS_RUNNING					= BASIC_API_BASE + 4,
	INIC_API_WIFI_SET_CHANNEL					= BASIC_API_BASE + 5,
	INIC_API_WIFI_GET_CHANNEL					= BASIC_API_BASE + 6,
	INIC_API_WIFI_SET_USR_CFG					= BASIC_API_BASE + 7,
	INIC_API_WIFI_SET_HOST_RTOS					= BASIC_API_BASE + 8,

	INIC_API_WIFI_ON							= BASIC_API_BASE + 9,
	INIC_API_WIFI_INIT_AP						= BASIC_API_BASE + 10,
	INIC_API_WIFI_DEINIT_AP						= BASIC_API_BASE + 11,
	INIC_API_WIFI_START_AP 						= BASIC_API_BASE + 12,
	INIC_API_WIFI_STOP_AP						= BASIC_API_BASE + 13,
	INIC_API_WIFI_SCAN_NETWROKS					= BASIC_API_BASE + 14,
	INIC_API_WIFI_GET_SCANNED_AP_INFO			= BASIC_API_BASE + 15,
	INIC_API_WIFI_SCAN_ABORT					= BASIC_API_BASE + 16,

	/* 0x1001~0x2000 reserved for ext API */
	INIC_API_WIFI_SET_MAC_ADDR					= EXT_API_BASE + 1,
	INIC_API_WIFI_GET_MAC_ADDR					= EXT_API_BASE + 2,
	INIC_API_WIFI_DRIVE_IS_MP					= EXT_API_BASE + 3,
	INIC_API_WIFI_GET_ASSOCIATED_CLIENT_LIST	= EXT_API_BASE + 4,
	INIC_API_WIFI_GET_SETTING					= EXT_API_BASE + 5,
	INIC_API_WIFI_SET_IPS_EN					= EXT_API_BASE + 6,
	INIC_API_WIFI_SET_LPS_EN					= EXT_API_BASE + 7,
	INIC_API_WIFI_SET_LPS_LISTEN_INTERVAL		= EXT_API_BASE + 8,
	INIC_API_WIFI_SET_MFP_SUPPORT				= EXT_API_BASE + 9,
	INIC_API_WIFI_SET_GROUP_ID					= EXT_API_BASE + 10,
	INIC_API_WIFI_SET_PMK_CACHE_EN				= EXT_API_BASE + 11,
	INIC_API_WIFI_SAE_STATUS					= EXT_API_BASE + 12,
	INIC_API_WIFI_FT_STATUS						= EXT_API_BASE + 13,
	INIC_API_WIFI_GET_SW_STATISTIC				= EXT_API_BASE + 14,
	INIC_API_WIFI_GET_PHY_STATISTIC				= EXT_API_BASE + 15,
	INIC_API_WIFI_SET_NETWORK_MODE				= EXT_API_BASE + 16,
	INIC_API_WIFI_SET_WPS_PHASE					= EXT_API_BASE + 17,
	INIC_API_WIFI_SET_GEN_IE					= EXT_API_BASE + 18,
	INIC_API_WIFI_SET_EAP_PHASE					= EXT_API_BASE + 19,
	INIC_API_WIFI_GET_EAP_PHASE					= EXT_API_BASE + 20,
	INIC_API_WIFI_SET_EAP_METHOD				= EXT_API_BASE + 21,
	INIC_API_WIFI_SEND_EAPOL					= EXT_API_BASE + 22,

	INIC_API_WIFI_CUS_IE						= EXT_API_BASE + 25,
	INIC_API_WIFI_SEND_MGNT						= EXT_API_BASE + 27,
	INIC_API_WIFI_SET_TXRATE_BY_TOS				= EXT_API_BASE + 28,
	INIC_API_WIFI_SET_EDCA_PARAM				= EXT_API_BASE + 29,
	INIC_API_WIFI_SET_TX_CCA					= EXT_API_BASE + 30,
	INIC_API_WIFI_SET_CTS2SEFL_DUR_AND_SEND		= EXT_API_BASE + 31,
	INIC_API_WIFI_GET_ANTENNA_INFO				= EXT_API_BASE + 32,
	INIC_API_WIFI_GET_BAND_TYPE					= EXT_API_BASE + 33,
	INIC_API_WIFI_DEL_STA						= EXT_API_BASE + 34,
	INIC_API_WIFI_AP_CH_SWITCH					= EXT_API_BASE + 35,
	INIC_API_WIFI_CONFIG_CSI					= EXT_API_BASE + 36,
	INIC_API_WIFI_GET_CSI_REPORT				= EXT_API_BASE + 37,
	INIC_API_WIFI_GET_CCMP_KEY					= EXT_API_BASE + 38,
	INIC_API_WIFI_SET_TX_POWER					= EXT_API_BASE + 39,
	INIC_API_WIFI_GET_TX_POWER					= EXT_API_BASE + 40,
	INIC_API_WIFI_SET_CONN_STEP_TRY_LIMIT		= EXT_API_BASE + 41,

	/* 0x2001~0x3000 reserved for inter API */
	INIC_API_WIFI_COEX_SET_PTA					= INTER_API_BASE + 1,
	INIC_API_WIFI_SET_WPA_MODE					= INTER_API_BASE + 2,
	INIC_API_WIFI_SET_PMF_MODE					= INTER_API_BASE + 3,
	INIC_API_WIFI_COEX_BT_RFK					= INTER_API_BASE + 4,
	INIC_API_WIFI_COEX_ZB_RFK					= INTER_API_BASE + 5,
	INIC_API_WIFI_SET_BT_SEL					= INTER_API_BASE + 6,
	INIC_API_WIFI_ADD_KEY						= INTER_API_BASE + 7,
	INIC_API_WIFI_GET_CHPLAN					= INTER_API_BASE + 8,
	INIC_API_WPA_4WAY_REPORT					= INTER_API_BASE + 9,
	INIC_API_WPA_PMKSA_OPS						= INTER_API_BASE + 10,
	INIC_API_WIFI_SET_OWE_PARAM					= INTER_API_BASE + 11,
	INIC_API_WIFI_PROMISC_INIT					= INTER_API_BASE + 12,
	INIC_API_WIFI_COEX_BT_HCI					= INTER_API_BASE + 13,
	INIC_API_WIFI_COEX_EXTCHIP					= INTER_API_BASE + 14,
	INIC_API_WIFI_COEX_VENDOR_INFO_SET			= INTER_API_BASE + 15,

	/* 0x3001~0x4000 reserved for misc API */
	INIC_API_WIFI_IP_UPDATE						= MISC_API_BASE + 1,
	INIC_API_WIFI_IWPRIV_INFO					= MISC_API_BASE + 2,
	INIC_API_WIFI_MP_CMD						= MISC_API_BASE + 3,
	INIC_API_WIFI_GET_NETWORK_MODE				= MISC_API_BASE + 4,
	INIC_API_WIFI_MSG_TO						= MISC_API_BASE + 5,
	INIC_API_WIFI_SPEAKER						= MISC_API_BASE + 6,
	INIC_API_WIFI_SET_CHPLAN					= MISC_API_BASE + 7,
	INIC_API_WIFI_SET_EDCCA_MODE				= MISC_API_BASE + 8,
	INIC_API_WIFI_GET_EDCCA_MODE				= MISC_API_BASE + 9,
	INIC_API_WIFI_SET_COUNTRY_CODE				= MISC_API_BASE + 10,
	INIC_API_WIFI_GET_COUNTRY_CODE				= MISC_API_BASE + 11,

	/* 0x4001~0x5000 reserved for nan API */
	INIC_API_NAN_INIT							= NAN_API_BASE + 1,
	INIC_API_NAN_DEINIT							= NAN_API_BASE + 2,
	INIC_API_NAN_START							= NAN_API_BASE + 3,
	INIC_API_NAN_STOP							= NAN_API_BASE + 4,
	INIC_API_NAN_ADD_FUNC						= NAN_API_BASE + 5,
	INIC_API_NAN_DEL_FUNC						= NAN_API_BASE + 6,
	INIC_API_NAN_CFGVENFOR						= NAN_API_BASE + 7,

	/* 0x5001~0x6000 reserved for p2p API */
	INIC_API_P2P_ROLE							= P2P_API_BASE + 1,
	INIC_API_P2P_REMAIN_ON_CH					= P2P_API_BASE + 2,

	/* 0x6001~0x7000 reserved for proxy API */
	INIC_API_WAR_OFFLOAD_CTRL					= PROXY_API_BASE + 1,
	INIC_API_WAR_SET_MDNS_PARA					= PROXY_API_BASE + 2,

	/* 0x7001~0x8000 reserved for sdio bridge API */
	INIC_API_BRIDGE_DHCP						= SDIO_BRIDGE_BASE + 1,
	INIC_API_BRIDGE_GET_IP						= SDIO_BRIDGE_BASE + 2,

	/* 0x8001~0x9000 reserved for wifi tunnel API */
	INIC_API_WTN_CMD							= WTN_API_BASE + 1,
};

enum IPC_LWIP_INFO_TYPE {
	INIC_WLAN_GET_IP = 0,
	INIC_WLAN_GET_GW = 1,
	INIC_WLAN_GET_GWMSK = 2,
	INIC_WLAN_GET_HW_ADDR = 3,
	INIC_WLAN_IS_VALID_IP = 4
};

#endif

