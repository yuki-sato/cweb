//
//  cweb_object.h
//  cweb
//
//  Created by oyabunn on 2014/07/19.
//  Copyright (c) 2014年 yuki-sato. All rights reserved.
//

#ifndef cweb_cweb_object_h
#define cweb_cweb_object_h

#define CWebObjectMaxChainLength (0x7FFE)

// indicate what *object is.
typedef enum _CWebObjectType {
    CWebObjectTypeNull = 0,
    CWebObjectTypeString,
    CWebObjectTypeDeciminal,
    CWebObjectTypeFloat,
    CWebObjectTypeBoolean,
    CWebObjectTypeObject            // CWebObject it self
}CWebObjectType;

typedef struct _CWebObject {
    char    *key;               // dictionary [key:"abc" value:"abc" _next:]    array [key:null  value:"abc" _next:]
    void    *value;
    CWebObjectType  type;
    
    struct _CWebObject   *_next;
}CWebObject;


// creation
CWebObject * CWebObjectCreate(void);
CWebObject * CWebObjectCreateDictionary(char *key, void *value, CWebObjectType valueType);
CWebObject * CWebObjectCreateArray(void *value, CWebObjectType valueType);

// free
void CWebObjectFree(CWebObject *object);

// add
int CWebObjectAddObjectToDictionary(CWebObject *object, char *key, void *value, CWebObjectType valueType);
int CWebObjectAddObjectToArray(CWebObject *object, void *value, CWebObjectType valueType);

// remove
int CWebObjectRemoveObject(CWebObject **object, CWebObject *targetObject);
int CWebObjectRemoveObjectAtIndex(CWebObject **object, int index);
int CWebObjectRemoveObjectWithKey(CWebObject **object, char *key);

// dicarray
void * CWebObjectValueWithKey(CWebObject *object, char *key);
void * CWebObjectValueAtIndex(CWebObject *object, int index);

// utils
CWebObject * CWebObjectCreateDictionaryStringValueWithCopy(char *key, char *value);
int CWebObjectAddStringObjectWithCopyToDictionary(CWebObject *object, char *key, char *value);

void CWebObjectPrint(CWebObject *object);


#endif
