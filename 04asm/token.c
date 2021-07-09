#include "global.h"

extern FILE *infile;
extern int lineno;

int getch(void);
void ungetch(int);
/* get token from input */
void gettoken(Token *token, int lim)
{
    int c;
    char *w;
    
    w = token->text;
    while (isspace(c = getch()))
        ;
    if (c == EOF) {
        token->type = T_EOF;
        strcpy(w, "T_EOF");
        return;
    }
    *w++ = c;
    if (!isalnum(c) && c != '_') {
        switch (c) {
        case '+':   token->type = T_PLUS;  break;
        case '-':   token->type = T_MINUS; break;
        case '*':   token->type = T_STAR;  break;
        case '/':   token->type = T_SLASH; break;
        
        default:
            fprintf(stderr, "line(%d): unknown tokan %c\n", lineno, c);
            exit(1);
        }
        *w = '\0';
        return;
    } else if (isdigit(c)) {
        token->type = T_NUM;
        for (; --lim > 0; w++) 
            if (!isdigit(*w = getch())) {
                ungetch(*w);
                *w = '\0';
                return;
            }
        
    } else {
        token->type = T_NAME;
        for (; --lim > 0; w++)
            if (!isalnum(c = getch()) && c != '_') {
                ungetch(*w);
                *w = '\0';
                return;
            }
    }

}

#define BUFSIZE 100

static char buf[BUFSIZE];      // buffer for ungetch
static int bufp = 0;           // next free position in buf

int getch(void)        // get a (possibly pushed back) character
{
    int c;
    
    c = (bufp > 0) ? buf[--bufp] : getc(infile);
    if (c == '\n')
        lineno++;
    return c;
}
void ungetch(int c)    // push character back on input
{
    if (c == '\n')
        lineno--;
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters.\n");
    else
        buf[bufp++] = c;
}

void ungets(char s[])
{
    int l = 0;
    while (s[l] != '\0')
        l++;    
    while (l > 0)
        ungetch(s[--l]);
}

