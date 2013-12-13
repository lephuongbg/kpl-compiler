/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "semantics.h"
#include "error.h"

extern SymTab* symtab;
extern Token* currentToken;

Object* lookupObject(char *name) {
  Scope* scope = symtab->currentScope;
  Object* obj;

  while (scope != NULL) {
    obj = findObject(scope->objList, name);
    if (obj != NULL) return obj;
    scope = scope->outer;
  }
  obj = findObject(symtab->globalObjectList, name);
  if (obj != NULL) return obj;
  return NULL;
}

void checkFreshIdent(char *name) {
  if (findObject(symtab->currentScope->objList, name) != NULL)
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
}

Object* checkDeclaredIdent(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_IDENT,currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

Object* checkDeclaredConstant(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_CONSTANT,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_CONSTANT)
    error(ERR_INVALID_CONSTANT,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredType(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_TYPE,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_TYPE)
    error(ERR_INVALID_TYPE,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredVariable(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_VARIABLE,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_VARIABLE)
    error(ERR_INVALID_VARIABLE,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredFunction(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_FUNCTION,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_FUNCTION)
    error(ERR_INVALID_FUNCTION,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredProcedure(char* name) {
  Object* obj = lookupObject(name);
  if (obj == NULL) 
    error(ERR_UNDECLARED_PROCEDURE,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_PROCEDURE)
    error(ERR_INVALID_PROCEDURE,currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object* checkDeclaredLValueIdent(char* name) {
  Object* obj = lookupObject(name);
  Scope* scope;

  if (obj == NULL)
    error(ERR_UNDECLARED_IDENT,currentToken->lineNo, currentToken->colNo);

  switch (obj->kind) {
  case OBJ_VARIABLE:
  case OBJ_PARAMETER:
    break;
  case OBJ_FUNCTION:
    scope = symtab->currentScope;
    while ((scope != NULL) && (scope != obj->funcAttrs->scope)) 
      scope = scope->outer;

    if (scope == NULL)
      error(ERR_INVALID_IDENT,currentToken->lineNo, currentToken->colNo);
    break;
  default:
    error(ERR_INVALID_IDENT,currentToken->lineNo, currentToken->colNo);
  }

  return obj;
}


void checkIntType(Type* type) {
  if ((type != NULL) && (type->typeClass == TP_INT))
    return;
  else error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkCharType(Type* type) {
  if ((type != NULL) && (type->typeClass == TP_CHAR))
    return;
  else error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkBasicType(Type* type) {
  if ((type != NULL) && ((type->typeClass == TP_INT) || (type->typeClass == TP_CHAR)))
    return;
  else error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkArrayType(Type* type) {
  if ((type != NULL) && (type->typeClass == TP_ARRAY))
    return;
  else error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkTypeEquality(Type* type1, Type* type2) {
  if (compareType(type1, type2) == 0)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}


