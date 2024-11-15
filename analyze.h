/****************************************************/
/* File: analyze.h                                  */
/* Semantic analyzer interface for TINY compiler    */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

/* Constroi a tabela de simbolos varrendo a arvore sintatica em pre-ordem */
void buildSymtab(TreeNode *);

/* Faz a checagem de tipo varrendo a arvore sintatica em pos-ordem */
void typeCheck(TreeNode *);

#endif
