#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// Tokenkind 型, Token 型 の定義
typedef enum {
    TOKEN_SYMBOL,
    TOKEN_NUM,
    TOKEN_EOF,
} Tokenkind;

typedef struct Token Token;

// 連結リスト
struct Token {
    Tokenkind kind;      // token の種類
    Token *next;    // 次の token
    int val;        // kind == TOKEN_NUM の場合の数字
    char *str;      // 文字列
};

// input された code (グローバル変数)
char *user_input;

// token を表すグローバル変数
Token *token;

// エラー処理
void error_at(char *loc, char *fmt, ...) {
    va_list ap;     // 可変長引数を一つにまとめたもの
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");       // pos 個の空白
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// token が期待した symbol であれば、true を返す
// そうでなければ、false を返す
bool check(char op) {
    return (token->kind == TOKEN_SYMBOL) && (token->str[0] == op);
}

// true なら読み進めて true を返す
// false なら false を返す
bool consume(char op) {
    bool checked = check(op);
    if (checked) {
        token = token->next;
    } 
    return checked;
}

// true なら読み進める
// false なら error
void expect(char op) {
    if (check(op)) {
        token = token->next;
    } else {
        error_at(token->str, "'%c'ではありません", op);
    }
}

// token が数値ならば数値を返す　そうでなければ error
int expect_number() {
    if (token->kind != TOKEN_NUM) error_at(token->str, "整数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

// EOF かどうか判断
bool at_eof() {
    return token->kind == TOKEN_EOF;
}

// 新しい token 作成
Token *new_token(Tokenkind kind, Token *cur, char *str) {       // struct の pointer を返す
    Token *tok = calloc(1, sizeof(Token));      // memory 割り当て
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *code) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*code) {     // code が null pointer でないなら
        if (isspace(*code)) {
            code++;
            continue;
        }

        if (*code == '+' || *code == '-') {
            cur = new_token(TOKEN_SYMBOL, cur, code);
            code++;
            continue;
        }

        // strtol(str, pointer, num) : str を読み込み、pointer を一つずらし、 int に変換する
        if (isdigit(*code)) {
            cur = new_token(TOKEN_NUM, cur, code);
            cur->val = strtol(code, &code, 10); 
            continue;
        }

        error_at(code, "tokenize error");
    }

    // EOF token 生成
    new_token(TOKEN_EOF, cur, code);
    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    user_input = argv[1];        // input された文字列の pointer
    token = tokenize(user_input);   // tokenize した文字列

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("    mov rax, %d\n", expect_number());   

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }

        if (consume('-')) {
            printf("    sub rax, %d\n", expect_number());
            continue;
        }
    }

    printf("    ret\n");
    return 0;
}