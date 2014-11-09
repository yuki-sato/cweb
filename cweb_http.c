//
//  File.c
//  cweb
//
//  Created by oyabunn on 2014/07/21.
//  Copyright (c) 2014年 yuki-sato. All rights reserved.
//

#include "cweb_http.h"
#include "cweb_object.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void CWebRequestFillPathQuery(CWebHTTPRequest *request, char *uri, unsigned long uriLength);
unsigned long CWebRequestFillHeaders(CWebHTTPRequest *request, char *requestString, unsigned long head, unsigned long textLength);

//===============================================
#pragma mark - Instant Methods -
//===============================================
CWebHTTPRequest *CWebRequestCreateWith(char *ip, char *method, int port, CWebObject *path) {
    CWebHTTPRequest *request = malloc(sizeof(CWebHTTPRequest));
    request->ip = ip;
    request->port = port;
    request->method = method;
    request->path = path;
    request->headers = NULL;
    request->params = NULL;
    request->queries = NULL;
    return request;
}

//===============================================
#pragma mark - Request -
//===============================================
CWebHTTPRequest *CWebRequestFromRequestString(char *requestString) {
    if(requestString == NULL)return NULL;
    
    CWebHTTPRequest *request = CWebRequestCreateWith(NULL, NULL, 0, NULL);
    
    unsigned long textLength = strlen(requestString);
    unsigned long head = 0 ;
    unsigned long length = 0;
    
    // method
    for (; head + length<textLength; length++) {
        if(requestString[head+length] == ' '){
            break;
        }
    }
    char *text = malloc(sizeof(char) * (length+1));
    memcpy(text, requestString, length);
    text[length] = '\0';
    request->method = text;
    
    // uri
    head += length + 1;
    length = 0;
    for (; head + length<textLength; length++) {
        if(requestString[head+length] == ' '){
            break;
        }
    }
    text = malloc(sizeof(char) * (length+1));
    memcpy(text, &requestString[head], length);
    text[length] = '\0';
    CWebRequestFillPathQuery(request, text, length);
    free(text);
    
    // headers
    for (; head + length<textLength; length++) {
        if(requestString[head+length] == '\n'){
            head += length + 1;
            break;
        }
    }
    head = CWebRequestFillHeaders(request, requestString, head, textLength);
    
    // params
//#warning TODO
    
    return request;
}

CWebHTTPResponse *CwebResponseFromResponseString(char *responseString, unsigned long length) {
    unsigned long index = 0;
    unsigned long i = 0;
    char statusCode[4];
    
    if(!responseString || length == 0)
        return NULL;
    
    CWebHTTPResponse *response = (CWebHTTPResponse *)malloc(sizeof(CWebHTTPResponse));
    response->headers = NULL;
    response->body = NULL;
    
    // skip to status
    while(1) {
        if(responseString[index++] == ' ')break;
        if(index == length)return NULL;     // nostatus code
    }
    i=0;
    while(1) {
        statusCode[i++] = responseString[index++];
        if(responseString[index] == ' ')break;
        if(index == length)return NULL;
    }
    statusCode[3] = '\0';
    response->statusCode = atoi(statusCode);
    // skip to headerField
    while(1) {
        if(responseString[index++] == '\n')break;
        if(index == length)return NULL;
    }
    
    // headerField
    // TODO
    
    return response;
}

void CWebRequestFillPathQuery(CWebHTTPRequest *request, char *uri, unsigned long uriLength) {
    int hasQuery = 0;
    int isLastCharactor = 0;
    
    unsigned long head = 1; //skip first /
    unsigned long length = 0;
    
    // path
    CWebObject *array = NULL;
    for (; head + length<uriLength;) {
        hasQuery = uri[head+length] == '?';
        isLastCharactor = head+length+1 == uriLength;
        if(uri[head+length] == '/' || hasQuery || isLastCharactor){
            if(isLastCharactor && uri[head+length] != '/')
                length++;
            if(length!=0){
                char *text = malloc(sizeof(char) * (length+1));
                memcpy(text, &uri[head], length);
                text[length] = '\0';
                if(array==NULL){
                    array = CWebObjectCreateArray(text, CWebObjectTypeString);
                }else{
                    CWebObjectAddObjectToArray(array, text, CWebObjectTypeString);
                }
            }
            head += length + 1;//  /a/b
            length = 0;
            if(hasQuery){
                break;
            }
        }else{
            length++;
        }
    }
    request->path = array;
    
    if(hasQuery){
        CWebObject *query = NULL;
        char *key = NULL;
        for (; head + length<uriLength;) {
            isLastCharactor = head+length+1 == uriLength;
            if(key==NULL && uri[head+length] == '='){
                if(length!=0){
                    char *text = malloc(sizeof(char) * (length+1));
                    memcpy(text, &uri[head], length);
                    text[length] = '\0';
                    key = text;
                }
                head += length + 1;
                length = 0;
            }else if(key!=NULL && (uri[head+length] == '&' || isLastCharactor)){
                if(isLastCharactor)
                    length++;
                if(length!=0){
                    char *text = malloc(sizeof(char) * (length+1));
                    memcpy(text, &uri[head], length);
                    text[length] = '\0';
                    
                    if(query==NULL){
                        query = CWebObjectCreateDictionary(key, text, CWebObjectTypeString);
                    }else{
                        CWebObjectAddObjectToDictionary(query, key, text, CWebObjectTypeString);
                    }
                    key = NULL;
                    
                }
                head += length + 1;
                length = 0;
            }else{
                length++;
            }
        }
        request->queries = query;
    }
}

unsigned long CWebRequestFillHeaders(CWebHTTPRequest *request, char *requestString, unsigned long head, unsigned long textLength) {
    int isLastCharactor = 0;
    
    unsigned long length = 0;
    
    // headers
    CWebObject *header = NULL;
    char *key = NULL;
    for (; head + length<textLength;) {
        isLastCharactor = head+length+1 == textLength;
        if(key==NULL && requestString[head+length] == ':'){
            if(length!=0){
                char *text = malloc(sizeof(char) * (length+1));
                memcpy(text, &requestString[head], length);
                text[length] = '\0';
                key = text;
            }
            head += length + 1 + 1; // skip space
            length = 0;
        }else if(key!=NULL && (requestString[head+length] == '\n' || isLastCharactor)){
            if(isLastCharactor)
                length++;
            if(length!=0){
                char *text = malloc(sizeof(char) * (length+1));
                memcpy(text, &requestString[head], length);
                text[length] = '\0';
                
                if(header==NULL){
                    header = CWebObjectCreateDictionary(key, text, CWebObjectTypeString);
                }else{
                    CWebObjectAddObjectToDictionary(header, key, text, CWebObjectTypeString);
                }
                key = NULL;
                
            }
            head += length + 1;
            length = 0;
        }else if(key==NULL && requestString[head+length] == '\n'){
            // empty line is end of header field
            head += length + 1;
            break;
        }else{
            length++;
        }
    }
    request->headers = header;
    head += length;
    return head;
}

// /stores/:id
// /stores/*
int CWebRequestMatch(CWebHTTPRequest *request, char *method, char *path) {
    // path may NULL(= any match)
    if(path==NULL || path[0] == '\0')return 1;
    if(request->method==NULL)return 0;  // can't process
    if(method != NULL && strcmp(method, request->method)!=0)return 0;
    
    unsigned long pathLength = strlen(path);
    unsigned long head = 1;
    unsigned long length = 0;
    int isLastCharactor=0;
    CWebObject *array = NULL;
    for (; head + length<pathLength;) {
        isLastCharactor = head+length+1 == pathLength;
        if(path[head+length] == '/' || isLastCharactor){
            if(isLastCharactor && path[head+length] != '/')
                length++;
            if(length!=0){
                char *text = malloc(sizeof(char) * (length+1));
                memcpy(text, &path[head], length);
                text[length] = '\0';
                if(array==NULL){
                    array = CWebObjectCreateArray(text, CWebObjectTypeString);
                }else{
                    CWebObjectAddObjectToArray(array, text, CWebObjectTypeString);
                }
            }
            head += length + 1;//  /a/b
            length = 0;
        }else{
            length++;
        }
    }

    CWebObject *req = request->path;
    while (1) {
        if(array==NULL){
            return (req==NULL) ? 1 : 0;
        }else if(req==NULL){
            // still have condition
            if(array->_next==NULL && strcmp("*", array->value)==0){
                return 1;
            }
            return 0;
        }else{
            if(strcmp(req->value, array->value)==0){
                // continue
            }else if(array->_next==NULL && strcmp("*", array->value)==0){
                return 1;
            }else{
                char *text = array->value;
                if(text[0] == ':'){
                    // path parameter. continue
                }else{
                    // not match string
                    return 0;
                }
            }
        }
        req = req->_next;
        array = array->_next;
    }
}

//===============================================
#pragma mark - Response -
//===============================================
CWebHTTPResponse *CWebResponseCreateWithHTMLBODY(char **html) {
    if(html==NULL){
        printf("CWeb Error No html body. maybe your rendering html is not exist\n");
        return NULL;
    }
    
    CWebHTTPResponse *response = malloc(sizeof(CWebHTTPResponse));
    response->body = *html;
    *html = NULL;   // passed html from response
    response->statusCode = 200;
    
    // fill normal headers
    CWebObject *headers = CWebObjectCreateDictionaryStringValueWithCopy("Connection", "close");
    CWebObjectAddStringObjectWithCopyToDictionary(headers, "Server", "CWeb");
    CWebObjectAddStringObjectWithCopyToDictionary(headers, "Content-Type", "text/html");
    char text[10];
    sprintf(text, "%lu", strlen(response->body));
    CWebObjectAddStringObjectWithCopyToDictionary(headers, "Content-Length", text);
    
    response->headers = headers;
    
    return response;
}

//===============================================
#pragma mark - Request -
//===============================================
// memory over lap is possible.
char * CWebHTTP_CreateRequestStringFrom(CWebHTTPRequest *request) {
    if(request==NULL)
        return NULL;
    unsigned long index = 0;
    unsigned long i = 0;
    char *buffer = malloc(sizeof(char) * 0xFFFF);
    
    // example.) "GET / HTTP/1.1\nHost: yuki-sato.com\n\n";
    
    // method
    while (1) {
        if(request->method[index] == '\0')break;
        buffer[index] = request->method[index];
        index++;
    }
    buffer[index++] = ' ';
    
    CWebObject *obj;
    
    // build path field
    obj = request->path;
    if(!obj){
        buffer[index++] = '/';
    }else{
        while (1) {
            i=0;
            while (1) {
                if(((char *)obj->value)[i] == '\0') break;
                buffer[index++] = ((char *)obj->value)[i++];
            }
            obj = obj->_next;
            if(!obj)break;
            buffer[index++] = '/';
        }
    }
    
    // build query field
    obj = request->queries;
    if(obj){
        buffer[index++] = '?';
        while (1) {
            // "key: value\n"
            i=0;
            while (1) {
                if(((char *)obj->key)[i] == '\0') break;
                buffer[index++] = ((char *)obj->key)[i++];
            }
            buffer[index++] = '=';
            i=0;
            while (1) {
                if(((char *)obj->value)[i] == '\0') break;
                buffer[index++] = ((char *)obj->value)[i++];
            }
            obj = obj->_next;
            if(!obj)break;
            buffer[index++] = '&';
        }
    }
    
    // reserved text
    strcpy(&buffer[index], " HTTP/1.1\n");
    index += 10;
    
    // header field
    obj = request->headers;
    
    while (obj) {
        // "key: value\n"
        i=0;
        while (1) {
            if(((char *)obj->key)[i] == '\0') break;
            buffer[index++] = ((char *)obj->key)[i++];
        }
        buffer[index++] = ':';
        buffer[index++] = ' ';
        i=0;
        while (1) {
            if(((char *)obj->value)[i] == '\0') break;
            buffer[index++] = ((char *)obj->value)[i++];
        }
        obj = obj->_next;
        buffer[index++] = '\n';
    }

    buffer[index++] = '\n';
    buffer[index++] = '\0';
    
    return buffer;
}

//===============================================
#pragma mark - Free -
//===============================================
void CWebRequestFree(CWebHTTPRequest *request) {
    int ref;
    if(request==NULL)return;
    if(request->ip != NULL && ((int *)request->ip > &ref)) free(request->ip);
    if(request->method != NULL && ((int *)request->method > &ref)) free(request->method);
    if(request->path != NULL && ((int *)request->path > &ref))   CWebObjectFree(request->path);
    if(request->queries != NULL && ((int *)request->queries > &ref))CWebObjectFree(request->queries);
    if(request->headers != NULL && ((int *)request->headers > &ref))CWebObjectFree(request->headers);
    if(request->params != NULL && ((int *)request->params > &ref)) CWebObjectFree(request->params);
    free(request);
}

void CWebResponseFree(CWebHTTPResponse *response) {
    if(response==NULL)return;
    if(response->headers!=NULL) CWebObjectFree(response->headers);
    if(response->body) free(response->body);
    free(response);
}
