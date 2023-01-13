#include <string.h>
#include "esp_event.h"
#include "nvs_flash.h"
#include "intf_wifi.h"
#include "esp_log.h"

static const char *__attribute__((__unused__)) TAG = "INTF_WIFI";

static esp_netif_t *pgApNetif;
static esp_netif_t *pgStaNetif;
static uint8_t gIsStaConnectedToAP;
static intf_wifi_scanResApInfo_t gScanList[INTF_WIFI_CONFIG_STA_AP_SCAN_LIST_LENGTH_MAX];
static uint16_t gScanListLength;
static uint16_t gScanInProgress;

/* update */
static wifi_ap_record_t gApInfoList[INTF_WIFI_CONFIG_STA_AP_SCAN_LIST_LENGTH_MAX];

void __attribute__((weak)) intf_wifi_ApStartedEventCallback()
{
}
void __attribute__((weak)) intf_wifi_ScanCompleteEventCallback()
{
}
void __attribute__((weak)) intf_wifi_ApStaConnectedEventCallback(wifi_event_ap_staconnected_t __attribute__((__unused__)) info)
{
}
void __attribute__((weak)) intf_wifi_ApStaDisconnectedEventCallback(wifi_event_ap_stadisconnected_t __attribute__((__unused__)) info)
{
}
void __attribute__((weak)) intf_wifi_ApStoppedEventCallback()
{
}

void __attribute__((weak)) intf_wifi_StaStartedEventCallback()
{
}
void __attribute__((weak)) intf_wifi_StaDisconnectedEventCallback(uint8_t __attribute__((__unused__)) disconnectReason)
{
}
void __attribute__((weak)) intf_wifi_StaConnectedEventCallback(esp_netif_ip_info_t __attribute__((__unused__)) ipInfo)
{
}
void __attribute__((weak)) intf_wifi_StaRssiLowEventCallback(int32_t __attribute__((__unused__)) rssi)
{
}
void __attribute__((weak)) intf_wifi_StaStoppedEventCallback()
{
}

void __attribute__((weak)) intf_wifi_DeinitCallback()
{
}
void __attribute__((weak)) intf_wifi_InitCallback()
{
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_SCAN_DONE)
    {
#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "Scan Complete");
#endif
        uint16_t scannedApCount = INTF_WIFI_CONFIG_STA_AP_SCAN_LIST_LENGTH_MAX;
        // wifi_ap_record_t apInfoList[INTF_WIFI_CONFIG_STA_AP_SCAN_LIST_LENGTH_MAX];
        // gScanListLength = 0;
        // memset(gScanList, 0, sizeof(gScanList));
        // memset(apInfoList, 0, sizeof(apInfoList));
        // if (esp_wifi_scan_get_ap_records(&scannedApCount, apInfoList) == ESP_OK)
        // {
        //     for (int i = 0; i < scannedApCount; i++)
        //     {
        //         strncpy((char *)gScanList[i].appSSID, (char *)apInfoList[i].ssid, sizeof(gScanList[i].appSSID));
        //         gScanList[i].RSSI = apInfoList[i].rssi;
        //     }
        // }
        // gScanListLength = scannedApCount;
        // gScanInProgress = 0;

        gScanListLength = 0;
        memset(gScanList, 0, sizeof(gScanList));
        memset(gApInfoList, 0, sizeof(gApInfoList));
        if (esp_wifi_scan_get_ap_records(&scannedApCount, gApInfoList) == ESP_OK)
        {
            for (int i = 0; i < scannedApCount; i++)
            {
                strncpy((char *)gScanList[i].appSSID, (char *)gApInfoList[i].ssid, sizeof(gScanList[i].appSSID));
                gScanList[i].RSSI = gApInfoList[i].rssi;
            }
        }
        gScanListLength = scannedApCount;
        gScanInProgress = 0;

        intf_wifi_ScanCompleteEventCallback();
    }
    else if (event_id == WIFI_EVENT_AP_START)
    {
#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "AP Started");
#endif
        intf_wifi_ApStartedEventCallback();
    }
    else if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        intf_wifi_ApStaConnectedEventCallback(*event);
#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
#endif
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        intf_wifi_ApStaDisconnectedEventCallback(*event);
#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
#endif
    }
    else if (event_id == WIFI_EVENT_AP_STOP)
    {
        intf_wifi_ApStoppedEventCallback();
#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "AP Stopped.");
#endif
    }

    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "STA STARTED");
#endif
        intf_wifi_StaStartedEventCallback();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifi_event_sta_disconnected_t *__attribute__((__unused__)) event = (wifi_event_sta_disconnected_t *)event_data;

#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "Disconnect Reason: %d", event->reason);
#endif
        gIsStaConnectedToAP = 0;
        intf_wifi_StaDisconnectedEventCallback(event->reason);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_BSS_RSSI_LOW)
    {
        wifi_event_bss_rssi_low_t *event = (wifi_event_bss_rssi_low_t *)event_data;

#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "Station has low RSSI, %d dBm.", event->rssi);
#endif
        intf_wifi_StaRssiLowEventCallback(event->rssi);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *__attribute__((__unused__)) event = (ip_event_got_ip_t *)event_data;

#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
#endif
        gIsStaConnectedToAP = 1;
        intf_wifi_StaConnectedEventCallback(event->ip_info);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
    {
#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "STA STOPPED");
#endif
        intf_wifi_StaStoppedEventCallback();
    }
}
intf_wifi_status_t intf_wifi_StaIsConnectedToAp()
{
    esp_err_t err;
    wifi_ap_record_t ap_info;
    err = esp_wifi_sta_get_ap_info(&ap_info);
    if (err == ESP_ERR_WIFI_NOT_CONNECT)
    {
        return INTF_WIFI_STATUS_STA_DISCONNECTED;
    }
    else if (err == ESP_OK)
    {
        return INTF_WIFI_STATUS_STA_CONNECTED;
    }
    else
    {
        ESP_LOGE(TAG, "Error fetching ap details");
        return INTF_WIFI_STATUS_ERROR;
    }
}
intf_wifi_status_t intf_wifi_StartScan()
{
    if (gScanInProgress)
    {
        ESP_LOGE(TAG, "Scan already in progress, cannot start fresh scan");
        return INTF_WIFI_STATUS_ERROR_SCAN_ALREADY_IN_PROGRESS;
    }
    esp_err_t err = esp_wifi_scan_start(NULL, false);
    if (err == ESP_ERR_WIFI_NOT_INIT || err == ESP_ERR_WIFI_NOT_STARTED)
    {
        ESP_LOGE(TAG, "Cannot Start Scan; Wifi Not Initalized or not started");
        return INTF_WIFI_STATUS_ERROR;
    }
    else if (err == ESP_ERR_WIFI_STATE)
    {
        ESP_LOGE(TAG, "Cannot Start Scan; Sta connection in progress");
        return INTF_WIFI_STATUS_ERROR;
    }
    else if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Cannot start scan");
        return INTF_WIFI_STATUS_ERROR;
    }
    gScanInProgress = 1;
    return INTF_WIFI_STATUS_OK;
}
intf_wifi_status_t intf_wifi_StopScan()
{
    if (gScanInProgress)
    {
        esp_err_t err = esp_wifi_scan_stop();
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error stopping scan");
            return INTF_WIFI_STATUS_ERROR;
        }
    }
    gScanInProgress = 0;
    return INTF_WIFI_STATUS_OK;
}
intf_wifi_status_t intf_wifi_GetScanList(intf_wifi_scanResApInfo_t **pScanList, uint16_t *itemCount)
{
    intf_wifi_status_t returnStatus;
    if (gScanInProgress)
    {
        // ESP_LOGE(TAG, "Scan in progress, wait for the intf_wifi_ScanCompleteEventCallback()");
        returnStatus = INTF_WIFI_STATUS_ERROR_SCAN_ALREADY_IN_PROGRESS;
        goto __exit_point;
    }
    *itemCount = gScanListLength;
    *pScanList = gScanList;
    returnStatus = INTF_WIFI_STATUS_OK;
__exit_point:
    return returnStatus;
}

intf_wifi_status_t intf_wifi_Connect()
{
    esp_err_t err;
    err = esp_wifi_connect();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error Connecting Wifi");
        return INTF_WIFI_STATUS_ERROR;
    }

    return INTF_WIFI_STATUS_OK;
}
intf_wifi_status_t intf_wifi_Disconnect()
{
    esp_err_t err;
    err = esp_wifi_disconnect();
    if (err != ESP_OK)
        return INTF_WIFI_STATUS_OK;
    else
    {
#ifdef INTF_WIFI_LOG_ENABLE
        ESP_LOGI(TAG, "Error Disconnecting Wifi");
#endif
        return INTF_WIFI_STATUS_ERROR;
    }
}
void intf_wifi_DeInit()
{
    intf_wifi_DeinitCallback();
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler);
    if (pgApNetif != NULL)
    {
        esp_netif_destroy_default_wifi(pgApNetif);
    }
    if (pgStaNetif != NULL)
    {
        esp_netif_destroy_default_wifi(pgStaNetif);
    }
    pgStaNetif = NULL;
    pgApNetif = NULL;
    gIsStaConnectedToAP = 0;
#ifdef INTF_WIFI_LOG_ENABLE
    ESP_LOGI(TAG, "WIFI Denitialized");
#endif
}
intf_wifi_status_t intf_wifi_StaSetApCredentials(intf_wifi_credentials_t creds)
{
    esp_err_t err;
    wifi_config_t wificonfig;
    memset(&wificonfig, 0, sizeof(wificonfig));

    err = esp_wifi_get_config(ESP_IF_WIFI_STA, &wificonfig);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error getting STA config");
        return INTF_WIFI_STATUS_ERROR;
    }

    strcpy((char *)wificonfig.sta.ssid, creds.ssid);
    strcpy((char *)wificonfig.sta.password, creds.pass);

    err = esp_wifi_set_config(ESP_IF_WIFI_STA, &wificonfig);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error setting STA config");
        return INTF_WIFI_STATUS_ERROR;
    }

    return INTF_WIFI_STATUS_OK;
}
intf_wifi_status_t intf_wifi_ApSetCredentials(intf_wifi_credentials_t creds)
{
    esp_err_t err;
    wifi_config_t wificonfig;
    memset(&wificonfig, 0, sizeof(wificonfig));

    err = esp_wifi_get_config(ESP_IF_WIFI_AP, &wificonfig);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error getting AP config");
        return INTF_WIFI_STATUS_ERROR;
    }
    wificonfig.ap.ssid_len = strlen(creds.ssid);
    strcpy((char *)wificonfig.ap.ssid, creds.ssid);
    strcpy((char *)wificonfig.ap.password, creds.pass);

    err = esp_wifi_set_config(ESP_IF_WIFI_AP, &wificonfig);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error setting STA config");
        return INTF_WIFI_STATUS_ERROR;
    }

    return INTF_WIFI_STATUS_OK;
}
intf_wifi_status_t intf_wifi_Init(wifi_mode_t mode, intf_wifi_credentials_t apCreds, intf_wifi_credentials_t staCreds, tcpip_adapter_ip_info_t ipInfo)
{

    esp_err_t err;
    if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA)
    {
        if (strlen(apCreds.ssid) == 0 || strlen(apCreds.ssid) >= sizeof(apCreds.ssid))
        {
            ESP_LOGE(TAG, "AP SSID LENGTH INVALID");
            return INTF_WIFI_STATUS_ERROR_BAD_ARGUMENT;
        }

        if (strlen(apCreds.pass) < 8 || strlen(apCreds.pass) >= sizeof(apCreds.pass))
        {
            ESP_LOGE(TAG, "AP PASS LENGTH INVALID");
            return INTF_WIFI_STATUS_ERROR_BAD_ARGUMENT;
        }
    }
    else if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA)
    {
        if (strlen(staCreds.ssid) != 0 && strlen(staCreds.ssid) >= sizeof(staCreds.ssid))
        {
            ESP_LOGE(TAG, "STA SSID LENGTH INVALID");
            return INTF_WIFI_STATUS_ERROR_BAD_ARGUMENT;
        }

        if (strlen(staCreds.pass) != 0 && (strlen(staCreds.pass) < 8 || strlen(staCreds.pass) >= sizeof(staCreds.pass)))
        {
            ESP_LOGE(TAG, "STA PASS LENGTH INVALID");
            return INTF_WIFI_STATUS_ERROR_BAD_ARGUMENT;
        }
    }
    else
    {
        ESP_LOGE(TAG, "INVALID MODE ARGUMENT");
        return INTF_WIFI_STATUS_ERROR_BAD_ARGUMENT;
    }

    if (pgApNetif != NULL || pgStaNetif != NULL)
    {
        ESP_LOGE(TAG, "Already initialized");
        return INTF_WIFI_STATUS_ERROR_ALREADY_INITIALIZED;
    }

    if (mode == WIFI_MODE_AP)
    {
        pgApNetif = esp_netif_create_default_wifi_ap();
    }
    else if (mode == WIFI_MODE_STA)
    {
        pgStaNetif = esp_netif_create_default_wifi_sta();
    }
    else
    {
        pgApNetif = esp_netif_create_default_wifi_ap();
        pgStaNetif = esp_netif_create_default_wifi_sta();
    }

    if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA)
    {
        tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);
        if (tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ipInfo) != ESP_OK)
        {
            ESP_LOGE(TAG, "Error: %d", INTF_WIFI_STATUS_ERROR_CANNOT_SET_IP);
            return INTF_WIFI_STATUS_ERROR_CANNOT_SET_IP;
        }

        tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error initializing wifi");
        return INTF_WIFI_STATUS_ERROR;
    }

    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error Registering wifi event handler");
        return INTF_WIFI_STATUS_ERROR;
    }

    if ((mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA))
    {
        err = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error Registering IP event handler");
            return INTF_WIFI_STATUS_ERROR;
        }
    }

    err = esp_wifi_set_mode(mode);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error Setting wifi mode");
        return INTF_WIFI_STATUS_ERROR;
    }

    if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA)
    {
        wifi_config_t apWifiConfig = {
            .ap = {
                .ssid_len = strlen((char *)apCreds.ssid),
                .channel = INTF_WIFI_CONFIG_AP_CHANNEL,
                .max_connection = INTF_WIFI_CONFIG_AP_MAX_STA_CONN,
                .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            },
        };
        strcpy((char *)apWifiConfig.ap.ssid, apCreds.ssid);
        strcpy((char *)apWifiConfig.ap.password, apCreds.pass);

        err = esp_wifi_set_config(ESP_IF_WIFI_AP, &apWifiConfig);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error[%d] setting AP config", err);
            return INTF_WIFI_STATUS_ERROR;
        }
    }

    if ((mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA))
    {
        wifi_config_t staWificonfig;
        memset(&staWificonfig, 0, sizeof(staWificonfig));

        staWificonfig.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
        staWificonfig.sta.pmf_cfg.capable = true;
        staWificonfig.sta.pmf_cfg.required = false;
        staWificonfig.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
        if (strlen(staCreds.ssid) > 0 && strlen(staCreds.pass) > 0)
        {
            strcpy((char *)staWificonfig.sta.ssid, staCreds.ssid);
            strcpy((char *)staWificonfig.sta.password, staCreds.pass);
        }

        err = esp_wifi_set_config(ESP_IF_WIFI_STA, &staWificonfig);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error setting STA config");
            return INTF_WIFI_STATUS_ERROR;
        }
    }

    err = esp_wifi_start();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error starting wifi");
        return INTF_WIFI_STATUS_ERROR;
    }

    return INTF_WIFI_STATUS_OK;
}
