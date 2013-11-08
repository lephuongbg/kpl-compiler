/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#ifndef __PARSER_H__
#define __PARSER_H__
#include "token.h"

void scan(void);
void eat(TokenType tokenType);

void compileProgram(void);
void compileBlock(void);
void compileBlock2(void);
void compileBlock3(void);
void compileBlock4(void);
void compileBlock5(void);
void compileConstDecls(void);
void compileConstDecl(void);
void compileTypeDecls(void);
void compileTypeDecl(void);
void compileVarDecls(void);
void compileVarDecl(void);
void compileSubDecls(void);
void compileFuncDecl(void);
void compileProcDecl(void);
void compileUnsignedConstant(void);
void compileConstant(void);
void compileConstant2(void);
void compileType(void);
void compileBasicType(void);
void compileParams(void);
void compileParams2(void);
void compileParam(void);
void compileStatements(void);
void compileStatements2(void);
void compileStatement(void);
void compileAssignSt(void);
void compileCallSt(void);
void compileGroupSt(void);
void compileIfSt(void);
void compileElseSt(void);
void compileWhileSt(void);
void compileForSt(void);
void compileArguments(void);
void compileArguments2(void);
void compileCondition(void);
void compileCondition2(void);
void compileExpression(void);
void compileExpression2(void);
void compileExpression3(void);
void compileTerm(void);
void compileTerm2(void);
void compileFactor(void);
void compileIndexes(void);

int compile(char *fileName);

#endif
