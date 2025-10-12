#include "9cc.h"

// 新しい token 作成
Token *new_token(Tokenkind kind, Token *cur, char *str, int len) {       // struct の pointer を返す
    Token *tok = calloc(1, sizeof(Token));      // memory 割り当て
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool is_start_with(char *code, char *str) {
    return strncmp(code, str, strlen(str)) == 0;
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

        if (is_start_with(code, "==")) {
            cur = new_token(TOKEN_SYMBOL, cur, code, 2);
            code += 2;
            continue;
        }

        if (is_start_with(code, "!=")) {
            cur = new_token(TOKEN_SYMBOL, cur, code, 2);
            code += 2;
            continue;
        }

        if (is_start_with(code, "<=")) {
            cur = new_token(TOKEN_SYMBOL, cur, code, 2);
            code += 2;
            continue;
        }

        if (is_start_with(code, ">=")) {
            cur = new_token(TOKEN_SYMBOL, cur, code, 2);
            code += 2;
            continue;
        }

        if (strchr("+-*/()<>", *code)) {      // arg_2 に arg_1 の文字列があるかチェックする
            cur = new_token(TOKEN_SYMBOL, cur, code, 1);
            code++;
            continue;
        }

        // strtol(str, pointer, num) : str を読み込み、pointer を一つずらし、 int に変換する
        if (isdigit(*code)) {
            cur = new_token(TOKEN_NUM, cur, code, 0);
            char *ptr = code;
            cur->val = strtol(code, &code, 10);
            cur->len = code - ptr; 
            continue;
        }

        error_at(code, "tokenize error");
    }

    // EOF token 生成
    new_token(TOKEN_EOF, cur, code, 0);
    return head.next;
}