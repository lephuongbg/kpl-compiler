/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab* symtab;
extern Token* currentToken;

static int on_search = 0;

Object* lookupObject(char *name) {
    Object* object = NULL;
    static Scope* currentScope = NULL;

    // Start searching from current scope if
    // we did not search any time before
    // or a global search has been carried out
    if (on_search == 0)
        currentScope = symtab->currentScope;

    // else we start from the scope of previous search
    while (currentScope != NULL) {
        object = findObject(currentScope->objList, name);

        // Jump to the outside scope
        currentScope = currentScope->outer;

        // mark as searching
        on_search = 1;

        // Return the found object
        if (object != NULL)
            return object;
    }

    // Fallback to search in global scope
    object = findObject(symtab->globalObjectList, name);

    // mark as done searching
    on_search = 0;

    return object;
}

void checkFreshIdent(char *name) {
    Object * obj = findObject(symtab->currentScope->objList, name);
    if (obj != NULL)
        error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
}

Object* checkDeclaredIdent(char* name) {
    // Start search
    on_search = 0;

    Object *obj = lookupObject(name);

    // Reset search
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredConstant(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_CONSTANT)
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredType(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_TYPE)
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredVariable(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_VARIABLE)
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredFunction(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_FUNCTION)
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
            error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredProcedure(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_PROCEDURE)
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
            error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredLValueIdent(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && (obj->kind == OBJ_FUNCTION || obj->kind == OBJ_PARAMETER || obj->kind == OBJ_VARIABLE))
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
            error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

    return obj;
}

