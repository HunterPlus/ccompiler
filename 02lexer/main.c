#include "global.h"

FILE *infile;
int lineno = 1;

Token *nexttoken(void);

int main(int argc, char *argv[])
{
    Token *tp;
    
    if (argc != 2 || (infile = fopen(argv[1], "r")) == NULL) {
        printf("fopen file %s error\n", argv[1]);
        exit(1);
    }
    while ((tp = nexttoken()) && tp->type != T_EOF)
        printf("%s\t", tp->text);
    printf("\n%s\n", tp->text);
    fclose(infile);
    return 0;
}
