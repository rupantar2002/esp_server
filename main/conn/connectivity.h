#ifndef __CONNECTIVITY_H__
#define __CONNECTIVITY_H__

#include <stdbool.h>
#include <esp_wifi.h>

void connectivity_SetMode(wifi_mode_t mode);
bool connectivity_SetApCred(const char *ssid, const char *password);
bool connectivity_SetStaCred(const char *ssid, const char *password);
bool connectivity_StartWifi();
bool connectivity_Connect(bool autoReconnect);

bool connectivity_ApStatus(void);
bool connectivity_StaStatus(void);

#endif