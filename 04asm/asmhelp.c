#include "global.h"

extern FILE *outfile;

static int regs[4];
static char *regtab[] = {"%r8", "%r9", "%r10", "%r11"};

static int regalloc(void)
{
    for (int i = 0; i < 4; i++)
        if (regs[i]) {
            regs[i] = 0;
            return i;
        }
    fprintf(stderr, "alloc register error: out of registers\n");
    exit(1);
}
void regfreeall(void)
{
    for (int i = 0; i < 4; i++)
        regs[i] = 1;
}
void regfree(int i)
{
    if (regs[i] != 0) {
        fprintf(stderr, "regfree error: regs[%d] not in use\n", i);
        exit(1);
    }
    regs[i] = 1;
}
/* print out assemble header code */
void preasm()
{
    regfreeall();
    fputs(
        "\t.text\n"
        ".LC0:\n"
        "\t.string\t\"%d\\n\"\n"
        "printint:\n"
        "\tpushq\t%rbp\n"
        "\tmovq\t%rsp, %rbp\n"
        "\tsubq\t$16, %rsp\n"
        "\tmovl\t%edi, -4(%rbp)\n"
        "\tmovl\t-4(%rbp), %eax\n"
        "\tmovl\t%eax, %esi\n"
        "\tleaq\t.LC0(%rip), %rdi\n"
        "\tmovl\t$0, %eax\n"
        "\tcall\tprintf@PLT\n"
        "\tnop\n"
        "\tleave\n"
        "\tret\n"
        "\n"
        "\t.global\tmain\n"
        "\t.type\tmain, @function\n"
        "main:\n"
        "\tpushq\t%rbp\n"
        "\tmovq\t%rsp, %rbp\n",
    outfile);
}
/* print out assemble tail code */
void postasm()
{
    fputs(
        "\tmovl\t$0, %eax\n"
        "\tpopq\t%rbp\n"
        "\tret\n",
    outfile);
}
int regload(int val)
{
    int r = regalloc();
    
    fprintf(outfile, "\tmovq\t$%d, %s\n", val, regtab[r]);
    return r;
}
int regadd(int r1, int r2)
{
    fprintf(outfile, "\taddq\t%s, %s\n", regtab[r1], regtab[r2]);
    regfree(r1);
    return r2;
}
int regsub(int r1, int r2)
{
    fprintf(outfile, "\tsubq\t%s, %s\n", regtab[r2], regtab[r1]);
    regfree(r2);
    return r1;
}
int regmul(int r1, int r2)
{
    fprintf(outfile, "\timulq\t%s, %s\n", regtab[r1], regtab[r2]);
    regfree(r1);
    return r2;
}
int regdiv(int r1, int r2)
{
    fprintf(outfile, "\tmovq\t%s, %%rax\n", regtab[r1]);
    fprintf(outfile, "\tcqo\n");
    fprintf(outfile, "\tidivq\t%s\n", regtab[r2]);
    fprintf(outfile, "\tmovq\t%%rax, %s\n", regtab[r1]);
    regfree(r2);
    return r1;
}
void printint(int r)
{
    fprintf(outfile, "\tmovq\t%s, %%rdi\n", regtab[r]);
    fprintf(outfile, "\tcall\tprintint\n");
    regfree(r);
}






