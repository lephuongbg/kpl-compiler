/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "error.h"
#include "codegen.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;
Object* writeiProcedure;
Object* writecProcedure;
Object* writelnProcedure;
Object* readiFunction;
Object* readcFunction;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type* duplicateType(Type* type) {
  Type* resultType = (Type*) malloc(sizeof(Type));
  resultType->typeClass = type->typeClass;
  if (type->typeClass == TP_ARRAY) {
    resultType->arraySize = type->arraySize;
    resultType->elementType = duplicateType(type->elementType);
  }
  return resultType;
}

int compareType(Type* type1, Type* type2) {
  if (type1->typeClass == type2->typeClass) {
    if (type1->typeClass == TP_ARRAY) {
      if (type1->arraySize == type2->arraySize)
	return compareType(type1->elementType, type2->elementType);
      else return 0;
    } else return 1;
  } else return 0;
}

void freeType(Type* type) {
  switch (type->typeClass) {
  case TP_INT:
  case TP_CHAR:
    free(type);
    break;
  case TP_ARRAY:
    freeType(type->elementType);
    freeType(type);
    break;
  }
}

int sizeOfType(Type* type) {
  // TODO
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
  ConstantValue* value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = TP_INT;
  value->intValue = i;
  return value;
}

ConstantValue* makeCharConstant(char ch) {
  ConstantValue* value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = TP_CHAR;
  value->charValue = ch;
  return value;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  ConstantValue* value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = v->type;
  if (v->type == TP_INT) 
    value->intValue = v->intValue;
  else
    value->charValue = v->charValue;
  return value;
}

/******************* Object utilities ******************************/

Scope* createScope(Object* owner) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = NULL;
  scope->frameSize = RESERVED_WORDS;
  return scope;
}

Object* createProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program);
  program->progAttrs->codeAddress = DC_VALUE;
  symtab->program = program;

  return program;
}

Object* createConstantObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_CONSTANT;
  obj->constAttrs = (ConstantAttributes*) malloc(sizeof(ConstantAttributes));
  return obj;
}

Object* createTypeObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_TYPE;
  obj->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
  return obj;
}

Object* createVariableObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_VARIABLE;
  obj->varAttrs = (VariableAttributes*) malloc(sizeof(VariableAttributes));
  obj->varAttrs->type = NULL;
  obj->varAttrs->scope = NULL;
  obj->varAttrs->localOffset = 0;
  return obj;
}

Object* createFunctionObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_FUNCTION;
  obj->funcAttrs = (FunctionAttributes*) malloc(sizeof(FunctionAttributes));
  obj->funcAttrs->returnType = NULL;
  obj->funcAttrs->paramList = NULL;
  obj->funcAttrs->paramCount = 0;
  obj->funcAttrs->codeAddress = DC_VALUE;
  obj->funcAttrs->scope = createScope(obj);
  return obj;
}

Object* createProcedureObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_PROCEDURE;
  obj->procAttrs = (ProcedureAttributes*) malloc(sizeof(ProcedureAttributes));
  obj->procAttrs->paramList = NULL;
  obj->procAttrs->paramCount = 0;
  obj->procAttrs->codeAddress = DC_VALUE;
  obj->procAttrs->scope = createScope(obj);
  return obj;
}

Object* createParameterObject(char *name, enum ParamKind kind) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_PARAMETER;
  obj->paramAttrs = (ParameterAttributes*) malloc(sizeof(ParameterAttributes));
  obj->paramAttrs->kind = kind;
  obj->paramAttrs->type = NULL;
  obj->paramAttrs->scope = NULL;
  obj->paramAttrs->localOffset = 0;
  return obj;
}

void freeObject(Object* obj) {
  switch (obj->kind) {
  case OBJ_CONSTANT:
    free(obj->constAttrs->value);
    free(obj->constAttrs);
    break;
  case OBJ_TYPE:
    free(obj->typeAttrs->actualType);
    free(obj->typeAttrs);
    break;
  case OBJ_VARIABLE:
    free(obj->varAttrs->type);
    free(obj->varAttrs);
    break;
  case OBJ_FUNCTION:
    freeReferenceList(obj->funcAttrs->paramList);
    freeType(obj->funcAttrs->returnType);
    freeScope(obj->funcAttrs->scope);
    free(obj->funcAttrs);
    break;
  case OBJ_PROCEDURE:
    freeReferenceList(obj->procAttrs->paramList);
    freeScope(obj->procAttrs->scope);
    free(obj->procAttrs);
    break;
  case OBJ_PROGRAM:
    freeScope(obj->progAttrs->scope);
    free(obj->progAttrs);
    break;
  case OBJ_PARAMETER:
    freeType(obj->paramAttrs->type);
    free(obj->paramAttrs);
  }
  free(obj);
}

void freeScope(Scope* scope) {
  freeObjectList(scope->objList);
  free(scope);
}

void freeObjectList(ObjectNode *objList) {
  ObjectNode* list = objList;

  while (list != NULL) {
    ObjectNode* node = list;
    list = list->next;
    freeObject(node->object);
    free(node);
  }
}

void freeReferenceList(ObjectNode *objList) {
  ObjectNode* list = objList;

  while (list != NULL) {
    ObjectNode* node = list;
    list = list->next;
    free(node);
  }
}

void addObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  while (objList != NULL) {
    if (strcmp(objList->object->name, name) == 0) 
      return objList->object;
    else objList = objList->next;
  }
  return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  symtab->program = NULL;
  symtab->currentScope = NULL;
  
  readcFunction = createFunctionObject("READC");
  declareObject(readcFunction);
  readcFunction->funcAttrs->returnType = makeCharType();

  readiFunction = createFunctionObject("READI");
  declareObject(readiFunction);
  readiFunction->funcAttrs->returnType = makeIntType();


  writeiProcedure = createProcedureObject("WRITEI");
  declareObject(writeiProcedure);
  enterBlock(writeiProcedure->procAttrs->scope);
    param = createParameterObject("i", PARAM_VALUE);
    param->paramAttrs->type = makeIntType();
    declareObject(param);
  exitBlock();

  writecProcedure = createProcedureObject("WRITEC");
  declareObject(writecProcedure);
  enterBlock(writecProcedure->procAttrs->scope);
    param = createParameterObject("ch", PARAM_VALUE);
    param->paramAttrs->type = makeCharType();
    declareObject(param);
  exitBlock();

  writelnProcedure = createProcedureObject("WRITELN");
  declareObject(writelnProcedure);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  // TODO: rewrite the function to fill all values of attributes
}


