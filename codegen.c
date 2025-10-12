#include "9cc.h"

void gen(Node *node) {
    if (node->kind == NODE_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
        case NODE_ADD:
            printf("    add rax, rdi\n");
            break;
        case NODE_SUB:
            printf("    sub rax, rdi\n");
            break;
        case NODE_MUL:
            printf("    imul rax, rdi\n");
            break;
        case NODE_DIV:
            printf("    cqo\n");        // idiv の準備: rax + rdx のレジスタを準備
            printf("    idiv rdi\n");   // rdi で割る
            break;                      // idiv は商を rax, 余りを rdx にセットする
        case NODE_EQ:
            printf("    cmp rax, rdi\n");   // rax == rdi ならば 
            printf("    sete al\n");        // AL (RAX の下位 8bit) に 1 をセット
            printf("    movzb rax, al\n");  // RAX の下位 8bit 以外をクリア
            break;
        case NODE_NE:
            printf("    cmp rax, rdi\n");   // rax != rdi ならば 
            printf("    setne al\n");       // AL (RAX の下位 8bit) に 1 をセット
            printf("    movzb rax, al\n");  // RAX の下位 8bit 以外をクリア
            break;
        case NODE_LT:
            printf("    cmp rax, rdi\n");   // rax < rdi ならば 
            printf("    setl al\n");        // AL (RAX の下位 8bit) に 1 をセット
            printf("    movzb rax, al\n");  // RAX の下位 8bit 以外をクリア
            break;
        case NODE_LE:
            printf("    cmp rax, rdi\n");   // rax <= rdi ならば 
            printf("    setle al\n");       // AL (RAX の下位 8bit) に 1 をセット
            printf("    movzb rax, al\n");  // RAX の下位 8bit 以外をクリア
            break;
    }

    printf("    push rax\n");
}