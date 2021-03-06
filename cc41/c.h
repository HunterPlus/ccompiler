#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;
typedef struct Node Node;

/*  strings.c   */
char *format(char *fmt, ...);

/* tokenize.c */
typedef enum {
    TK_IDENT,   /* identifiers */
    TK_PUNCT,   /* puncuators */
    TK_KEYWORD, /* keywords */
    TK_STR,     /* string literals */
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;            /* if kind is TK_NUM, its value */
    char *loc;          /* token location   */
    int len;            /* token length     */
    Type *ty;           /* used if TK_STR   */
    char *str;          /* string literal contents including terminating '\0' */
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
bool consume(Token **rest, Token *tok, char *str);
Token *tokenize_file(char *filename);

/*   parse.c   */

/*  variable or function    */
typedef struct Obj Obj;
struct Obj {
    Obj *next;
    char *name;     /* variable name    */
    Type *ty;       /* type         */
    bool is_local;  /* local or global/function */

    int offset;     /* local variable   */

    bool is_function;   /* global variable or function */

    char *init_data;    /* global variable  */

    /*  function    */
    Obj *params;
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
    ND_STMT_EXPR,   /* statement expression     */
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

    Node *body;     /* block or statement expression */

    char *funcname; /* function call            */
    Node *args;
    
    Obj *var;       /* used if kind == ND_VAR   */
    int val;        /* used if kind == ND_NUM   */
};

Obj *parse(Token *tok);

/* type.c  */
typedef enum {
    TY_CHAR,
    TY_INT,
    TY_PTR,
    TY_FUNC,
    TY_ARRAY,
} TypeKind;

struct Type {
    TypeKind kind;

    int size;           /* sizeof() value   */

    Type *base;         /* pointer      */

    Token *name;        /* declaration  */

    int array_len;      /* array        */

    /*  function type   */
    Type *return_ty;
    Type *params;
    Type *next;
};

extern Type *ty_char;
extern Type *ty_int;

bool is_integer(Type *ty);
Type *copy_type(Type *ty);
Type *pointer_to(Type *base);
Type *func_type(Type *return_ty);
Type *array_of(Type *base, int size);
void add_type(Node *node);

/*  codegen.c  */
void codegen(Obj *prog, FILE *out);