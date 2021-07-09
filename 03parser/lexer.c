#include "global.h"

#define K   64

static Token tokbuf[K];
static int tf = -1;     /* token queue front index */
static int tr = -1;     /* token queue rear index */
static int tcnt = 0;

void enqtoken(void);
Token *nexttoken()
{
    if (tcnt == 0)
        enqtoken();
    tf = ++tf % K;
    tcnt--;
    return &tokbuf[tf];
}
Token *LT(int n)       /* Look Ahead i token, return token pointer */
{
    int i;
    Token *tp;
    
    if (n > K - 1) {
        fprintf(stderr, "LT error: Look Ahead %d greater than cap", n);
        exit(1);
    }
    i = n - tcnt;
    while (i-- > 0)
        enqtoken();
    tp = &tokbuf[(tf + n) % K];
    return tp;
}
int LA(int n)           /* Look Ahead i token, return token type */
{
    return LT(n)->type;
}
void enqtoken()         /* get token into tokbuf queue */
{
    void gettoken(Token *, int);
    Token *tp;
    
    if (tcnt == K - 1) {
        fprintf(stderr, "enqtoken error: buffer full\n");
        exit(1);
    }
    tcnt++;
    tr = ++tr % K;
    tp = &tokbuf[tr];
    gettoken(tp, K);
    return;
}

