/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include "debug.h"

void pad(int n) {
  int i;
  for (i = 0; i < n ; i++) printf(" ");
}

void printType(Type* type) {
  switch (type->typeClass) {
  case TP_INT:
    printf("Int");
    break;
  case TP_CHAR:
    printf("Char");
    break;
  case TP_ARRAY:
    printf("Arr(%d,",type->arraySize);
    printType(type->elementType);
    printf(")");
    break;
  }
}

void printConstantValue(ConstantValue* value) {
  switch (value->type) {
  case TP_INT:
    printf("%d",value->intValue);
    break;
  case TP_CHAR:
    printf("\'%c\'",value->charValue);
    break;
  default:
    break;
  }
}

void printObject(Object* obj, int indent) {
  switch (obj->kind) {
  case OBJ_CONSTANT:
    pad(indent);
    printf("Const %s = ", obj->name);
    printConstantValue(obj->constAttrs->value);
    break;
  case OBJ_TYPE:
    pad(indent);
    printf("Type %s = ", obj->name);
    printType(obj->typeAttrs->actualType);
    break;
  case OBJ_VARIABLE:
    pad(indent);
    printf("Var %s : ", obj->name);
    printType(obj->varAttrs->type);
    break;
  case OBJ_PARAMETER:
    pad(indent);
    if (obj->paramAttrs->kind == PARAM_VALUE) 
      printf("Param %s : ", obj->name);
    else
      printf("Param VAR %s : ", obj->name);
    printType(obj->paramAttrs->type);
    break;
  case OBJ_FUNCTION:
    pad(indent);
    printf("Function %s : ",obj->name);
    printType(obj->funcAttrs->returnType);
    printf("\n");
    printScope(obj->funcAttrs->scope, indent + 4);
    break;
  case OBJ_PROCEDURE:
    pad(indent);
    printf("Procedure %s\n",obj->name);
    printScope(obj->procAttrs->scope, indent + 4);
    break;
  case OBJ_PROGRAM:
    pad(indent);
    printf("Program %s\n",obj->name);
    printScope(obj->progAttrs->scope, indent + 4);
    break;
  }
}

void printObjectList(ObjectNode* objList, int indent) {
  ObjectNode *node = objList;
  while (node != NULL) {
    printObject(node->object, indent);
    printf("\n");
    node = node->next;
  }
}

void printScope(Scope* scope, int indent) {
  printObjectList(scope->objList, indent);
}

