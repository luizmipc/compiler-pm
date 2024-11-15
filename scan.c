#include "globals.h"
#include "util.h"
#include "scan.h"

/* Estados do DFA para análise léxica */
typedef enum {
    START, INASSIGN, INNUM, INREAL, INID, DONE, INCOMMENT
} StateType;

/* Lexema para identificador ou palavra reservada */
char tokenString[MAXTOKENLEN+1];

/* BUFLEN = tamanho do buffer de entrada para linhas do código fonte */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* Linha atual */
static int linepos = 0; /* Posição em lineBuf */
static int bufsize = 0; /* Tamanho atual da string no buffer */
static int EOF_flag = FALSE; /* Corrige o comportamento de ungetNextChar no EOF */

/* getNextChar pega o próximo caractere não branco de lineBuf, lendo uma nova linha se lineBuf estiver todo lido */
static int getNextChar(void) {
    if (!(linepos < bufsize)) {
        lineno++;
        if (fgets(lineBuf, BUFLEN-1, source)) {
            if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);
            bufsize = strlen(lineBuf);
            linepos = 0;
            return lineBuf[linepos++];
        } else {
            EOF_flag = TRUE;
            return EOF;
        }
    } else {
        return lineBuf[linepos++];
    }
}

/* ungetNextChar retrocede um caractere em lineBuf */
static void ungetNextChar(void) {
    if (!EOF_flag) linepos--;
}

/* Tabela de busca de palavras reservadas */
static struct {
    char* str;
    TokenType tok;
} reservedWords[MAXRESERVED] = {
    {"inteiro", INTEIRO}, {"real", REAL}, {"se", SE}, {"entao", ENTAO},
    {"senao", SENAO}, {"enquanto", ENQUANTO}, {"repita", REPITA},
    {"ate", ATE}, {"ler", LER}, {"mostrar", MOSTRAR}
};

/* Verifica se um identificador é uma palavra reservada */
static TokenType reservedLookup(char *s) {
    int i;
    for (i = 0; i < MAXRESERVED; i++) {
        if (!strcmp(s, reservedWords[i].str)) {
            return reservedWords[i].tok;
        }
    }
    return IDENTIFICADOR;
}

/**********************************/
/* Função principal da varredura  */
/**********************************/

TokenType getToken(void) {
    int tokenStringIndex = 0;
    TokenType currentToken;
    
    /* estado corrente */
    StateType state = START;
    int save;

    while (state != DONE) {
        int c = getNextChar();
        save = TRUE;

        switch (state) {
            case START:
                if (isdigit(c)) {
                    state = INNUM;
                } else if (c == '.') {
                    state = INREAL;
                } else if (isalpha(c)) {
                    state = INID;
                } else if (c == ':') {
                    state = INASSIGN;
                } else if ((c == ' ') || (c == '\t') || (c == '\n')) {
                    save = FALSE;
                } else {
                    state = DONE;
                    switch (c) {
                        case EOF:
                            save = FALSE;
                            currentToken = ENDFILE;
                            break;
                        case '{':
                            currentToken = INICIA_BLOCO_COMANDOS;
                            break;
                        case '}':
                            currentToken = FECHA_BLOCO_COMANDOS;
                            break;
                        case '=':
                            if (getNextChar() == '=') {
                                currentToken = IGUAL;
                            } else {
                                ungetNextChar();
                                currentToken = ATRIBUICAO;
                            }
                            break;
                        case '<':
                            if (getNextChar() == '=') {
                                currentToken = MENOR_QUE_IGUAL;
                            } else {
                                ungetNextChar();
                                currentToken = MENOR_QUE;
                            }
                            break;
                        case '>':
                            if (getNextChar() == '=') {
                                currentToken = MAIOR_QUE_IGUAL;
                            } else {
                                ungetNextChar();
                                currentToken = MAIOR_QUE;
                            }
                            break;
                        case '!':
                            if (getNextChar() == '=') {
                                currentToken = NAO_IGUAL;
                            } else {
                                ungetNextChar();
                                currentToken = ERROR;
                            }
                            break;
                        case '+':
                            currentToken = MAIS;
                            break;
                        case '-':
                            currentToken = MENOS;
                            break;
                        case '*':
                            currentToken = VEZES;
                            break;
                        case '/':
                            if (getNextChar() == '*') {
                                save = FALSE;
                                state = INCOMMENT;
                            } else {
                                ungetNextChar();
                                currentToken = SOBRE;
                            }
                            break;
                        case '&':
                            if(getNextChar() == '&') {
                                currentToken = E;
                            } else {
                                ungetNextChar();
                                state = INID;
                            }
                            break;
                        case '|':
                            if(getNextChar() == '|') {
                                currentToken = OU;
                            } else {
                                ungetNextChar();
                                state = INID;
                            }
                            break;
                        case '(':
                            currentToken = ABRE_BLOCO_EXPRESSAO;
                            break;
                        case ')':
                            currentToken = FECHA_BLOCO_EXPRESSAO;
                            break;
                        case ';':
                            currentToken = SEPARADOR_COMANDO;
                            break;
                        case ',':
                            currentToken = SEPARADOR_ID;
                            break;
                        default:
                            currentToken = ERROR;
                            break;
                    }
                }
                break;

            case INCOMMENT:
                save = FALSE;
                if (c == EOF)
                { state = DONE;
                  currentToken = ENDFILE;
                }
                else if (c == '*' && getNextChar() == '/') state = START;
                break;

            case INASSIGN:
                state = DONE;
                if (c == '=') {
                    currentToken = ATRIBUICAO;
                } else {
                    ungetNextChar();
                    save = FALSE;
                    currentToken = ERROR;
                }
                break;

            case INNUM:
                if (isdigit(c)) {
                    // Continua no estado de número
                } else if (c == '.') {
                    state = INREAL; // Se encontrar um ponto, muda para INREAL
                } else {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken = NUMERO_INTEIRO;
                }
                break;

            case INREAL:
                if (isdigit(c)) {
                    // Continua no estado de número real
                } else {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken = NUMERO_REAL;
                }
                break;

            case INID:
                if (!isalnum(c)) {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken = IDENTIFICADOR;
                }
                break;

            case DONE:
                tokenString[tokenStringIndex] = '\0';
                if (currentToken == IDENTIFICADOR) {
                    currentToken = reservedLookup(tokenString);
                }
                break;
                
            default:
                state = DONE;
                currentToken = ERROR;
                break;
        }

        if ((save) && (tokenStringIndex <= MAXTOKENLEN)) {
            tokenString[tokenStringIndex++] = (char)c;
        }
        
        if (state == DONE) {
            tokenString[tokenStringIndex] = '\0';
            if (currentToken == IDENTIFICADOR) {
                currentToken = reservedLookup(tokenString);
            }
        }
    }

    if (TraceScan) {
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    
    return currentToken;
}