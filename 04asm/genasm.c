#include "global.h"


int regload(int);
int regadd(int, int);
int regsub(int, int);
int regmul(int, int);
int regdiv(int, int);

void preasm(void);
void postasm(void);
void printint(int);

static int genasm(Anode *ap)
{
    int lreg, rreg;             /* left register, right register */
    
    if (ap->left)
        lreg = genasm(ap->left);
    if (ap->right)
        rreg = genasm(ap->right);
    
    switch (ap->token->type) {
    case T_NUM: return regload(atoi(ap->token->text));
    case T_ADD: return regadd(lreg, rreg);
    case T_SUB: return regsub(lreg, rreg);
    case T_MUL: return regmul(lreg,rreg);
    case T_DIV: return regdiv(lreg,rreg);
    
    default:
        fprintf(stderr, "genasm: unkown ast op %s\n", ap->token->text);
        exit(1);
    }
}
void gencode(Anode *ap)
{
    int reg;
    
    preasm();
    reg = genasm(ap);
    printint(reg);
    postasm();
}