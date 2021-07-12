#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static Scope gscope; 
static Scope *scp = &gscope;
static Symbol **gsym = gscope.symtab;
static char* keytab[] = { "char", "int", "long", "if", "else" };
static int keytype[] = { 1, 2, 3, 4, 5 };

void scopeinit();
void scopepop();
Scope *scopepush();
Symbol *lookup(char *s);
Symbol *install(char *name, int type, int keytype);


unsigned hash(char *s)
{
    unsigned hashval;
    
    for (hashval = 0; *s != '\0'; s++) 
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

void scopeinit()
{
    int n = sizeof(keytab) / sizeof(keytab[0]);

    for (int i = 0; i < n; i++)
        install(keytab[i], keytype[i], 0);
}
Scope *scopepush()
{
    Scope *scope;
    
    scope = (Scope *) malloc(sizeof(Scope));
    for (int i = 0; i < HASHSIZE; i++)
        scope->symtab[i] = NULL;
    scope->up = scp;
    scp = scope;
    return scp;
}
void scopepop()
{
    Scope *scope;
    Symbol *sp, *next;
    
    scope = scp;
    scp = scp->up;
    for (int i = 0; i < HASHSIZE; i++) 
        for (sp = scope->symtab[i]; sp != NULL; sp = next) {
            next = sp->next;
            free(sp);
        }
    free(scope);
}
Symbol *resolve(char *s)
{
    Symbol *sp;
    Scope *scope;
    
    for (scope = scp; scope != NULL; scope = scope->up)              
        for (sp = scope->symtab[hash(s)]; sp != NULL; sp = sp->next) 
            if (strcmp(s, sp->name) == 0)
                return sp;
    return NULL;
}
Symbol *lookup(char *s)
{
    Symbol *sp;
                  
    for (sp = scp->symtab[hash(s)]; sp != NULL; sp = sp->next) 
        if (strcmp(s, sp->name) == 0)
            return sp;
    return NULL;
}
Symbol *install(char *name, int type, int refkey)
{
    Symbol *sp;
    unsigned hashval;
    
    if ((sp = lookup(name)) == NULL) {
        sp = (Symbol *) malloc(sizeof(Symbol));
        if (sp != NULL && (sp->name = strdup(name)) != NULL) {
            sp->type = type;
            sp->ref = refkey;
            hashval = hash(name);
            sp->next = scp->symtab[hashval];
            scp->symtab[hashval] = sp;
            return sp;
        }
    }
    fprintf(stderr, "Symbol install error\n");
    exit(1);
}
void printsym(void)
{
    Symbol *sp;
    
    for (int i = 0; i < HASHSIZE; i++) 
        for (sp = scp->symtab[i]; sp != NULL; sp = sp->next)
            printf("%s %d\n", sp->name, sp->type);
}
