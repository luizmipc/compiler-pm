/****************************************************/
/* File: util.h                                     */
/* Utility functions for the P- compiler          */
/****************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

/* Imprime o token e seu lexema */
void printToken( TokenType, const char* );

/* Cria um no de declaracao para construcao da arvore sintatica */
TreeNode * newStmtNode(StmtKind);

/* Cria um no de expressao para construcao da arvore sintatica */
TreeNode * newExpNode(ExpKind);

/* Aloca espaco e faz copia de uma string */
char * copyString( char * );

/* Imprime a arvore sintatica usando indentacao para indicar as subarvores  */
void printTree( TreeNode * );

#endif
