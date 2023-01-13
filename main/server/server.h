#ifndef __SERVER_H__
#define __SERVER_H__

void server_Init();

void server_Start();

void server_StartSocket(int port);

void server_StopSocket(int port);

void server_Stop();

#endif