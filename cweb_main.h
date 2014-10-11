//
//  cweb_main.h
//  cweb
//
//  Created by oyabunn on 2014/07/19.
//  Copyright (c) 2014年 yuki-sato. All rights reserved.
//

#ifndef cweb_cweb_main_h
#define cweb_cweb_main_h

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

typedef struct _CWebTCPConnection {
    int socket;
    struct sockaddr_in client;
}CWebTCPConnection;

int CWebListen(int port);
void CWebResponse(CWebTCPConnection *connection, CWebHTTPResponse *response);
CWebHTTPResponse * CWebRequest(CWebHTTPRequest *request);
// delegates
void didReceiveCWebRequest(CWebTCPConnection *connection, CWebHTTPRequest *request);

#endif
