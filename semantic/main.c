/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "symtab.h"
#include "debug.h"

extern SymTab* symtab;
/******************************************************************/

int main() {
  Object* obj;

  initSymTab();

  obj = createProgramObject("PRG");
  enterBlock(obj->progAttrs->scope);

  obj = createConstantObject("c1");
  obj->constAttrs->value = makeIntConstant(10);
  declareObject(obj);

  obj = createConstantObject("c2");
  obj->constAttrs->value = makeCharConstant('a');
  declareObject(obj);

  obj = createTypeObject("t1");
  obj->typeAttrs->actualType = makeArrayType(10,makeIntType());
  declareObject(obj);

  obj = createVariableObject("v1");
  obj->varAttrs->type = makeIntType();
  declareObject(obj);

  obj = createVariableObject("v2");
  obj->varAttrs->type = makeArrayType(10,makeArrayType(10,makeIntType()));
  declareObject(obj);

  obj = createFunctionObject("f");
  obj->funcAttrs->returnType = makeIntType();
  declareObject(obj);
  
    enterBlock(obj->funcAttrs->scope);
 
    obj = createParameterObject("p1", PARAM_VALUE, symtab->currentScope->owner);
    obj->paramAttrs->type = makeIntType();
    declareObject(obj);

    obj = createParameterObject("p2", PARAM_REFERENCE, symtab->currentScope->owner);
    obj->paramAttrs->type = makeCharType();
    declareObject(obj);

    exitBlock();

  obj = createProcedureObject("p");
  declareObject(obj);
  
    enterBlock(obj->procAttrs->scope);
 
    obj = createParameterObject("v1", PARAM_VALUE, symtab->currentScope->owner);
    obj->paramAttrs->type = makeIntType();
    declareObject(obj);

    obj = createConstantObject("c1");
    obj->constAttrs->value = makeCharConstant('a');
    declareObject(obj);
    
    obj = createConstantObject("c3");
    obj->constAttrs->value = makeIntConstant(10);
    declareObject(obj);

    obj = createTypeObject("t1");
    obj->typeAttrs->actualType = makeIntType();
    declareObject(obj);

    obj = createTypeObject("t2");
    obj->typeAttrs->actualType = makeArrayType(10,makeIntType());
    declareObject(obj);

    obj = createVariableObject("v2");
    obj->varAttrs->type = makeArrayType(10,makeIntType());
    declareObject(obj);

    obj = createVariableObject("v3");
    obj->varAttrs->type = makeCharType();
    declareObject(obj);

    exitBlock();


  exitBlock();
  printObject(symtab->program,0);
  cleanSymTab();
    
  return 0;
}
