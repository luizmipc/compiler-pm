/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the P- compiler                            */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Imprime um token e seu lexema no arquivo listing */
void printToken(TokenType token, const char* tokenString) {
    switch (token) {
        /* declaracao de tipo */
        case INTEIRO:
            fprintf(listing, "%s\n", tokenString);
            break;
        case REAL:
            fprintf(listing, "%s\n", tokenString);
            break;
        /* estrutura de decisao */
        case SE:
            fprintf(listing, "%s\n", tokenString);
            break;
        case ENTAO:
            fprintf(listing, "%s\n", tokenString);
            break;
        case SENAO:
            fprintf(listing, "%s\n", tokenString);
            break;
        /* estrutura de repeticao */
        case ENQUANTO:
            fprintf(listing, "%s\n", tokenString);
            break;
        case REPITA:
            fprintf(listing, "%s\n", tokenString);
            break;
        case ATE:
            fprintf(listing, "%s\n", tokenString);
            break;
        /* rotinas predefinidas */
        case LER:
            fprintf(listing, "%s\n", tokenString);
            break;
        case MOSTRAR:
            fprintf(listing, "%s\n", tokenString);
            break;
        /* operadores */
        case MAIS:
            fprintf(listing, "+\n", tokenString);
            break;
        case MENOS:
            fprintf(listing, "-\n", tokenString);
            break;
        case VEZES:
            fprintf(listing, "*\n", tokenString);
            break;
        case SOBRE:
            fprintf(listing, "/\n", tokenString);
            break;
        case E:
            fprintf(listing, "&&\n", tokenString);
            break;
        case OU:
            fprintf(listing, "||\n", tokenString);
            break;
        case MENOR_QUE:
            fprintf(listing, "<\n", tokenString);
            break;
        case MENOR_QUE_IGUAL:
            fprintf(listing, "<=\n", tokenString);
            break;
        case MAIOR_QUE:
            fprintf(listing, ">\n", tokenString);
            break;
        case MAIOR_QUE_IGUAL:
            fprintf(listing, ">=\n", tokenString);
            break;
        case IGUAL:
            fprintf(listing, "==\n", tokenString);
            break;
        case NAO_IGUAL:
            fprintf(listing, "!=\n", tokenString);
            break;
        case ATRIBUICAO: 
            fprintf(listing, "=\n", tokenString);
            break;
        /* separadores */
        case SEPARADOR_COMANDO:
            fprintf(listing, ";\n", tokenString);
            break;
        case SEPARADOR_ID:
            fprintf(listing, ",\n", tokenString);
            break;
        /* blocos */
        case ABRE_BLOCO_EXPRESSAO:
            fprintf(listing, "%s\n", tokenString);
            break;
        case FECHA_BLOCO_EXPRESSAO:
            fprintf(listing, "%s\n", tokenString);
            break;
        case INICIA_BLOCO_COMANDOS:
            fprintf(listing, "%s\n", tokenString);
            break;
        case FECHA_BLOCO_COMANDOS:
            fprintf(listing, "%s\n", tokenString);
            break;
        /* tipos de numero */
        case NUMERO_INTEIRO:
            fprintf(listing, "%s\n", tokenString);
            break;
        case NUMERO_REAL:
            fprintf(listing, "%s\n", tokenString);
            break;
        /* identificador */
        case IDENTIFICADOR:
            fprintf(listing, "%s\n", tokenString);
            break;
        case ENDFILE: fprintf(listing, "EOF\n"); break;
        case ERROR: fprintf(listing, "ERROR: %s\n", tokenString); break;
        default: fprintf(listing, "Unknown token: %d\n", token);
    }
}

/* cria um no de declaracao para a construcao da arvore sintatica */
TreeNode * newStmtNode(StmtKind kind) {
  TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0; i<MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* cria um no de expressao para a construcao da arvore sintatica */
TreeNode *newExpNode(ExpKind kind) {
  TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0; i<MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* aloca espaço e faz copia de uma string existente */
char * copyString(char * s) {
  int n;
  char * t;
  if (s == NULL)
    return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t == NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else
    strcpy(t,s);
  return t;
}

/* A variavel indentno eh usada por printTree para armazenar a quantidade de espacos a indentar */
static int indentno = 0;

/* Macros para incrementar e decrementrar a indentacao */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces faz a indentacao imprimindo espacos */
static void printSpaces(void) {
  int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* A funcao printTree imprime e arvore sintatica para o arquivo 
 * listing usando indentacao para indicar as sub-arvores
 */
void printTree( TreeNode * tree ) {
  int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK) {
      switch (tree->kind.stmt) {
        case DeclK:
          if (strcmp(tree->attr.name, "inteiro") == 0) {
              fprintf(listing, "Inteiro: \n");
          } else {
              fprintf(listing, "Real: \n");
          }
          break;
        case IfK:
          fprintf(listing,"Se: \n");
          break;
        case WhileK:
          fprintf(listing, "Enquanto: \n");
          break;
        case RepeatK:
          fprintf(listing,"Repita: \n");
          break;
        case AssignK:
          fprintf(listing,"Atribui para: %s\n",tree->attr.name);
          break;
        case ReadK:
          fprintf(listing,"Leia: %s\n",tree->attr.name);
          break;
        case WriteK:
          fprintf(listing,"Mostrar: \n");
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK) {
      switch (tree->kind.exp) {
        case OpK:
          fprintf(listing,"Op: ");
          printToken(tree->attr.op,"\0");
          break;
        case ConstK:
          if(tree->type == Real)
            fprintf(listing,"Const: %f\n",tree->attr.val.vreal);
          else
            fprintf(listing, "Const: %d\n", tree->attr.val.vint);
          break;
        case IdK:
          fprintf(listing,"Id: %s\n",tree->attr.name);
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
