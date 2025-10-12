#include "9cc.h"

// global variables (definition)
char *user_input;       // input された code
Token *token;

int main(int argc, char **argv) {
    if (argc != 2) {
        error("%s: invalid number of arguments", argv[0]);
    }

    user_input = argv[1];        // input された文字列の pointer
    token = tokenize(user_input);   // tokenize した文字列
    Node *node = expr();        // parsing

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}