#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }
    char *code = argv[1];

    // strtol(str, pointer, num) : str を読み込み、pointer を一つずらし、 int に変換する
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("    mov rax, %ld\n", strtol(code, &code, 10));   

    while (*code) {     // code が null pointer でないなら
        if (*code == '+') {
            code += 1;
            printf("    add rax, %ld\n", strtol(code, &code, 10));
            continue;
        }

        if (*code == '-') {
            code += 1;
            printf("    sub rax, %ld\n", strtol(code, &code, 10));
            continue;
        }

        fprintf(stderr, "unexpected character: '%c'\n", *code);
        return 1;
    }

    printf("    ret\n");
    return 0;
}