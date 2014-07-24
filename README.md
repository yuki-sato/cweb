Cweb
====

Simple and Fast Web Framework for C lang



====
How to use

This is just sources which have useful codes to treat http request and response through socket.
So you need only

1. include cweb.h
2. just compile and link .c files except for example.c

====
hello world

I made example.c and simple makefile.
If you want to just test how work this, try above.

1. open shell and move to this directory
2. $ make
3. $ ./example
4. open http://127.0.0.1:8080/ in a browser

====
example main.c

    #include "cweb.h"

    #include <string.h>
    #include <stdio.h>

    int main(int argc, char *argv[])
    {
        CWebListen(8080);
        return 1;
    }
    
    void didReceiveCWebRequest(CWebTCPConnection *connection, CWebHTTPRequest *request)
    {
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


