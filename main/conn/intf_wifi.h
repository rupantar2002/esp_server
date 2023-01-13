#ifndef __INTF_WIFI_AP_H__
#define __INTF_WIFI_AP_H__
#include "esp_wifi.h"

#define INTF_WIFI_LOG_ENABLE 

#define INTF_WIFI_CONFIG_AP_CHANNEL 1
#define INTF_WIFI_CONFIG_AP_MAX_STA_CONN 1
#define INTF_WIFI_CONFIG_STA_AP_SCAN_LIST_LENGTH_MAX 20


typedef enum
{
    INTF_WIFI_STATUS_ERROR_SCAN_ALREADY_IN_PROGRESS=-6,
    INTF_WIFI_STATUS_ERROR_ALREADY_INITIALIZED =-5,
    INTF_WIFI_STATUS_ERROR_CANNOT_SET_IP=-4,
    INTF_WIFI_STATUS_ERROR_BAD_ARGUMENT=-3,
    INTF_WIFI_STATUS_ERROR_CONNECTION_FAILED = -2,
    INTF_WIFI_STATUS_ERROR = -1,
    INTF_WIFI_STATUS_OK = 0,
    INTF_WIFI_STATUS_STA_CONNECTED=1,
    INTF_WIFI_STATUS_STA_DISCONNECTED=2,
} intf_wifi_status_t;

typedef struct{
    char ssid[32];
    char pass[65];
}intf_wifi_credentials_t;

typedef struct{
    uint8_t appSSID[32];
    int RSSI;
}intf_wifi_scanResApInfo_t;

intf_wifi_status_t intf_wifi_Init(wifi_mode_t mode,intf_wifi_credentials_t apCreds,intf_wifi_credentials_t staCreds,tcpip_adapter_ip_info_t ipInfo);
void intf_wifi_DeInit();
intf_wifi_status_t intf_wifi_Connect();
intf_wifi_status_t intf_wifi_Disconnect();
intf_wifi_status_t intf_wifi_StartScan();
intf_wifi_status_t intf_wifi_StopScan();
intf_wifi_status_t intf_wifi_GetScanList(intf_wifi_scanResApInfo_t** pScanList,uint16_t* itemCount);
intf_wifi_status_t intf_wifi_StaIsConnectedToAp();
intf_wifi_status_t intf_wifi_StaSetApCredentials(intf_wifi_credentials_t creds);
intf_wifi_status_t intf_wifi_ApSetCredentials(intf_wifi_credentials_t creds);

void intf_wifi_DeinitCallback();
void intf_wifi_InitCallback();

void intf_wifi_ApStartedEventCallback();
void intf_wifi_ApStaConnectedEventCallback(wifi_event_ap_staconnected_t info);
void intf_wifi_ApStaDisconnectedEventCallback(wifi_event_ap_stadisconnected_t info);
void intf_wifi_ApStoppedEventCallback();
void intf_wifi_ScanCompleteEventCallback();

void intf_wifi_StaStartedEventCallback();
void intf_wifi_StaDisconnectedEventCallback(uint8_t disconnectReason);
void intf_wifi_StaConnectedEventCallback(esp_netif_ip_info_t ipInfo);
void intf_wifi_StaRssiLowEventCallback(int32_t rssi);
void intf_wifi_StaStoppedEventCallback();

#endif