/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"

/* SIZE -> Tamanho da tabela hash */
#define SIZE 211

/* SHIFT -> Valor usado como multiplicador para o calculo do hash */
#define SHIFT 4

/* Funcao de hash */
static int hash (char *key) {
  int temp = 0;
  int i = 0;
  while (key[i] != '\0') {
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* Lista encadeada dos numeros de linha do codigo fonte onde a variavel eh referenciada */
typedef struct LineListRec {
  int lineno;
  struct LineListRec *next;
} *LineList;

/* Registro das informacoes das variaveis a serem
   armazenadas na tabela de simbolos.
   Inclui nome, localizacao de memoria e lista de
   numeros de linha em que a variavel aparece no codigo fonte. */
typedef struct BucketListRec {
  char *name;
  LineList lines;
  int memloc ; /* Localizacao de memoria da variavel. */
  struct BucketListRec *next;
  ExpType type;
} *BucketList;
/* Tabela hash */
static BucketList hashTable[SIZE];

/* Insere numeros de linha e localizacao de memoria na tabela de simbolos.
   loc = localizacao de memoria. Inserida apenas na primeira chamada.      */
void st_insert( char * name, int lineno, int loc, ExpType expType ) {
  int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) { /* Variavel ainda nao esta na tabela de simbolos */
    l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->type = expType;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = hashTable[h];
    hashTable[h] = l;
  }
  else { /* Variavel encontrada na tabela de simbolos. Apenas acrescenta numero de linha. */
    LineList t = l->lines;
    while (t->next != NULL)
      t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

/* Retorna a posicao da varivel na memoria ou -1 se nao encontrada */
int st_lookup ( char * name ) {
  int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL)
    return -1;
  else
    return l->memloc;
}

/* Mostra uma listagem formatada do conteudo da tabela de simbolos */
void printSymTab(FILE * listing) {
  int i;
  fprintf(listing,"Variable Name  Location   Type   Line Numbers\n");
  fprintf(listing,"-------------  --------   ----     ------------\n");
  for (i=0; i<SIZE; ++i) {
    if (hashTable[i] != NULL) {
      BucketList l = hashTable[i];
      while (l != NULL) {
        LineList t = l->lines;
        fprintf(listing,"%-14s ", l->name);
        fprintf(listing,"%-8d  ", l->memloc);
        if (l->type == Real) {
          fprintf(listing, "%-4s", "real");
        } else if (l->type == Integer) {
          fprintf(listing, "%-4s", "inteiro");
        }
        while (t != NULL) {
          fprintf(listing,"%4d ", t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */
