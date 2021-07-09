#include "global.h"

extern int lineno;

Token *nexttoken(void);
Token * LT(int);
int LA(int);

Anode *anode(Token *);
Anode *mkast(Token *, Anode *, Anode *);

/* parse a primary factor and return ast node pointer */
Anode *primary(void) {
    Token *tp;
 
    if (LA(1) != T_NUM) {
        fprintf(stderr, "primary error: not a number\n");
        exit(1);
    }
    tp = nexttoken();
    return anode(tp);
}
/* convert a token type to ast oprator */
void arithop(Token *tp) {
    switch(tp->type) {
    case T_PLUS:    tp->type = T_ADD;   break;
    case T_MINUS:   tp->type = T_SUB;   break;
    case T_STAR:    tp->type = T_MUL;   break;
    case T_SLASH:   tp->type = T_DIV;   break;
    default:
        fprintf(stderr, "line %d: unknown arithop %s\n", lineno, tp->text);
        exit(1);
    }
}

static int getprec(Token *tp)
{
    switch (tp->type) {
    case T_EOF:
    case T_NUM: return 0;
    
    case T_ADD:
    case T_SUB: return 10;
    
    case T_MUL:
    case T_DIV: return 20;
    
    default:
        fprintf(stderr, "line(%d): unsupport precedence %s\n", 
                            lineno, tp->text);
        exit(1);
    }
}

/* Return an AST tree whose root is a binary operator */
Anode *binexpr(int ptp){        /* ptp: previous token precedence */
    Token *tp;
    Anode *left, *right;
    
    left = primary();
    tp = nexttoken();
    if (tp->type == T_EOF)
        return left;
    arithop(tp);

    while (getprec(tp) > ptp) {
        right = binexpr(getprec(tp));
        left = mkast(tp, left, right);
        tp = LT(0);
    }
    return left;
}
