#include "c.h"

static int depth;

static void push(void) {
    printf("\tpush\t%%rax\n");
    depth++;
}
static void pop(char *arg) {
    printf("\tpop\t%s\n", arg);
    depth--;
}
static int align_to(int n, int align) {
    return (n + align -1) / align * align;
}
static void gen_addr(Node *node) {
    if (node->kind == ND_VAR) {
        printf("\tlea\t%d(%%rbp), %%rax\n", node->var->offset);
        return;
    }
    error("not an lvalue");
}
static void gen_expr(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        printf("\tmov\t$%d, %%rax\n", node->val);
        return;
    case ND_NEG:
        gen_expr(node->lhs);
        printf("\tneg\t%%rax\n");
        return;
    case ND_VAR:
        gen_addr(node);
        printf("\tmov\t(%%rax), %%rax\n");
        return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        pop("%rdi");
        printf("\tmov\t%%rax, (%%rdi)\n");
        return;
    }

    gen_expr(node->rhs);
    push();
    gen_expr(node->lhs);
    pop("%rdi");

    switch (node->kind) {
    case ND_ADD:
        printf("\tadd\t%%rdi, %%rax\n");    return;
    case ND_SUB:
        printf("\tsub\t%%rdi, %%rax\n");    return;
    case ND_MUL:
        printf("\timul\t%%rdi, %%rax\n");   return;
    case ND_DIV:
        printf("\tcqo\n");
        printf("\tidiv\t%%rdi\n");          return;
    case ND_EQ:     case ND_NE:
    case ND_LT:     case ND_LE:
        printf("\tcmp\t%%rdi, %%rax\n");

        if (node->kind == ND_EQ)
            printf("\tsete\t%%al\n");
        else if (node->kind == ND_NE)
            printf("\tsetne\t%%al\n");
        else if (node->kind == ND_LT)
            printf("\tsetl\t%%al\n");
        else if (node->kind == ND_LE)
            printf("\tsetle\t%%al\n");

        printf("\tmovzb\t%%al, %%rax\n");
        return;    
    }
    error("invalid expression");
}
static void gen_stmt(Node *node) {    
    switch (node->kind) {
    case ND_BLOCK:
        for (Node *n = node->body; n; n = n->next)
            gen_stmt(n);
            return;
    case ND_RETURN:
        gen_expr(node->lhs);
        printf("\tjmp\t.L.return\n");
        return;
    case ND_EXPR_STMT:
        gen_expr(node->lhs);
        return;
    }
    error("invalid statement");
}
/* assign offsets to local variables. */
static void assign_lvar_offsets(Function *prog) {
    int offset = 0;
    for (Obj *var = prog->locals; var; var = var->next) {
        offset += 8;
        var->offset = -offset;
    }
    prog->stack_size = align_to(offset, 16);
}
void codegen(Function *prog) {
    assign_lvar_offsets(prog);

    printf("\t.globl\tmain\n");
    printf("main:\n");

    /* prologue */
    printf("\tpush\t%%rbp\n");
    printf("\tmov\t%%rsp, %%rbp\n");
    printf("\tsub\t$%d, %%rsp\n", prog->stack_size);

    gen_stmt(prog->body);
    assert(depth == 0);

    printf(".L.return:\n");
    printf("\tmov\t%%rbp, %%rsp\n");
    printf("\tpop\t%%rbp\n");
    printf("\tret\n");
}
