//
//  cweb_object.c
//  cweb
//
//  Created by oyabunn on 2014/07/19.
//  Copyright (c) 2014年 yuki-sato. All rights reserved.
//

#include "cweb_object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//===============================================
#pragma mark --- MemoryRelateds
//===============================================
CWebObject * CWebObjectCreate(void) {
    CWebObject *object = (CWebObject *)malloc(sizeof(CWebObject));
    if(object == NULL){
        // memory error
        return NULL;
    }
    object->key = NULL;
    object->value = NULL;
    object->_next = NULL;
    return object;
}

CWebObject * CWebObjectCreateDictionary(char *key, void *value, CWebObjectType valueType) {
    if(key == NULL)return NULL;
    CWebObject * object = (CWebObject *)malloc(sizeof(CWebObject));
    if(object == NULL){
        // memory error
        return NULL;
    }
    object->key = key;
    object->value = value;
    object->type = valueType;
    object->_next = NULL;
    return object;
}

CWebObject * CWebObjectCreateArray(void *value, CWebObjectType valueType) {
    CWebObject * object = (CWebObject *)malloc(sizeof(CWebObject));
    if(object == NULL){
        // memory error
        return NULL;
    }
    object->key = NULL;
    object->value = value;
    object->type = valueType;
    object->_next = NULL;
    return object;
}

void CWebObjectFree(CWebObject *object) {
    if(object->key != NULL)free(object->key);
    if(object->value != NULL)free(object->value);
    if(object->_next != NULL)CWebObjectFree(object->_next);
    free(object);
}

//===============================================
#pragma mark --- Add
//===============================================
int CWebObjectAddObjectToDictionary(CWebObject *object, char *key, void *value, CWebObjectType valueType) {
    if(object==NULL || key==NULL)return 0;
    CWebObject * newobject = (CWebObject *)malloc(sizeof(CWebObject));
    if(newobject == NULL){
        // memory error
        return 0;
    }
    newobject->key = key;
    newobject->value = value;
    object->type = valueType;
    newobject->_next = NULL;
    
    CWebObject *target = object;
    unsigned int i=0;
    for (; i<CWebObjectMaxChainLength; i++) {
        if(target->_next == NULL){
            target->_next = newobject;
            return 1;
        }
        target = target->_next;
    }
    
    return 0;
}

int CWebObjectAddObjectToArray(CWebObject *object, void *value, CWebObjectType valueType) {
    if(object==NULL)return 0;
    CWebObject * newobject = (CWebObject *)malloc(sizeof(CWebObject));
    if(newobject == NULL){
        // memory error
        return 0;
    }
    newobject->key = NULL;
    newobject->value = value;
    object->type = valueType;
    newobject->_next = NULL;
    
    CWebObject *target = object;
    unsigned int i=0;
    for (; i<CWebObjectMaxChainLength; i++) {
        if(target->_next == NULL){
            target->_next = newobject;
            return 1;
        }
        target = target->_next;
    }
    return 1;
}

//===============================================
#pragma mark --- Remove
//===============================================
int CWebObjectRemoveObject(CWebObject **object, CWebObject *targetObject) {
    if(*object == NULL)return 0;
    
    CWebObject *lastObject = NULL;
    CWebObject *obj = *object;
    unsigned int i=0;
    for (; i<CWebObjectMaxChainLength; i++) {
        if(obj == targetObject){
            if(lastObject == NULL){
                *object = obj->_next;
            }else{
                lastObject->_next = obj->_next;
            }
            obj->_next = NULL;
            CWebObjectFree(obj);
            return 1;
        }
        if(obj->_next == NULL)break;
        lastObject = obj;
        obj = obj->_next;
    }
    
    return 0;
}

int CWebObjectRemoveObjectAtIndex(CWebObject **object, int index) {
    if(*object == NULL)return 0;
    
    CWebObject *lastObject = NULL;
    CWebObject *obj = *object;
    unsigned int i=0;
    for (; i<CWebObjectMaxChainLength; i++) {
        if(i==index){
            if(lastObject == NULL){
                *object = obj->_next;
            }else{
                lastObject->_next = obj->_next;
            }
            obj->_next = NULL;
            CWebObjectFree(obj);
            break;
        }
        if(obj->_next == NULL)break;
        lastObject = obj;
        obj = obj->_next;
    }
    
    return 0;
}

int CWebObjectRemoveObjectWithKey(CWebObject **object, char *key) {
    if(*object == NULL)return 0;
    if((*object)->key == NULL)return 0;
    
    CWebObject *lastObject = NULL;
    CWebObject *obj = *object;
    unsigned int i=0;
    for (; i<CWebObjectMaxChainLength; i++) {
        if(strcmp(obj->key, key) == 0){
            if(lastObject == NULL){
                *object = obj->_next;
            }else{
                lastObject->_next = obj->_next;
            }
            obj->_next = NULL;
            CWebObjectFree(obj);
            break;
        }
        if(obj->_next == NULL)break;
        lastObject = obj;
        obj = obj->_next;
    }
    
    return 0;
}

//===============================================
#pragma mark - DicArray -
//===============================================
void * CWebObjectValueWithKey(CWebObject *object, char *key) {
    if(object == NULL)return NULL;
    
    CWebObject *obj = object;
    
    for (unsigned int i=0; i<CWebObjectMaxChainLength; i++) {
        if(strcmp(obj->key, key) == 0){
            return obj->value;
        }
        if(obj->_next == NULL)break;
        obj = obj->_next;
    }
    
    return NULL;

}

void * CWebObjectValueAtIndex(CWebObject *object, int index) {
    if(object == NULL)return NULL;
    
    CWebObject *obj = object;
    for (unsigned int i=0; i<CWebObjectMaxChainLength; i++) {
        if(i==index){
            return obj->value;
        }
        if(obj->_next == NULL)break;
        obj = obj->_next;
    }
    
    return NULL;
}


//===============================================
#pragma mark - Utils -
//===============================================
CWebObject * CWebObjectCreateDictionaryStringValueWithCopy(char *key, char *value) {
    if(key==NULL || value==NULL)return 0;
    
    char *_key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(_key, key);
    char *_value = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(_value, value);
    
    CWebObject *object = CWebObjectCreate();
    object->type = CWebObjectTypeString;
    object->key = _key;
    object->value = _value;
    
    return object;
}

int CWebObjectAddStringObjectWithCopyToDictionary(CWebObject *object, char *key, char *value) {
    if(object==NULL || key==NULL || value==NULL)return 0;
    
    char *_key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(_key, key);
    char *_value = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(_value, value);
    
    return CWebObjectAddObjectToDictionary(object, _key, _value, CWebObjectTypeString);
}

void CWebObjectPrint(CWebObject *object) {
    CWebObject *target = object;
    while (target!=NULL) {
        printf("%s:%s\n",target->key,target->value);
        target = target->_next;
    }
}


