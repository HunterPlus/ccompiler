#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;
typedef struct Node Node;

/* tokenize.c */
typedef enum {
    TK_IDENT,   /* identifiers */
    TK_PUNCT,   /* puncuators */
    TK_KEYWORD, /* keywords */
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
bool consume(Token **rest, Token *tok, char *str);
Token *tokenize(char *input);

/*   parse.c   */

/* local variable */
typedef struct Obj Obj;
struct Obj {
    Obj *next;
    char *name;     /* variable name    */
    Type *ty;       /* var type         */
    int offset;     /* offset from %rbp */
};
/* function */
typedef struct Function Function;
struct Function {
    Function *next;
    char *name;
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
    ND_LT,          /* <                        */
    ND_LE,          /* <=                       */
    ND_ASSIGN,      /* =                        */
    ND_ADDR,        /* unary &                  */
    ND_DEREF,       /* unary *                  */
    ND_RETURN,      /* "return "                */
    ND_IF,          /* "if"                     */
    ND_FOR,         /* "for" or "while"         */
    ND_BLOCK,       /* {...}                    */
    ND_FUNCALL,     /* function call            */
    ND_EXPR_STMT,   /* Expression statement     */
    ND_VAR,         /* variable                 */
    ND_NUM,         /* integer                  */
} NodeKind;
struct Node {
    NodeKind kind;
    Node *next;
    Type *ty;
    Token *tok;
    Node *lhs;
    Node *rhs;
    /* "if" or "for statement */
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;

    Node *body;     /* block */

    char *funcname; /* function call            */
    Node *args;
    
    Obj *var;       /* used if kind == ND_VAR   */
    int val;        /* used if kind == ND_NUM   */
};

Function *parse(Token *tok);

/* type.c  */
typedef enum {
    TY_INT,
    TY_PTR,
    TY_FUNC,
} TypeKind;

struct Type {
    TypeKind kind;
    Type *base;         /* pointer      */
    Token *name;        /* declaration  */

    /*  function type   */
    Type *return_ty;
};
extern Type *ty_int;

bool is_integer(Type *ty);
Type *pointer_to(Type *base);
Type *func_type(Type *return_ty);
void add_type(Node *node);

/*  codegen.c  */
void codegen(Function *prog);