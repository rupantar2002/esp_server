#include "connectivity.h"
#include "intf_wifi.h"
#include <string.h>
#include <tcpip_adapter_types.h>
#include <esp_log.h>

#include "server/server.h"

static const char *TAG = "connectivity";

static intf_wifi_credentials_t *pgApCred = NULL;
static intf_wifi_credentials_t *pgStaCred = NULL;
static wifi_mode_t gMode = WIFI_MODE_NULL;
static tcpip_adapter_ip_info_t gIpInfo = {0};
static bool gAutoReconn = false;
static bool gApConnFlg = false;
static bool gStaConnFlg = false;

void connectivity_SetMode(wifi_mode_t mode)
{
    gMode = mode;
}

bool connectivity_SetApCred(const char *ssid, const char *password)
{
    pgApCred = (intf_wifi_credentials_t *)pvPortMalloc(sizeof(intf_wifi_credentials_t));
    if (pgApCred == NULL)
    {
        ESP_LOGE(TAG, "failed to allocate memory\r\n");
        return false;
    }
    strcpy(pgApCred->ssid, ssid);
    strcpy(pgApCred->pass, password);

    printf("apSSID->%s\r\n", pgApCred->ssid);
    printf("apPASSWORD->%s\r\n", pgApCred->pass);

    return true;
}

bool connectivity_SetStaCred(const char *ssid, const char *password)
{
    pgStaCred = (intf_wifi_credentials_t *)pvPortMalloc(sizeof(intf_wifi_credentials_t));
    if (pgStaCred == NULL)
    {
        ESP_LOGE(TAG, "failed to allocate memory\r\n");
        return false;
    }
    strcpy(pgStaCred->ssid, ssid);
    strcpy(pgStaCred->pass, password);

    printf("staSSID->%s\r\n", pgStaCred->ssid);
    printf("staPASSWORD->%s\r\n", pgStaCred->pass);

    return true;
}

bool connectivity_StartWifi()
{
    IP4_ADDR(&gIpInfo.ip, 192, 168, 0, 1);
    IP4_ADDR(&gIpInfo.gw, 192, 168, 0, 1);
    IP4_ADDR(&gIpInfo.netmask, 255, 255, 255, 0);

    // if (pgApCred == NULL || pgStaCred == NULL || gMode == WIFI_MODE_NULL || gMode == WIFI_MODE_MAX)
    // {
    //     ESP_LOGE(TAG, "invalid arguments\r\n");
    //     return false;
    // }
    if (intf_wifi_Init(gMode, *pgApCred, *pgStaCred, gIpInfo) != INTF_WIFI_STATUS_OK)
    {
        return false;
    }
    server_Init();
    return true;
}

bool connectivity_Connect(bool autoReconnect)
{
    gAutoReconn = autoReconnect;
    if (intf_wifi_Connect() != INTF_WIFI_STATUS_OK)
    {
        return false;
    }
    return true;
}

bool connectivity_ApStatus(void) { return gApConnFlg; }

bool connectivity_StaStatus(void) { return gStaConnFlg; }

/* ================WIFI CALLBACKS====================== */

void intf_wifi_ApStartedEventCallback()
{
    if (!gApConnFlg)
    {

        gApConnFlg = true;
    }
}

void intf_wifi_ApStaConnectedEventCallback(wifi_event_ap_staconnected_t info)
{
    // server_StartSocket();
    server_Start();
}

// void intf_wifi_ApStaDisconnectedEventCallback(wifi_event_ap_stadisconnected_t info);

void intf_wifi_ApStoppedEventCallback()
{
    if (gApConnFlg)
    {
        // server_StopSocket();
        server_Stop();
        gApConnFlg = false;
    }
}

// void intf_wifi_ScanCompleteEventCallback();

// void intf_wifi_StaStartedEventCallback();

void intf_wifi_StaDisconnectedEventCallback(uint8_t disconnectReason)
{
    if (gStaConnFlg)
    {
        gStaConnFlg = false;
    }

    if (gAutoReconn)
    {
        intf_wifi_Connect();
    }
}

void intf_wifi_StaConnectedEventCallback(esp_netif_ip_info_t ipInfo)
{
    printf("sta connected\r\n");
    if (!gStaConnFlg)
    {
        gStaConnFlg = true;
    }
}

// void intf_wifi_StaRssiLowEventCallback(int32_t rssi);
// void intf_wifi_StaStoppedEventCallback();