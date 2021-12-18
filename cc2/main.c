#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"%s, invalid number of arguments\n", argv[0]);
        return 1;
    }

    char *p = argv[1];

    printf("\t.intel_syntax noprefix\n");
    printf("\t.global main\n");
    printf("main:\n");
    printf("\tmov\trax, %ld\n", strtol(p, &p, 10));

    while (*p) {
        if (*p == '+') {
            p++;
            printf("\tadd\trax, %ld\n", strtol(p, &p, 10));            
        } else if (*p == '-') {
            p++;
            printf("\tsub\trax, %ld\n", strtol(p, &p, 10));            
        } else {
            fprintf(stderr, "unexpected character: '%c'\n", *p);
            return 1;
        }
    }
    printf("\tret\n");
    return 0;
}

