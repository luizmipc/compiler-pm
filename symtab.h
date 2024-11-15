/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* Insere numeros de linha e localizacao de memoria na tabela de simbolos.
   loc = localizacao de memoria. Inserida apenas na primeira chamada.      */
void st_insert(char * name, int lineno, int loc, ExpType expType );

/* Retorna a posicao da varivel na memoria ou -1 se nao encontrada */
int st_lookup ( char * name );

/* Mostra uma listagem formatada do conteudo da tabela de simbolos */
void printSymTab(FILE * listing);

#endif
