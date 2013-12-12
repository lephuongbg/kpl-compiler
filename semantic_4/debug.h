/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __DEBUG_H__
#define __DEBUG_H_

#include "symtab.h"

void printType(Type* type);
void printConstantValue(ConstantValue* value);
void printObject(Object* obj, int indent);
void printObjectList(ObjectNode* objList, int indent);
void printScope(Scope* scope, int indent);

#endif
