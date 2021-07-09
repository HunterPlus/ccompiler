#include "global.h"

FILE *infile;
FILE *outfile;
int lineno = 1;

Token *nexttoken(void);
Anode *binexpr(int);
int interp(Anode *);
void inorder(Anode *);

void gencode(Anode *);

int main(int argc, char *argv[])
{
    Anode *ap;
    
    if (argc != 2 || (infile = fopen(argv[1], "r")) == NULL
                    || (outfile = fopen("out.s", "w")) == NULL) {
        printf("fopen file %s error\n", argv[1]);
        exit(1);
    }
    ap = binexpr(0);
    printf("binexpr = %d\n", interp(ap));
    inorder(ap);
    gencode(ap);
    fclose(infile);
    return 0;
}
