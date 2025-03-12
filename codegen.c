#include "9cc.h"

static void gen_expr(Node *node);
static void gen_stmt(Node *node);

static void gen_expr(Node *node) {
    switch (node->kind) {
    case ND_NUM:
      printf("    push %d\n", node->val);
      return;
    case ND_NEG:
      gen_expr(node->lhs);
      printf("    pop rax\n");
      printf("    neg rax\n");
      printf("    push rax\n");
      return;
    }

    gen_expr(node->lhs);
    gen_expr(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
    case ND_ADD:
      printf("    add rax, rdi\n");
      break;
    case ND_SUB:
      printf("    sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("    imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("    cqo\n");
      printf("    idiv rdi\n");
      break;
    case ND_EQ:
      printf("    cmp rax, rdi\n");
      printf("    sete al\n");
      printf("    movzb rax, al\n");
      break;
    case ND_NE:
      printf("    cmp rax, rdi\n");
      printf("    setne al\n");
      printf("    movzb rax, al\n");
      break;
    case ND_LT:
      printf("    cmp rax, rdi\n");
      printf("    setl al\n");
      printf("    movzb rax, al\n");
      break;
    case ND_LE:
      printf("    cmp rax, rdi\n");
      printf("    setle al\n");
      printf("    movzb rax, al\n");
      break;
    }

    printf("    push rax\n");
}

static void gen_stmt(Node *node) {
    if (node->kind == ND_EXPR_STMT) {
      gen_expr(node->lhs);
      return;
    }

    error("Invalid statement");
}

void codegen(Node *node) {
    // Print out the first half of assembly
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // Traverse the AST to emit assembly
    for (Node *n = node; n; n = n->next) {
        gen_stmt(n);
    }

    // A result must be at the top of the stack, so pop it
    // to RAX to make it a program exit code.
    printf("    pop rax\n");
    printf("    ret\n");
}