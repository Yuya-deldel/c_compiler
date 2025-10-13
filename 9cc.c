#include "9cc.h"

// global variables (definition)
char *user_input;       // input された code
Token *token;
Node *code[100];

int main(int argc, char **argv) {
    if (argc != 2) {
        error("%s: invalid number of arguments", argv[0]);
    }

    user_input = argv[1];           // input された文字列の pointer
    token = tokenize(user_input);   // tokenize した文字列
    program();                      // parse

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    for (int i = 0; code[i]; i++) {     // code[i] == NULL になるまで
        gen(code[i]);
        printf("    pop rax\n");        // 残ったスタックの値の処理
    }       // 最後の式の結果は rax に残っている (返り値)
    
    printf("    mov rsp, rbp\n");   // スタックポインタを関数のベースポインタにセット
    printf("    pop rbp\n");        // 関数のリターンアドレスをベースポインタにセット
    printf("    ret\n");            // スタックからアドレスを pop -> アドレスに jump
    return 0;
}