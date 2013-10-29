/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while (charCodes[currentChar] == CHAR_SPACE)
	readChar();
}

void skipComment() {
  // TODO
}

Token* readIdentKeyword(void) {
  int count = 0;
  Token* token = makeToken(TK_IDENT, lineNo, colNo);
  
  while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT) {
    // Add current character to identifier
    token->string[count] = currentChar;
    
    // Increase identifier length
    count++;
    
    // Get next character
    readChar();
  }
  // End string
  token->string[count] = '\0';
  
  // Limit identifier length
  if (count > MAX_IDENT_LEN) {
    // Announce error
    error(ERR_IDENTTOOLONG, lineNo, colNo - count);
  } else {
    // If this identifier is a keyword
    TokenType type = checkKeyword(token->string);
    
    // Otherwise
    if (type != TK_NONE) {
      token->tokenType = type;
    }
  }
  
  return token;
}

Token* readNumber(void) {
  int count = 0;
  Token* token = makeToken(TK_NUMBER, lineNo, colNo);
  
  while (charCodes[currentChar] == CHAR_DIGIT) {
    // Add current character to the number
    token->string[count] = currentChar;
    
    // Increase string index
    count++;
    
    // Read next character
    readChar();
  }
  
  // End string
  token->string[count] = '\0';
  
  // Convert current number to string
  token->value = atoi(token->string);
  
  return token;
}

Token* readConstChar(void) {
  int count = 0;
  Token* token = makeToken(TK_CHAR, lineNo, colNo);
  
  return token;
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS:
    // Token Plus
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_MINUS:
    // Token Minus
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    // Token Times
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    // Token Slash
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;
  case CHAR_EQ:
    // Token Equal
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA:
    // Token Comma
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SEMICOLON:
    // Token Semicolon
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_RPAR:
    // Token Right Parenthesis
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
  case CHAR_LPAR:
    // Empty token
    token = makeToken(TK_NONE, lineNo, colNo);
    // Get next character first
    readChar();
    
    switch(charCodes[currentChar]) {
    case CHAR_PERIOD:
      // This is token LSEL
      token->tokenType = SB_LSEL;
      readChar();
      return token;
    case CHAR_TIMES:
      // This is a comment so free the allocated token first then skip comments
      free(token);
      skipComment();
      return getToken();
    case CHAR_SPACE:
      readChar();
      return getToken();
    default:
      // Token Left Parenthesis
      token->tokenType = SP_LPAR;
      readChar();      
      return token;
    }
  case CHAR_GT:
    // Token Greater
    token = makeToken(SB_GT, lineNo, colNo);
    
    // If next character is '='
    readChar();
    if (charCodes[currentChar] == CHAR_EQ) {
      // Token is Greater Than
      token->tokenType = SB_GE;
      readChar();
    }
    return token;
  case CHAR_EXCLAIMATION:
    // Make empty token
    token = makeToken(TK_NONE, lineNo, colNo);
    
    // If next character is not '='
    readChar();
    if (charCodes[currentChar] != CHAR_EQ) {
      // it is an invalid token
      error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
    } else {
      // else, it's token Not Equal
      token->tokenType = SB_NEQ;
    }
    return token;
  case CHAR_PERIOD:
    // Token Period
    token = makeToken(SB_PERIOD, lineNo, colNo);
    
    // If next character is Right Parenthesis
    readChar();
    if (charCodes[currentChar] == CHAR_RPAR) {
      // it is token Right Parenthesis
      token->tokenType = SB_RPAR;
      readChar();
    }
    return token;
  case CHAR_COLON:
    // Token Semicolon
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    
    // If next character is Equal
    readChar();
    if (charCodes[currentChar] == CHAR_EQ) {
      // it is token Assignment
      token->tokenType = SB_ASSIGN;
      readChar();
    }
    return token;
    // ....
    // TODO
    // ....
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



