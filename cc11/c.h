#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;
/* tokenize.c */
typedef enum {
    TK_IDENT,   /* identifiers */
    TK_PUNCT,   /* puncuators */
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *loc;
    int len;
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
Token *tokenize(char *input);

/* parse.c */

/* local variable */
typedef struct Obj Obj;
struct Obj {
    Obj *next;
    char *name;     /* variable name */
    int offset;     /* offset from %rbp */
};
/* function */
typedef struct Function Function;
struct Function {
    Node *body;
    Obj *locals;
    int stack_size;
};
/* ast node */
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NEG,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_ASSIGN,
    ND_EXPR_STMT,   /* Expression statement */
    ND_VAR,
    ND_NUM,
} NodeKind;
struct Node {
    NodeKind kind;
    Node *next;
    Node *lhs;
    Node *rhs;
    Obj *var;       /* used if kind == ND_VAR */
    int val;        /* used if kind == ND_NUM */
};

Function *parse(Token *tok);

void codegen(Function *prog);