//
//  cweb_main.c
//  cweb
//
//  Created by oyabunn on 2014/07/19.
//  Copyright (c) 2014年 yuki-sato. All rights reserved.
//

#include "cweb.h"


#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


int _socket;

int _cweb_main_error_withsocket(int socket, char *error) {
    printf("%s\n", error);
    if(socket>=0){
        int ret = close(socket);
        if(ret<0){
            printf("CWeb_SocketCloseError\n");
            return 0;
        }
    }
    return 1;
}

void SignalHandler(int signalType) {
    printf("\nBye.\n");
    _cweb_main_error_withsocket(_socket, "");
    exit(1);
}

void SiPipeHandler(int signalType) {
    printf("lost connection");
}

int CWebAddSignalObservation(void) {
#ifndef _WIN32
    // Receiving Cntrl + C
    struct sigaction action;
    action.sa_handler = SignalHandler;
    if(sigfillset(&action.sa_mask) < 0){
        return 0;
    }
    action.sa_flags = 0;
    if(sigaction(SIGINT, &action, 0) < 0){
        return 0;
    }
    // Receiving SigPipe
    struct sigaction sigpipeAction;
    sigpipeAction.sa_handler = SiPipeHandler;
    if(sigfillset(&sigpipeAction.sa_mask) < 0){
        return 0;
    }
    sigpipeAction.sa_flags = 0;
    if(sigaction(SIGPIPE, &sigpipeAction, 0) < 0){
        return 0;
    }
#endif
	return 1;
}

int CWebListen(int port) {
    _socket = -1;
    
    int ret = CWebAddSignalObservation();
    if(ret < 1)
        return 0;

#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif

    // open socket
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock < 0)
        return 0;
    
    _socket = sock;
    
    // bind to port
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(port);
    if(bind(sock, (struct sockaddr *) &localAddr, sizeof(localAddr)) < 0) {
        return _cweb_main_error_withsocket(sock, "CWeb binding error");
    }
    
    // start listening
    if(listen(sock, 10) < 0){
        return _cweb_main_error_withsocket(sock, "CWeb listen error");
    }
    
    struct sockaddr_in clientAddr;
    socklen_t length = sizeof(clientAddr);
    while (1) {
        int connectedSock = accept(sock, (struct sockaddr *)&clientAddr, &length);
        if(connectedSock < 0){
            continue;
        }
//        printf("connection from %s\n", inet_ntoa(clientAddr.sin_addr));
        
        // receve data
//#warning TODO separated data
		char data[0xFFF]; // about 4kb
#ifdef _WIN32
		int dataLength;
#else
		ssize_t dataLength;
#endif
		
        dataLength = recv(connectedSock, data, 0xFFF - 1, 0);
        if(dataLength < 0){
            return _cweb_main_error_withsocket(sock, "CWeb receiving data error");
        }
        data[dataLength] = '\0';
//        printf("\n-------request\n");
//        printf("%s", data);
//        printf("\n-------\n");
        
        // formed data
        CWebTCPConnection connection;
        connection.socket = connectedSock;
        connection.client = clientAddr;
        CWebHTTPRequest *request = CWebRequestFromRequestString(data);
        if(request == NULL){
            // invalid request
        }else{
            didReceiveCWebRequest(&connection, request);
        }
        CWebRequestFree(request);
        
#ifdef _WIN32
		closesocket(sock);
		WSACleanup();
#else
		int ret = close(connectedSock);
		if (ret < 0){
			printf("CWeb socket closing error\n");
		}
#endif
    }
    
    // never reach here
    
    return 1;
}

void CWebResponse(CWebTCPConnection *connection, CWebHTTPResponse *response) {
    // invalid response
    if(response==NULL)return;
    // header
    char text[0xFFF];   // 4kb limited header fields
    unsigned long index = 0;
    sprintf(&text[0], "HTTP/1.1 %3d OK\n", response->statusCode);
    index += 16;
    CWebObject *obj = response->headers;
    while (obj != NULL) {
        if(obj->key != NULL){
            memcpy(&text[index], obj->key, strlen(obj->key));
            index += strlen(obj->key);
        }
        text[index++] = ':';
        text[index++] = ' ';
        if(obj->value != NULL){
            memcpy(&text[index], obj->value, strlen(obj->value));
            index += strlen(obj->value);
        }
        text[index++] = '\n';
        obj = obj->_next;
    }
    text[index++] = '\n';
    if(send(connection->socket, text, index, 0) != index){
        printf("CWeb response header error");
    }
    
    // body
    if(send(connection->socket, response->body, strlen(response->body), 0) != strlen(response->body)){
        printf("CWeb response body error");
    }
}

CWebHTTPResponse * CWebRequest(CWebHTTPRequest *request) {
    
    if(request==NULL || request->ip == NULL)
        return NULL;
    
    // Ctrl+C and SIGPIPE handling
    int ret = CWebAddSignalObservation();
    if(ret < 1)
        return NULL;

#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif
    
    // open socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        return NULL;
    
    // destionation
    struct sockaddr_in targetAddr;
    memset(&targetAddr, 0, sizeof(targetAddr));
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_addr.s_addr = inet_addr(request->ip);
    targetAddr.sin_port = htons(request->port);
    
    // connect
    if(connect(sock, (struct sockaddr *) &targetAddr, sizeof(targetAddr)) < 0)
        return NULL;
    
    // send a request
    char *requestString = CWebHTTP_CreateRequestStringFrom(request);
    if(!requestString)
        return NULL;
    unsigned long length = strlen(requestString);
    printf("\nrequest %lu[\n%s\n]", length, requestString);
    if(send(sock, requestString, length, 0) != length){
        printf("CWeb HTTP request error");
    }
    free(requestString);
    
    // receive a response
    char data[0xFFFF];
#ifdef _WIN32
	int dataLength;
#else
    ssize_t dataLength;
#endif
	dataLength = recv(sock, data, 0xFFFF - 1, 0);
    if(dataLength < 0){
        return NULL;
    }
    data[dataLength] = '\0';
    printf("\nresponse[\n%s\n]", data);
    // parse
    CWebHTTPResponse *response = CwebResponseFromResponseString(data, dataLength);

#ifdef _WIN32
	closesocket(sock);
	WSACleanup();
#else
	// cut tcp connection
	if (close(sock) < 0) {
		return NULL;
	}
#endif
    
    return response;
}




