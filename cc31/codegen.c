#include "c.h"

static int depth;
static char *argreg[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
static Obj *current_fn;

static void gen_expr(Node *node);

static int count(void) {
    static int i = 1;
    return i++;
}

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
    switch (node->kind) {
    case ND_VAR:
        if (node->var->is_local)    /*  local variable  */
            printf("\tlea\t%d(%%rbp), %%rax\n", node->var->offset);
        else                        /* global variable  */
            printf("\tlea\t%s(%%rip), %%rax\n", node->var->name);
        return;
    case ND_DEREF:
        gen_expr(node->lhs);
        return;
    }
    error_tok(node->tok, "not an lvalue");
}

/*  load a value from where %rax is pointing to.    */
static void load(Type *ty) {
    if (ty->kind == TY_ARRAY) {
        return;
    }

    printf("\tmov\t(%%rax), %%rax\n");
}
/*  store %rax to an address that the stack top is pointing to.     */
static void store(void) {
    pop("%rdi");
    printf("\tmov\t%%rax, (%%rdi)\n");
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
        load(node->ty);
        return;
    case ND_DEREF:
        gen_expr(node->lhs);
        load(node->ty);
        return;
    case ND_ADDR:
        gen_addr(node->lhs);
        return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        store();
        return;
    case ND_FUNCALL: {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_expr(arg);
            push();
            nargs++;
        }
        for (int i = nargs - 1; i >= 0; i--)
            pop(argreg[i]);
    
        printf("\tmov\t$0, %%rax\n");
        printf("\tcall\t%s\n", node->funcname);
        return;
    }
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
    error_tok(node->tok, "invalid expression");
}
static void gen_stmt(Node *node) {    
    switch (node->kind) {
    case ND_IF: {
        int c = count();
        gen_expr(node->cond);
        printf("\tcmp\t$0, %%rax\n");
        printf("\tje\t.L.else.%d\n", c);
        gen_stmt(node->then);
        printf("\tjmp\t.L.end.%d\n", c);
        printf(".L.else.%d:\n", c);
        if (node->els)
            gen_stmt(node->els);
        printf(".L.end.%d:\n", c);
        return;
    }
    case ND_FOR: {
        int c = count();
        if (node->init)
            gen_stmt(node->init);
        printf(".L.begin.%d:\n", c);
        if (node->cond) {
            gen_expr(node->cond);
            printf("\tcmp\t$0, %%rax\n");
            printf("\tje\t.L.end.%d\n", c);
        }
        gen_stmt(node->then);
        if (node->inc)
            gen_expr(node->inc);
        printf("\tjmp\t.L.begin.%d\n", c);
        printf(".L.end.%d:\n", c);
        return;
    }
    case ND_BLOCK:
        for (Node *n = node->body; n; n = n->next)
            gen_stmt(n);
        return;
    case ND_RETURN:
        gen_expr(node->lhs);
        printf("\tjmp\t.L.return.%s\n", current_fn->name);
        return;
    case ND_EXPR_STMT:
        gen_expr(node->lhs);
        return;
    }
    error_tok(node->tok, "invalid statement");
}
/* assign offsets to local variables. */
static void assign_lvar_offsets(Obj *prog) {
    for (Obj *fn = prog; fn; fn = fn->next) {
        if (!fn->is_function)
            continue;

        int offset = 0;
        for (Obj *var = fn->locals; var; var = var->next) {
            offset += var->ty->size;
            var->offset = -offset;
        }
        fn->stack_size = align_to(offset, 16);
    }
}
static void emit_data(Obj *prog) {
    for (Obj *var = prog; var; var = var->next) {
        if (var->is_function)
            continue;
        
        printf("\t.data\n");
        printf("\t.globl\t%s\n", var->name);
        printf("%s:\n", var->name);
        printf("\t.zero\t%d\n", var->ty->size);
    }
}
static void emit_text(Obj *prog) {
    assign_lvar_offsets(prog);

    for (Obj *fn = prog; fn; fn = fn->next) {
        if (!fn->is_function)
            continue;

        printf("\t.globl\t%s\n", fn->name);
        printf("\t.text\n");
        printf("%s:\n", fn->name);
        current_fn = fn;

        /*  prologue    */
        printf("\tpush\t%%rbp\n");
        printf("\tmov\t%%rsp, %%rbp\n");
        printf("\tsub\t$%d, %%rsp\n", fn->stack_size);

        /*  save passed by register arguments to the stack  */
        int i = 0;
        for (Obj *var = fn->params; var; var = var->next)
            printf("\tmov\t%s, %d(%%rbp)\n", argreg[i++], var->offset);

        /*  emit code   */
        gen_stmt(fn->body);
        assert(depth == 0);

        /* epilogue */
        printf(".L.return.%s:\n", fn->name);
        printf("\tmov\t%%rbp, %%rsp\n");
        printf("\tpop\t%%rbp\n");
        printf("\tret\n");
    }
}

void codegen(Obj *prog) {
    assign_lvar_offsets(prog);
    emit_data(prog);
    emit_text(prog);
}