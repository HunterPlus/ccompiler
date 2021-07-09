#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TOKENSIZ  100

/* token structure */
typedef struct Token {
    int type;
    char text[TOKENSIZ];
} Token;

/* tokens */
enum {
    T_EOF, 
    T_PLUS, T_MINUS,    T_STAR, T_SLASH, 
    T_ADD,  T_SUB,      T_MUL,  T_DIV,      /* arithmetic oprator */
    
    T_NUM, 
    T_NAME,
};
/* ast node structure */
typedef struct Anode {
    Token *token;
    struct Anode *left;
    struct Anode *right;
} Anode; 
