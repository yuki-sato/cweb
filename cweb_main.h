//
//  cweb_main.h
//  cweb
//
//  Created by oyabunn on 2014/07/19.
//  Copyright (c) 2014年 yuki-sato. All rights reserved.
//

#ifndef cweb_cweb_main_h
#define cweb_cweb_main_h

#include <netinet/in.h>

typedef struct _CWebTCPConnection {
    int socket;
    struct sockaddr_in client;
}CWebTCPConnection;

int CWebListen(int port);
void CWebResponse(CWebTCPConnection *connection, CWebHTTPResponse *response);

// delegates
void didReceiveCWebRequest(CWebTCPConnection *connection, CWebHTTPRequest *request);

#endif
