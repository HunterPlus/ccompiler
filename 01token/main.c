#include "global.h"

FILE *infile;
int lineno = 1;

void gettoken(Token *, int);

int main(int argc, char *argv[])
{
    Token token;
    
    if (argc != 2 || (infile = fopen(argv[1], "r")) == NULL) {
        printf("fopen file %s error\n", argv[1]);
        exit(1);
    }
    while (1) {
        gettoken(&token, TOKENSIZ);
        printf("%d\t%s\n", token.type, token.text);
        if (token.type == T_EOF)
            break;
    }
    fclose(infile);
    return 0;
}
