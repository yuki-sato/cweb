//
//  main.c
//  cweb
//
//  Created by oyabunn on 2014/07/05.
//  Copyright (c) 2014年 yuki-sato. All rights reserved.
//


#include "cweb.h"

#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    CWebListen(8080);
    return 1;
}

void didReceiveCWebRequest(CWebTCPConnection *connection, CWebHTTPRequest *request) {
    CWebHTTPResponse *response=NULL;
    char *html = NULL;
    if(CWebRequestMatch(request, "GET", "/")){
        CWebObject *obj = CWebObjectCreateDictionaryStringValueWithCopy("title", "/");
        html = CWebRenderHTML("./index.html", obj);
        CWebObjectFree(obj);
        response = CWebResponseCreateWithHTMLBODY(&html);
        
    }else{
        CWebObject *obj = CWebObjectCreateDictionaryStringValueWithCopy("title", "404 Notfound");
        html = CWebRenderHTML("./index.html", obj);
        response = CWebResponseCreateWithHTMLBODY(&html);
        CWebObjectFree(obj);
        response->statusCode = 404;
    }
    CWebResponse(connection, response);
    CWebResponseFree(response);
}