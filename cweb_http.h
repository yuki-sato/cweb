
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
    char * method;
    CWebObject  *path;
    CWebObject  *queries;
    CWebObject  *params;
    CWebObject  *headers;
}CWebHTTPRequest;

typedef struct _CWebHTTPResponse {
    CWebObject  *headers;
    int     statusCode;
    char    *body;
}CWebHTTPResponse;

CWebHTTPRequest *CWebRequestFromRequestString(char *requestString);
int CWebRequestMatch(CWebHTTPRequest *request, char *method, char *path);
CWebHTTPResponse *CWebResponseCreateWithHTMLBODY(char **html);

void CWebRequestFree(CWebHTTPRequest *request);
void CWebResponseFree(CWebHTTPResponse *response);

#endif
