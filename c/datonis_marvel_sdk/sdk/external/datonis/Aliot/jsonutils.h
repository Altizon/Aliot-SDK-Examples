/*
 * jsonutils.h
 *
 *  Created on: 06-Feb-2014
 *      Author: ranjit
 */

#ifndef JSONUTILS_H_
#define JSONUTILS_H_ 

void startJSON(char *buffer);
void endJSON(char *buffer);


void putJSONString(char *buffer, char *key, char *value);

void putJSONStringAndComma(char *buffer, char *key, char *value);


void putJSONDouble(char *buffer, char *key, double value);

void putJSONDoubleAndComma(char *buffer, char *key, double value);

void putJSONInt(char *buff, char *key, int value);

void putJSONIntAndComma(char *buff, char *key, int value);

void putJSONBoolean(char *buff, char *key, int value);

void putJSONBooleanAndComma(char *buff, char * key, int value);


#endif /* JSONUTILS_H_ */
