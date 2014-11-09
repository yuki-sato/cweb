
//
//  cweb_http.h
//  cweb
//
//  Created by oyabunn on 2014/07/21.
//  Copyright (c) 2014年 yuki-sato. All rights reserved.
//

#ifndef cweb_cweb_http_h
#define cweb_cweb_http_h

#include "cweb_object.h"

typedef struct _CWebHTTPRequest {
    char * ip;        // like "192.168.0.0"
    int port;
    
    CWebObject  *path;
    char * method;
    CWebObject  *queries;
    CWebObject  *params;
    CWebObject  *headers;
}CWebHTTPRequest;

typedef struct _CWebHTTPResponse {
    CWebObject  *headers;
    int     statusCode;
    char    *body;
}CWebHTTPResponse;

// instance method
CWebHTTPRequest *CWebRequestCreateWith(char *ip, char *method, int port, CWebObject *path);

// for listening
CWebHTTPRequest *CWebRequestFromRequestString(char *requestString);
CWebHTTPResponse *CwebResponseFromResponseString(char *responseString, unsigned long length);

int CWebRequestMatch(CWebHTTPRequest *request, char *method, char *path);
CWebHTTPResponse *CWebResponseCreateWithHTMLBODY(char **html);

// for sending
char * CWebHTTP_CreateRequestStringFrom(CWebHTTPRequest *request);


void CWebRequestFree(CWebHTTPRequest *request);
void CWebResponseFree(CWebHTTPResponse *response);

#endif
