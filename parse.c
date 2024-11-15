/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the P- compiler  */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* Armazena o token corrente */

/* Prototipos de funcoes para as chamadas recursivas */
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * decl(void);
static TreeNode * if_stmt(void);
static TreeNode * while_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * expr(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);

/* Funcao para mostrar aviso de erro sintatico */
static void syntaxError(char *message) {
  fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

/* Verifica se o token esperado é o token corrente.
   Caso afirmativo, pega o proximo token. */
static void match(TokenType expected) {
  if (token == expected)
    token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}

/* Avalia sequência de declarações */
/* decl-sequencia -> { declaracao; } | ENDFILE | FECHA_BLOCO_COMANDOS */
TreeNode *stmt_sequence(void) {
  TreeNode *t = statement(); /* Monta no com primeira declaracao */
  TreeNode *p = t;
  while ((token!=ENDFILE) && (token!=FECHA_BLOCO_COMANDOS) && (token!=SENAO) && (token!=ATE)) {
    TreeNode * q;
    if(token==ENDFILE)
      break;
    if(token==SEPARADOR_COMANDO)
      match(SEPARADOR_COMANDO); /* Captura ponto e virgula */
    if (token==FECHA_BLOCO_COMANDOS) {
      break;
    }
    q = statement(); /* Monta no de declaracao */
    if (q != NULL) {
      if (t == NULL)
        t = p = q;
      else {
        p->sibling = q; /* Encadeia com comando anterior */
        p = q;
      }
    }
  }

  return t;
}

/* Avalia declaracao */
/* declaracao -> if-decl | repeat-decl | atribuicao-decl | read-decl | write-decl | ENDFILE */
TreeNode *statement(void) {
  TreeNode *t = NULL;
  switch (token) {
    case INTEIRO:
    case REAL:
      t = decl();
      break;
    case SENAO:
    case SE:
      t = if_stmt();
      break;
    case ENQUANTO:
      t = while_stmt();
      break;
    case REPITA:
      t = repeat_stmt(); 
      break;
    case IDENTIFICADOR:
      t = assign_stmt(); /* Monta no ATRIBUICAO */
      break;
     case LER:
      t = read_stmt();
      break;
    case MOSTRAR:
      t = write_stmt(); 
      break;
    case ENDFILE:
      break;
    default: /* Erro */
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
  } /* end case */
  return t;
}

/* declaração -> tipo identificador { , identificador } */
/* tipo -> INTEIRO | REAL */
/* identificador -> IDENTIFICADOR */
TreeNode *decl(void) {

    TreeNode *t = newStmtNode(DeclK); // Cria o nó para a declaração
    if(token==REAL) {
      t->attr.name = copyString(tokenString);
      t->type = Real;
      match(REAL);
      if (t != NULL) {
        // Lida com o primeiro identificador
        t->child[0] = expr(); // Assumindo que expr() retorna um nó para o primeiro identificador
        TreeNode *current = t->child[0]; // Começa com o primeiro identificador
        current->type = Real;
        // Laço para conectar identificadores adicionais
        while (token == SEPARADOR_ID) {
          match(SEPARADOR_ID); // Captura o identificador
          TreeNode *nextId = expr(); // Cria um novo nó para o próximo identificador
          nextId->type = Real;
          if (nextId != NULL) {
            // Liga o filho do identificador atual ao novo identificador
            current->sibling = nextId; // Assumindo que você deseja usar sibling para próximos identificadores
            current = nextId; // Move para o identificador recém-adicionado
          }
        }
      }
    }
    if(token==INTEIRO) {
      t->attr.name = copyString(tokenString);
      t->type = Integer;
      match(INTEIRO);
      if (t != NULL) {
        // Lida com o primeiro identificador
        t->child[0] = expr(); // Assumindo que expr() retorna um nó para o primeiro identificador
        TreeNode *current = t->child[0]; // Começa com o primeiro identificador
        current->type = Integer;
        // Laço para conectar identificadores adicionais
        while (token == SEPARADOR_ID) {
          match(SEPARADOR_ID); // Captura o identificador
          TreeNode *nextId = expr(); // Cria um novo nó para o próximo identificador
          nextId->type = Integer;
          
          if (nextId != NULL) {
            // Liga o filho do identificador atual ao novo identificador
            current->sibling = nextId; // Assumindo que você deseja usar sibling para próximos identificadores
            current = nextId; // Move para o identificador recém-adicionado
          }
        }
      }
    } 
    return t; // Retorna o nó raiz da lista de identificadores
}

/* Avalia declaração condicional (IF) */
/* if-decl -> se exp entao comando1 [ senao comando2 ] */
/* comando1 -> stmt | bloco-comandos */
/* comando2 -> stmt | bloco-comandos */
/* stmt-sequencia -> declaracao { ; declaracao } */
TreeNode *if_stmt(void) {
    TreeNode *t = newStmtNode(IfK); /* Aloca no de declaração IF */
    
    
    if (token == SE) {
        t->attr.name = copyString(tokenString);
        match(SE); /* Captura SE */
    }

    if (t != NULL) {
        t->child[0] = expr(); /* Monta nó de expressão */
    }

    if (token == ENTAO) {
        match(ENTAO); /* Captura ENTAO */
    }
    

    /* Verifica se o próximo token é um bloco de comandos ou apenas uma única declaração */
    if (token == INICIA_BLOCO_COMANDOS) {
      match(INICIA_BLOCO_COMANDOS); /* Início do bloco de comandos */
      if (t != NULL) {
          t->child[1] = stmt_sequence(); /* Monta nó com sequência de declarações */
      }
      match(FECHA_BLOCO_COMANDOS); /* Fecha bloco de comandos */
    } else if (token == SE) {
      /* Aqui tratamos um 'se' aninhado logo após o 'entao' */
      if (t != NULL) {
        t->child[1] = if_stmt(); /* Chama if_stmt() recursivamente para o 'se' aninhado */
      }
    } else {
      if (t != NULL) {
        t->child[1] = statement(); /* Monta nó com uma única declaração */
      }
    }

    /* Verifica se existe a parte SENAO */
    if (token == SENAO) { /* Se existir parte ELSE */
        if(token==SENAO) {
          match(SENAO); /* Captura SENAO */
        }

        /* Verifica se o próximo token é um bloco de comandos ou apenas uma única declaração */
        if (token == INICIA_BLOCO_COMANDOS) {
          match(INICIA_BLOCO_COMANDOS); /* Início do bloco de comandos do ELSE */
          if (t != NULL) {
              t->child[2] = stmt_sequence(); /* Monta nó com sequência de declarações do ELSE */
          }
          match(FECHA_BLOCO_COMANDOS); /* Fecha bloco de comandos do ELSE */
        } else if (token == SE) {
          /* Aqui tratamos um 'se' aninhado logo após o 'entao' */
          if (t != NULL) {
            t->child[1] = if_stmt(); /* Chama if_stmt() recursivamente para o 'se' aninhado */
          }
        }else {
            if (t != NULL) {
                t->child[2] = statement(); /* Monta nó com uma única declaração no ELSE */
                match(SEPARADOR_COMANDO);
            }
        }
    }
    return t;
}

/* while-decl -> enquanto exp bloco-comandos */
/* comando-sequencia -> stmt { SEPARADOR_COMANDO stmt } */
TreeNode *while_stmt(void) {
  TreeNode *t = newStmtNode(WhileK);
  
  // Verificar se o token atual é 'ENQUANTO'
  if (token == ENQUANTO) {
    t->attr.name = copyString(tokenString);
    match(ENQUANTO);
  }

  // Construir a expressão condicional do while
  if (t != NULL)
    t->child[0] = expr(); 

  // Verificar se é uma sequência de comandos ou um comando único
  if (token == INICIA_BLOCO_COMANDOS) {
    match(INICIA_BLOCO_COMANDOS);
    if (t != NULL)
      t->child[1] = stmt_sequence();
    match(FECHA_BLOCO_COMANDOS);
  } else {
    // Se não for um bloco, aceitar apenas um comando simples
    if (t != NULL)
      t->child[1] = statement();
  }

  return t;
}

/* Avalia declaracao de repeticao (REPEAT) */
/* repet-decl -> repita decl-sequencia ate exp */
TreeNode *repeat_stmt(void) {
  TreeNode *t = newStmtNode(RepeatK); /* Aloca no de declaracao IF */
  if(token==REPITA) {
    t->attr.name = copyString(tokenString);
    match(REPITA); /* Captura IF */
  }
  if(token==INICIA_BLOCO_COMANDOS) {
    match(INICIA_BLOCO_COMANDOS);
  }
  if (t!=NULL)
    t->child[0] = stmt_sequence(); /* Monta no com sequencia de declaracoes */
  if(token==FECHA_BLOCO_COMANDOS) {
    match(FECHA_BLOCO_COMANDOS);
  }
  match(ATE); /* Captura THEN */
  if (t != NULL)
    t->child[1] = expr(); /* Monta no de expressao */
  return t;
}

/* Avalia declaracao de atribuicao */
/* atrib-decl -> identificador=exp; */
TreeNode *assign_stmt(void) {
  TreeNode *t = newStmtNode(AssignK); 
  if ((t != NULL) && (token == IDENTIFICADOR))
    t->attr.name = copyString(tokenString);
  match(IDENTIFICADOR); 
  match(ATRIBUICAO); 
  if (t != NULL)
    t->child[0] = expr();
  return t;
}

/* Avalia declaracao READ */
/* read-decl -> ler(identificador); */
TreeNode *read_stmt(void) {
  TreeNode *t = newStmtNode(ReadK); 
  if(token==LER) {
    match(LER);
  }
  if(token == ABRE_BLOCO_EXPRESSAO)
    match(ABRE_BLOCO_EXPRESSAO);
  if ((t != NULL) && (token == IDENTIFICADOR))
    t->attr.name = copyString(tokenString);
    match(IDENTIFICADOR); 
  match(FECHA_BLOCO_EXPRESSAO);
  return t;
}

/* Avalia declaracao WITE */
/* write-decl -> mostrar(exp); */
TreeNode *write_stmt(void) {
  TreeNode *t = newStmtNode(WriteK); 
  if ((t != NULL) && (token == IDENTIFICADOR))
    t->attr.name = copyString(tokenString); 
  match(MOSTRAR);
  match(ABRE_BLOCO_EXPRESSAO);
  if (t != NULL)
    t->child[0] = expr();
  match(FECHA_BLOCO_EXPRESSAO);
  return t;
}

/* Avalia expressao */
/* exp -> simples-exp [ comparacao-op simples-exp ] */
TreeNode *expr(void) {
  TreeNode *t = simple_exp(); 
  if ((token == MAIOR_QUE) || (token == MAIOR_QUE_IGUAL) || 
  (token == MENOR_QUE) || (token == MENOR_QUE_IGUAL) ||
  (token == IGUAL) || (token == NAO_IGUAL) ||
  (token == E) || (token == OU)
  ) {
    TreeNode *p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token); 
    if (t != NULL)
      t->child[1] = simple_exp();
  }
  return t;
}

/* Avalia expressao simples */
/* simples-exp -> termo { soma termo } */
TreeNode *simple_exp(void) {
  TreeNode *t = term();
  while ((token == MAIS) || (token == MENOS) ||
  (token == MAIOR_QUE) || (token == MAIOR_QUE_IGUAL) || 
  (token == MENOR_QUE) || (token == MENOR_QUE_IGUAL) ||
  (token == IGUAL) || (token == NAO_IGUAL) ||
  (token == E) || (token == OU)) {
    TreeNode *p = newExpNode(OpK); 
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token); 
      t->child[1] = term();
    }
  }
  return t;
}

/* Avalia termo */
/* termo -> fator { op fator } */
TreeNode * term(void) {
  TreeNode * t = factor(); // Obtenha o primeiro fator
  while (token == VEZES || token == SOBRE ||
  (token == MAIOR_QUE) || (token == MAIOR_QUE_IGUAL) || 
  (token == MENOR_QUE) || (token == MENOR_QUE_IGUAL) ||
  (token == IGUAL) || (token == NAO_IGUAL) ||
  (token == E) || (token == OU)) { // Enquanto houver multiplicação ou divisão
    TreeNode * p = newExpNode(OpK); // Cria um novo nó de operação
    if (p != NULL) {
      p->child[0] = t; // O filho esquerdo é o resultado anterior
      p->attr.op = token; // A operação é o token atual
      match(token); // Avança o token para o próximo
      p->child[1] = factor(); // O filho direito é o próximo fator
      t = p; // Atualiza t para ser o novo nó
    }
  }
  return t; // Retorna a árvore construída
}

/* Avalia fator */
/* fator -> ( exp ) | numero | identificador  | ENDFILE */
TreeNode * factor(void) {
  TreeNode * t = NULL;
  switch (token) {
    case NUMERO_INTEIRO: /* fator -> numero */
      t = newExpNode(ConstK); 
      if ((t != NULL) && (token == NUMERO_INTEIRO))
        t->attr.val.vint = atoi(tokenString);
      t->type = Integer;
      match(NUMERO_INTEIRO); 
      break;
    case NUMERO_REAL:
      t = newExpNode(ConstK);
      if((t != NULL) && (token == NUMERO_REAL))
        t->attr.val.vreal = atof(tokenString);
      t->type = Real;
      match(NUMERO_REAL);
      break;
    case IDENTIFICADOR: 
      t = newExpNode(IdK);
      if ((t != NULL) && (token == IDENTIFICADOR))
        t->attr.name = copyString(tokenString); 
      match(IDENTIFICADOR); 
      break;
    case ABRE_BLOCO_EXPRESSAO: 
      match(ABRE_BLOCO_EXPRESSAO); 
      t = expr();
      match(FECHA_BLOCO_EXPRESSAO); 
      break;
    case ENDFILE: 
      token = getToken();
      break; // O programa so terminou, entao, nao imprime nenhum erro
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
    }
  return t;
}

/******************************/
/* Funcao principal do parser */
/******************************/

/* A funcao parse retorna a arvore sintatica construida */
TreeNode * parse(void) {
  TreeNode * t;
  token = getToken(); /* Captura primeiro token */
  t = stmt_sequence(); /* Monta arvore sintatica */
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
