#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED, 
    TK_NUM, 
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};
char *user_input;   /* input program */
Token *token;       /* current token */

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
/* reports an error location and exit. */
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");   /* print pos spaces. */
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "expected '%c'", op);
    token = token->next;
}
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}
bool at_eof() {
    return token->kind == TK_EOF;
}
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}
Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) 
            p++;
        else if (strchr("+-*/()", *p)) 
            cur = new_token(TK_RESERVED, cur, p++);
        else if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
        } else
            error_at(p, "invalid token");
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}
/* parser */
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,     /* integer */
} NodeKind;
typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;      /* left hand side */
    Node *rhs;      /* right hand side */
    int val;        /* used if kind == ND_NUM */
};
Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}
Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *expr();
Node *mul();
Node *unary();
Node *primary();

/* expr = mul ("+" mul | "-" mul)*  */
Node *expr() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = new_binary(ND_ADD, node, mul());
        else if (consume('-'))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}
/* mul = unary ("*" unary | "/" unary)*  */
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume('*'))
            node = new_binary(ND_MUL, node, unary());
        else if (consume('/'))
            node = new_binary(ND_DIV, node, unary());
        else   
            return node;
    }
}
/* unary = ("+" | "-")? unary   */
Node *unary() {
    if (consume('+'))
        return unary();
    if (consume('-'))
        return new_binary(ND_SUB, new_num(0), unary());
    return primary();
}
/* primary = "(" expr ")" | num   */
Node *primary() {
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }
    return new_num(expect_number());
}
/* code generator */
void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("\tpush\t%d\n", node->val);
        return;
    }
    gen(node->lhs);
    gen(node->rhs);

    printf("\tpop\trdi\n");
    printf("\tpop\trax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("\tadd\trax, rdi\n");    break;
    case ND_SUB:
        printf("\tsub\trax, rdi\n");    break;
    case ND_MUL:
        printf("\timul\trax, rdi\n");   break;
    case ND_DIV:
        printf("\tcqo\n");
        printf("\tidiv\trdi\n");        break;
    }
    printf("\tpush\trax\n");
}


int main(int argc, char *argv[]) {
    if (argc != 2) 
        error("%s, invalid number of arguments", argv[0]);

    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    printf("\t.intel_syntax noprefix\n");
    printf("\t.global main\n");
    printf("main:\n");  

    /* traverse the AST to emit assembly. */
    gen(node);  

    printf("\tpop\trax\n");
    printf("\tret\n");
    return 0;
}
