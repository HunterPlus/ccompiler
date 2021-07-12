#include <stdio.h>
#define HASHSIZE    101

typedef struct Symbol {
    char *name;
    int type;
    int ref;
    struct Symbol *next;
} Symbol;
typedef struct Scope {
    struct Scope *up;
    Symbol *symtab[HASHSIZE];
} Scope;

void scopeinit(void);
Scope *scopepush(void);
void *scopepop(void);
void printsym(void);
Symbol *resolve(char *);
Symbol *lookup(char *);
Symbol *install(char *, int, int);

int main()
{
    scopeinit();
    printsym();
    
    Symbol *sp;
    
    sp = lookup("else");
    printf("\n%s, %d\n", sp->name, sp->type);
    printf("\n\n");
    
    scopepush();
    install("abc", 1, 1);
    install("aka", 2, 2);
    install("num", 3, 3);
    printsym();
    if ((sp = lookup("else")) == NULL)
        printf("lookup: can't find ident\n");
    sp = resolve("else");
    printf("resolve: %s\n", sp->name);
    scopepop();
    
    install("for", 8, 0);
    printf("\n\n");
    printsym();

    return 0;
}
