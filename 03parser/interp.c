#include "global.h"

int interp(Anode *ap) {
    int lval, rval;
    
    if (ap->left)
        lval = interp(ap->left);
    if (ap->right)
        rval = interp(ap->right);
    if (ap->token->type == T_NUM)
        printf("num: %d\n", atoi(ap->token->text));
    else
        printf("%d %s %d\n", lval, ap->token->text, rval);
    switch (ap->token->type) {
    case T_NUM: return atoi(ap->token->text);
    case T_ADD: return (lval + rval);
    case T_SUB: return (lval - rval);
    case T_MUL: return (lval * rval);
    case T_DIV: return (lval / rval);
    default:
        fprintf(stderr, "interp error: %s\n", ap->token->text);
        exit(1);
    }
}