#include "bds_device.h"

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "wifi_intf_drv_to_app_basic.h"

#define TAG "device"

static char* s_my_mac = NULL;
char*        bds_get_wifi_mac() {
    if (!s_my_mac) {
        s_my_mac      = bdsc_malloc(18);
        rtw_mac_t mac = {0};
        int       ret = wifi_get_mac_address(0, &mac, 0);
        if (ret != 0) {
            bdsc_loge(TAG, "get wifi mac failed! ret=%d", ret);
            char* default_mac = "11:11:11:11:11:11";
            memcpy(s_my_mac, default_mac, 17);
        } else {
            sprintf(s_my_mac, "%02x:%02x:%02x:%02x:%02x:%02x", mac.octet[0], mac.octet[1], mac.octet[2], mac.octet[3],
                    mac.octet[4], mac.octet[5]);
        }
        s_my_mac[17] = '\0';
    }
    return s_my_mac;
}

char* bds_get_udid() {
    return bds_get_wifi_mac();
}
