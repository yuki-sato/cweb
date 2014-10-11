//
//  cwe_brender.c
//  cweb
//
//  Created by oyabunn on 2014/07/19.
//  Copyright (c) 2014年 yuki-sato. All rights reserved.
//

#include "cweb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void CWebRenderOverwriteRendering(char *text, unsigned long textLength, CWebObject *params);

/*
 simple replacement
 <%=name%>  => <>
*/
char * CWebRenderHTML(char * filename, CWebObject *params) {
    if(filename==NULL) return NULL;
    
    // file reading
    unsigned long filesize;
    struct stat sb;
    if (stat (filename, & sb) != 0) {
        // not found
        return NULL;
    }
    filesize = sb.st_size;
    FILE *fp;
	if ((fp = fopen(filename, "r")) == NULL) {
		return NULL;
	}
    unsigned long bufferSize = filesize + 1;
    char * text = malloc(sizeof(char) * bufferSize);
    unsigned long readedsize = fread (text, sizeof (char), filesize, fp);
    if(filesize != readedsize){
        free(text);
        return NULL;
    }
    
    // rendering (replacement method)
    CWebRenderOverwriteRendering(text, filesize, params);
    
    // null terminate
    text[bufferSize-1] = '\0';
    
    // closing
	int ret = fclose(fp);
    if(ret != 0){
        // closing error
    }
    
    return text;
}

void CWebRenderOverwriteRendering(char *text, unsigned long textLength, CWebObject *params) {
    if(params == NULL)return;
    unsigned long commandHead = textLength;
    unsigned long commandTail = textLength;
    
    for (unsigned long i = 1; i<textLength; i++) {
        if(text[i] == '\0'){
            return;
        }else if(text[i] == '%' && text[i-1] == '<'){
            commandHead = i-1;
        }else if(commandHead<textLength && text[i] == '>' && text[i-1] == '%'){
            commandTail = i;
            
            // execute command
            if(text[commandHead + 2] == '='){
                // replace tag with params
                char *key = malloc(sizeof(char) * (commandTail - commandHead - 4 + 1));
                memcpy(key, &text[commandHead + 3], (commandTail - commandHead - 4));
                key[commandTail - commandHead - 4] = '\0';
                
#warning TODO not only strings
                char *value = (char *)CWebObjectValueWithKey(params, key);
                memset(&text[commandHead], ' ', commandTail - commandHead + 1);
                memcpy(&text[commandHead], value, strlen(value));
                
                free(key);
                
                
            }else{
                // unknown command
            }
            commandHead = textLength;
            commandHead = textLength;
        }
    }
}