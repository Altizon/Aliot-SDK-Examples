/*
 * jsonutils.c
 *
 *  Created on: 06-Feb-2014
 *      Author: ranjit
 */

#include <stdio.h>
#include <string.h>


void startJSON(char *buffer) {
	strcpy(buffer, "{");
}

void endJSON(char *buffer) {
	strcat(buffer, "}");
}


void putJSONString(char *buffer, char *key, char *value) {
	strcat(buffer, "\"");
	strcat(buffer, key);
	strcat(buffer, "\":\"");
	strcat(buffer, value);
	strcat(buffer, "\"");
}


void putJSONStringAndComma(char *buffer, char *key, char *value) {
	putJSONString(buffer, key, value);
	strcat(buffer, ",");
}


void putJSONDouble(char *buffer, char *key, double value) {

	char doublestr[1024];
	memset(doublestr, 1024, '\0');
	sprintf(doublestr, "\"%s\":%0.0f", key, value);
	strcat(buffer, doublestr);
}


void putJSONDoubleAndComma(char *buffer, char *key, double value) {
	putJSONDouble(buffer, key, value);
	strcat(buffer, ",");
}


void putJSONInt(char *buff, char *key, int value) {
    char str[1024];
    sprintf(str, "\"%s\":%d", key, value);
    strcat(buff, str);
}

void putJSONIntAndComma(char *buff, char *key, int value) {
    putJSONInt(buff, key, value);
    strcat(buff, ",");
}

void putJSONBoolean(char *buff, char *key, int value) {
    strcat(buff, "\"");
    strcat(buff, key);
    strcat(buff, "\":");
    if (value) {
        strcat(buff, "true");
    } else {
        strcat(buff, "false");
    }
}

void putJSONBooleanAndComma(char *buff, char *key, int value) {
    putJSONBoolean(buff, key, value);
    strcat(buff, ",");
}
