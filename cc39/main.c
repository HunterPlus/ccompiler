#include "c.h"

int main(int argc, char *argv[]) {
    if (argc != 2) 
        error("%s, invalid number of arguments", argv[0]);

    Token *tok = tokenize_file(argv[1]);
    Obj *prog = parse(tok);
    
    /* traverse the ast to emit assembly. */
    codegen(prog);
    return 0;
}
