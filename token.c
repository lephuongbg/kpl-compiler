/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <ctype.h>
#include "token.h"

struct {
  char string[MAX_IDENT_LEN + 1];
  TokenType tokenType;
} keywords[KEYWORDS_COUNT] = {
  {"PROGRAM", KW_PROGRAM},
  {"CONST", KW_CONST},
  {"TYPE", KW_TYPE},
  {"VAR", KW_VAR},
  {"INTEGER", KW_INTEGER},
  {"CHAR", KW_CHAR},
  {"ARRAY", KW_ARRAY},
  {"OF", KW_OF},
  {"FUNCTION", KW_FUNCTION},
  {"PROCEDURE", KW_PROCEDURE},
  {"BEGIN", KW_BEGIN},
  {"END", KW_END},
  {"CALL", KW_CALL},
  {"IF", KW_IF},
  {"THEN", KW_THEN},
  {"ELSE", KW_ELSE},
  {"WHILE", KW_WHILE},
  {"DO", KW_DO},
  {"FOR", KW_FOR},
  {"TO", KW_TO}
};

int keywordEq(char *kw, char *string) {
  while ((*kw != '\0') && (*string != '\0')) {
    if (*kw != toupper(*string)) break;
    kw ++; string ++;
  }
  return ((*kw == '\0') && (*string == '\0'));
}

TokenType checkKeyword(char *string) {
  int i;
  for (i = 0; i < KEYWORDS_COUNT; i++)
    if (keywordEq(keywords[i].string, string)) 
      return keywords[i].tokenType;
  return TK_NONE;
}

Token* makeToken(TokenType tokenType, int lineNo, int colNo) {
  Token *token = (Token*)malloc(sizeof(Token));
  token->tokenType = tokenType;
  token->lineNo = lineNo;
  token->colNo = colNo;
  return token;
}
