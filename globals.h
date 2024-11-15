/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for P- compiler          */
/* must come before other include files             */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = quantidade máxima de palavras reservadas */
#define MAXRESERVED 10

/* Tokens definidos como um tipo enumerado */
typedef enum {
   /* declaracao de tipo */
   INTEIRO, 
   REAL,
   /* estrutura de decisao */
   SE, 
   ENTAO, 
   SENAO,
   /* estrutura de repeticao */
   ENQUANTO, 
   REPITA, 
   ATE,
   /* rotinas predefinidas */
   LER, 
   MOSTRAR,
   /* operadores */
   MAIS,
   MENOS,
   VEZES,
   SOBRE,
   E,
   OU,
   MENOR_QUE,
   MENOR_QUE_IGUAL,
   MAIOR_QUE,
   MAIOR_QUE_IGUAL,
   IGUAL,
   NAO_IGUAL,
   ATRIBUICAO,
   /* separadores */
   SEPARADOR_COMANDO,
   SEPARADOR_ID,
   /* blocos */
   ABRE_BLOCO_EXPRESSAO,
   FECHA_BLOCO_EXPRESSAO,
   INICIA_BLOCO_COMANDOS,
   FECHA_BLOCO_COMANDOS,
   /* tipos de numero */
   NUMERO_INTEIRO,
   NUMERO_REAL,
   /* identificador */
   IDENTIFICADOR,
   /* tokens de controle */
   ENDFILE,
   ERROR,
} TokenType;

extern FILE *source; /* arquivo de codigo fonte */
extern FILE *listing; /* arquivo com texto de saida */
extern FILE *code; /* arquivo de codigo para a maquina alvo */

extern int lineno; /* numero da linha do codigo fonte */

/*************************************************/
/*******  Arvore sintatica para o parser  ********/
/*************************************************/

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {DeclK,IfK,WhileK,RepeatK,ReadK,WriteK,AssignK} StmtKind;
typedef enum {OpK,ConstK,IdK} ExpKind;

/* ExpType eh utilizado para checagem de tipo */
typedef enum {Void,Integer,Real,Boolean} ExpType; // Isso faz parte do analisador semantico

#define MAXCHILDREN 3

typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     NodeKind nodekind;
     union { StmtKind stmt; ExpKind exp;} kind;
     union { TokenType op;
             union {
               int vint;
               float vreal;
             } val;
             char * name; } attr;
     ExpType type; /* para checagem de tipo das espressoes */
   } TreeNode;

/*****************************************************/
/***********   Flags para rastreamento    ************/
/*****************************************************/

/* EchoSource = TRUE faz o programa fonte ser impresso no
   arquivo listing com o numero de linha durante a analise
   sintatica */
extern int EchoSource;

/* TraceScan = TRUE faz a informacao do token ser impressa
   quando cada token eh reconhecido pela varredura */
extern int TraceScan;

/* TraceParse = TRUE faz a arvore sintatica ser impressa
   de forma linearizada (identada por filho) */
 extern int TraceParse;

/* TraceAnalyze = TRUE faz a insercao e a busca na
   tabela de simbolos serem mostradas no arquivo listing */
extern int TraceAnalyze;

/* TraceCode = TRUE faz os comentarios serem gravados no
   arquivo de codigo da maquina alvo quando o codigo eh gerado */
extern int TraceCode;

/* Error = TRUE previne passadas futuras se ocorrer um erro */
extern int Error; 
#endif
