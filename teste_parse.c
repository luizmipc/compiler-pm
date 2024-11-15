#include "util.c"
#include "scan.c"
#include "parse.c"
#include "symtab.c"
#include "analyze.c"

int lineno = 0;
int Error;

FILE *source;   /* arquivo de código fonte */
FILE *listing;  /* arquivo texto de saída */
FILE *code;     /* arquivo de código para a máquina alvo */

int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = FALSE;

/* Função para fazer o parse e listar a árvore de sintaxe */
void parse_and_list() {
    TreeNode *t;

    // Chama a função de parse
    t = parse();
    
    // Verifica se a árvore de sintaxe foi gerada com sucesso
    if (t != NULL) {
        // Imprime a árvore de sintaxe no arquivo de listagem
        printTree(t);
    } else {
        fprintf(listing, "Erro durante o parsing.\n");
    }
}

void main() {
	TreeNode *t;
	
	if ((source = fopen("sample.pm", "r")) == NULL) {
		perror("Abertura de sample.pm: ");
		return;
	}
	if ((listing = fopen("listing.txt", "w")) == NULL) {
		perror("Abertura de listing.txt: ");
		return;
	}

	t = parse();
	buildSymtab(t);
	typeCheck(t);
	
	fclose(source);
	fclose(listing);
}
