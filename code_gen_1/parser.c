/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "semantics.h"
#include "error.h"
#include "debug.h"

Token *currentToken;
Token *lookAhead;

extern Type* intType;
extern Type* charType;
extern SymTab* symtab;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    //    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  Object* program;

  eat(KW_PROGRAM);
  eat(TK_IDENT);

  program = createProgramObject(currentToken->string);
  enterBlock(program->progAttrs->scope);

  eat(SB_SEMICOLON);

  compileBlock();
  eat(SB_PERIOD);

  exitBlock();
}

void compileConstDecls(void) {
  Object* constObj;
  ConstantValue* constValue;

  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    do {
      eat(TK_IDENT);
      checkFreshIdent(currentToken->string);
      constObj = createConstantObject(currentToken->string);
      declareObject(constObj);
      
      eat(SB_EQ);
      constValue = compileConstant();
      constObj->constAttrs->value = constValue;
      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);
  }
}

void compileTypeDecls(void) {
  Object* typeObj;
  Type* actualType;

  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    do {
      eat(TK_IDENT);
      
      checkFreshIdent(currentToken->string);
      typeObj = createTypeObject(currentToken->string);
      declareObject(typeObj);
      
      eat(SB_EQ);
      actualType = compileType();
      typeObj->typeAttrs->actualType = actualType;
      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);
  } 
}

void compileVarDecls(void) {
  Object* varObj;
  Type* varType;

  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    do {
      eat(TK_IDENT);
      checkFreshIdent(currentToken->string);
      varObj = createVariableObject(currentToken->string);
      eat(SB_COLON);
      varType = compileType();
      varObj->varAttrs->type = varType;
      declareObject(varObj);      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);
  } 
}

void compileBlock(void) {
  compileConstDecls();
  compileTypeDecls();
  compileVarDecls();
  compileSubDecls();

  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileSubDecls(void) {
  while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE)) {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else compileProcDecl();
  }
}

void compileFuncDecl(void) {
  Object* funcObj;
  Type* returnType;

  eat(KW_FUNCTION);
  eat(TK_IDENT);

  checkFreshIdent(currentToken->string);
  funcObj = createFunctionObject(currentToken->string);
  declareObject(funcObj);

  enterBlock(funcObj->funcAttrs->scope);
  
  compileParams();

  eat(SB_COLON);
  returnType = compileBasicType();
  funcObj->funcAttrs->returnType = returnType;

  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);

  exitBlock();
}

void compileProcDecl(void) {
  Object* procObj;

  eat(KW_PROCEDURE);
  eat(TK_IDENT);

  checkFreshIdent(currentToken->string);
  procObj = createProcedureObject(currentToken->string);
  declareObject(procObj);

  enterBlock(procObj->procAttrs->scope);

  compileParams();

  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);

  exitBlock();
}

ConstantValue* compileUnsignedConstant(void) {
  ConstantValue* constValue;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value);
    break;
  case TK_IDENT:
    eat(TK_IDENT);

    obj = checkDeclaredConstant(currentToken->string);
    constValue = duplicateConstantValue(obj->constAttrs->value);

    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

ConstantValue* compileConstant(void) {
  ConstantValue* constValue;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    constValue = compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    constValue = compileConstant2();
    constValue->intValue = - constValue->intValue;
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    constValue = compileConstant2();
    break;
  }
  return constValue;
}

ConstantValue* compileConstant2(void) {
  ConstantValue* constValue;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    obj = checkDeclaredConstant(currentToken->string);
    if (obj->constAttrs->value->type == TP_INT)
      constValue = duplicateConstantValue(obj->constAttrs->value);
    else
      error(ERR_UNDECLARED_INT_CONSTANT,currentToken->lineNo, currentToken->colNo);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

Type* compileType(void) {
  Type* type;
  Type* elementType;
  int arraySize;
  Object* obj;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER);
    type =  makeIntType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  case KW_ARRAY:
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);

    arraySize = currentToken->value;

    eat(SB_RSEL);
    eat(KW_OF);
    elementType = compileType();
    type = makeArrayType(arraySize, elementType);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    obj = checkDeclaredType(currentToken->string);
    type = duplicateType(obj->typeAttrs->actualType);
    break;
  default:
    error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

Type* compileBasicType(void) {
  Type* type;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER); 
    type = makeIntType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  default:
    error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

void compileParams(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    while (lookAhead->tokenType == SB_SEMICOLON) {
      eat(SB_SEMICOLON);
      compileParam();
    }
    eat(SB_RPAR);
  }
}

void compileParam(void) {
  Object* param;
  Type* type;
  enum ParamKind paramKind = PARAM_VALUE;

  if (lookAhead->tokenType == KW_VAR) {
    paramKind = PARAM_REFERENCE;
    eat(KW_VAR);
  }

  eat(TK_IDENT);
  checkFreshIdent(currentToken->string);
  param = createParameterObject(currentToken->string, paramKind);
  eat(SB_COLON);
  type = compileBasicType();
  param->paramAttrs->type = type;
  declareObject(param);
}

void compileStatements(void) {
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
  }
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

Type* compileLValue(void) {
  Object* var;
  Type* varType;

  eat(TK_IDENT);
  
  var = checkDeclaredLValueIdent(currentToken->string);

  switch (var->kind) {
  case OBJ_VARIABLE:
    if (var->varAttrs->type->typeClass == TP_ARRAY) {
      varType = compileIndexes(var->varAttrs->type);
    }
    else
      varType = var->varAttrs->type;
    break;
  case OBJ_PARAMETER:
    varType = var->paramAttrs->type;
    break;
  case OBJ_FUNCTION:
    varType = var->funcAttrs->returnType;
    break;
  default: 
    error(ERR_INVALID_LVALUE,currentToken->lineNo, currentToken->colNo);
  }

  return varType;
}

void compileAssignSt(void) {
  Type* varType;
  Type* expType;

  varType = compileLValue();
  
  eat(SB_ASSIGN);
  expType = compileExpression();
  checkTypeEquality(varType, expType);
}

void compileCallSt(void) {
  Object* proc;

  eat(KW_CALL);
  eat(TK_IDENT);

  proc = checkDeclaredProcedure(currentToken->string);
  compileArguments(proc->procAttrs->paramList);
}

void compileGroupSt(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileIfSt(void) {
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) {
    eat(KW_ELSE);
    compileStatement();
  }
}

void compileWhileSt(void) {
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
}

void compileForSt(void) {
  Type* varType;
  Type *type;

  eat(KW_FOR);

  varType = compileLValue();
  eat(SB_ASSIGN);
  type = compileExpression();
  checkTypeEquality(varType, type);

  eat(KW_TO);
  type = compileExpression();
  checkTypeEquality(varType, type);

  eat(KW_DO);
  compileStatement();

}

void compileArgument(Object* param) {
  Type* type;

  if (param->paramAttrs->kind == PARAM_VALUE) {
    type = compileExpression();
    checkTypeEquality(type, param->paramAttrs->type);
  } else {
    type = compileLValue();
    checkTypeEquality(type, param->paramAttrs->type);
  }
}

void compileArguments(ObjectNode* paramList) {
  ObjectNode* node = paramList;

  switch (lookAhead->tokenType) {
  case SB_LPAR:
    eat(SB_LPAR);
    if (node == NULL)
      error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    compileArgument(node->object);
    node = node->next;

    while (lookAhead->tokenType == SB_COMMA) {
      eat(SB_COMMA);
      if (node == NULL)
	error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
      compileArgument(node->object);
      node = node->next;
    }

    if (node != NULL)
      error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    
    eat(SB_RPAR);
    break;
    // Check FOLLOW set 
  case SB_TIMES:
  case SB_SLASH:
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void) {
  Type* type1;
  Type* type2;
  TokenType op;

  type1 = compileExpression();
  checkBasicType(type1);

  op = lookAhead->tokenType;
  switch (op) {
  case SB_EQ:
    eat(SB_EQ);
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    break;
  case SB_LE:
    eat(SB_LE);
    break;
  case SB_LT:
    eat(SB_LT);
    break;
  case SB_GE:
    eat(SB_GE);
    break;
  case SB_GT:
    eat(SB_GT);
    break;
  default:
    error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }

  type2 = compileExpression();
  checkTypeEquality(type1,type2);
}

Type* compileExpression(void) {
  Type* type;
  
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    type = compileExpression2();
    checkIntType(type);
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    type = compileExpression2();
    checkIntType(type);
    break;
  default:
    type = compileExpression2();
  }
  return type;
}

Type* compileExpression2(void) {
  Type* type;

  type = compileTerm();
  type = compileExpression3(type);

  return type;
}


Type* compileExpression3(Type* argType1) {
  Type* argType2;
  Type* resultType;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    checkIntType(argType1);
    argType2 = compileTerm();
    checkIntType(argType2);
    resultType = compileExpression3(argType1);
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    checkIntType(argType1);
    argType2 = compileTerm();
    checkIntType(argType2);
    resultType = compileExpression3(argType1);
    break;
    // check the FOLLOW set
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    resultType = argType1;
    break;
  default:
    error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
  return resultType;
}

Type* compileTerm(void) {
  Type* type;
  type = compileFactor();
  type = compileTerm2(type);

  return type;
}

Type* compileTerm2(Type* argType1) {
  Type* argType2;
  Type* resultType;

  switch (lookAhead->tokenType) {
  case SB_TIMES:
    eat(SB_TIMES);
    checkIntType(argType1);
    argType2 = compileFactor();
    checkIntType(argType2);
    resultType = compileTerm2(argType1);
    break;
  case SB_SLASH:
    eat(SB_SLASH);
    checkIntType(argType1);
    argType2 = compileFactor();
    checkIntType(argType2);
    resultType = compileTerm2(argType1);
    break;
    // check the FOLLOW set
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    resultType = argType1;
    break;
  default:
    error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
  }
  return resultType;
}

Type* compileFactor(void) {
  Type* type;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    type = intType;
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    type = charType;
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    obj = checkDeclaredIdent(currentToken->string);

    switch (obj->kind) {
    case OBJ_CONSTANT:
      switch (obj->constAttrs->value->type) {
      case TP_INT:
	type = intType;
	break;
      case TP_CHAR:
	type = charType;
	break;
      default:
	break;
      }
      break;
    case OBJ_VARIABLE:
      if (obj->varAttrs->type->typeClass == TP_ARRAY) {
	type = compileIndexes(obj->varAttrs->type);
      } else {
	type = obj->varAttrs->type;
      }
      break;
    case OBJ_PARAMETER:
      type = obj->paramAttrs->type;
      break;
    case OBJ_FUNCTION:
      compileArguments(obj->funcAttrs->paramList);
      type = obj->funcAttrs->returnType;
      break;
    default: 
      error(ERR_INVALID_FACTOR,currentToken->lineNo, currentToken->colNo);
      break;
    }
    break;
  default:
    error(ERR_INVALID_FACTOR, lookAhead->lineNo, lookAhead->colNo);
  }
  
  return type;
}

Type* compileIndexes(Type* arrayType) {
  Type* type;

  
  while (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);
    type = compileExpression();
    checkIntType(type);
    checkArrayType(arrayType);

    arrayType = arrayType->elementType;
    eat(SB_RSEL);
  }
  checkBasicType(arrayType);
  return arrayType;
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  initSymTab();

  compileProgram();

  printObject(symtab->program,0);

  cleanSymTab();
  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
