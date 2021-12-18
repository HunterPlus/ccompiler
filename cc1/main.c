#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"%s, invalid number of arguments\n", argv[0]);
        return 1;
    }
    printf("\t.intel_syntax noprefix\n");
    printf("\t.global main\n");
    printf("main:\n");
    printf("\tmov\trax, %d\n", atoi(argv[1]));
    printf("\tret\n");
    return 0;
}

