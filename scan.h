#ifndef _SCAN_H_
#define _SCAN_H_

/* MAXTOKENLEN é o tamanho máximo de um token */
#define MAXTOKENLEN 40

/* tokenString armazena o lexema de cada token */
extern char tokenString[MAXTOKENLEN+1];

/* retorna o próximo token do arquivo fonte */
TokenType getToken(void);

#endif
