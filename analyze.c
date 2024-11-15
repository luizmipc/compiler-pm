/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* Conta a localizacao da variavel na memoria */
static int location = 0;

/* Funcao recursiva generica para percorrer a arvore sintatica.
   Executa a funcao preProc em pre-ordem.
   Executa a funcao postProc em pos-ordem. */
static void traverse(TreeNode * t, void (* preProc) (TreeNode *), void (* postProc) (TreeNode *)) {
  if (t != NULL) {
    preProc(t); {
      int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* Funcao que não faz nada */
static void nullProc(TreeNode * t) {
  if (t == NULL) return;
  else return;
}

/* Insere os identificadores armazenados em t na tabela de simbolos */
static void insertNode( TreeNode * t) {
  switch (t->nodekind) {
    case StmtK: /* Se for uma declaracao */
      switch (t->kind.stmt) {
        case AssignK: /* Atribuicao */
        case ReadK: /* Leitura */
          if (st_lookup(t->attr.name) == -1) /* Ainda nao estah na tabela de simbolos */
            st_insert(t->attr.name,t->lineno,location++, t->type);
          else /* Ja esta na tabela de simbolos. Adicionar numero da linha */
            st_insert(t->attr.name,t->lineno,0, t->type);
          break;
        default:
          break;
      }
      break;
    case ExpK: /* Se for uma expressao */
      switch (t->kind.exp) {
        case IdK: /* Identificador */
          if (st_lookup(t->attr.name) == -1) /* Ainda nao estah na tabela de simbolos */
            st_insert(t->attr.name,t->lineno,location++, t->type);
          else /* Ja esta na tabela de simbolos. Adicionar numero da linha */
            st_insert(t->attr.name,t->lineno,0, t->type);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Constroi a tabela de simbolos varrendo a arvore sintatica em pre-ordem */
void buildSymtab(TreeNode * syntaxTree) {
  traverse(syntaxTree,insertNode,nullProc);
  if (TraceAnalyze) {
    fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

/* Exibe mensagem de erro de tipo */
static void typeError(TreeNode * t, char * message) {
  fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

/* Faz a verificacao de tipo em um no da arvore */
static void checkNode(TreeNode * t) {
  switch (t->nodekind) {
    case ExpK: /* No de expressao */
      switch (t->kind.exp) {
        case OpK: /* Tipo operador */
          if ((t->attr.op == IGUAL) || (t->attr.op == NAO_IGUAL) || (t->attr.op == MENOR_QUE) || (t->attr.op == MENOR_QUE_IGUAL) || (t->attr.op == MAIOR_QUE) || (t->attr.op == MAIOR_QUE_IGUAL )) {
            t->type = Boolean;
            break;
          } else if ((t->child[0]->type == Integer) || (t->child[1]->type == Integer)) {
            t->type = Integer;
            break;
          } else if ((t->child[0]->type == Integer) || (t->child[1]->type == Real)) {
            t->type = Real;
            break;
          } else if ((t->child[0]->type == Real) || (t->child[1]->type == Integer)) {
            t->type = Real;
            break;
          } else if ((t->child[0]->type == Real) || (t->child[1]->type == Real)) {
            t->type = Real;
            break;      
          }
          else
          typeError(t,"Op applied to non-integer or non-real");
          break;
        case ConstK: /* Constante */
        case IdK: /* Identificador */
          t->type = Integer;
          break;
        default:
          break;
      }
      break;
    case StmtK: /* No de declaracao */
      switch (t->kind.stmt) {
        case IfK: /* Declaracao IF */
        case WhileK:
          if (t->child[0]->type == Integer || t->child[0]->type == Real)
            typeError(t->child[0],"if test is not Boolean");
          break;
        case AssignK: /* Declaracao de atribuicao */
          if (t->child[0]->type == Integer || t->child[0]->type == Real) {
            t->child[0]->type = t->type;
            break;
          }
          else 
            typeError(t->child[0],"assignment of non-integer or non-real value");
          break;
        case WriteK: /* Declaracao WRITE */
          if (t->child[0]->type == Integer || t->child[0]->type == Real)
            break;
          else
            typeError(t->child[0],"write of non-integer or non-real value");
            break;
        /* case ReadK:
          if (t->child[0]->type == Integer || t->child[0]->type == Real)
            break;
          else
            typeError(t->child[0],"write of non-integer or non-real value");
            break; */
        case RepeatK:  /* Declaracao REPÈAT */
          if (t->child[1]->type == Integer || t->child[1]->type == Real)
            typeError(t->child[1],"repeat test is not Boolean");
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Faz a checagem de tipo varrendo a arvore sintatica em pos-ordem */
void typeCheck(TreeNode * syntaxTree) {
  traverse(syntaxTree,nullProc,checkNode);
}
