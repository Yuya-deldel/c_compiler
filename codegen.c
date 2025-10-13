#include "9cc.h"

void gen_left_value(Node *node) {
    if (node->kind != NODE_LOCAL_VAR) error("左辺が変数ではありません");

    printf("    mov rax, rbp\n");       // rbp: 現在の関数フレームの開始位置
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");       // 変数のアドレスを push
}

void gen(Node *node) {
    switch (node->kind) {
        case NODE_NUM:
            printf("    push %d\n", node->val);
            return;
        case NODE_LOCAL_VAR:
            gen_left_value(node);           // 変数のアドレスを push
            printf("    pop rax\n");        // 変数のアドレス取り出し
            printf("    mov rax, [rax]\n");     // rax の値をアドレスとみなして rax にロード
            printf("    push rax\n");       // 変数の値を push
            return;
        case NODE_ASSIGN:
            gen_left_value(node->lhs);      // 変数のアドレスを push
            gen(node->rhs);                 // 右辺値を push

            printf("    pop rdi\n");        // 右辺値 -> rdi
            printf("    pop rax\n");        // 変数のアドレス -> rax
            printf("    mov [rax], rdi\n"); // rax の値をアドレスとみなして rdi をストア (代入)
            printf("    push rdi\n");       // 右辺値を push (ex. int a = b = 1)
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