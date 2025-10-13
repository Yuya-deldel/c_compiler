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

bool is_start_with(char *p, char *str) {
    return strncmp(p, str, strlen(str)) == 0;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {     // p が null pointer でないなら
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (is_start_with(p, "==")) {
            cur = new_token(TOKEN_SYMBOL, cur, p, 2);
            p += 2;
            continue;
        }

        if (is_start_with(p, "!=")) {
            cur = new_token(TOKEN_SYMBOL, cur, p, 2);
            p += 2;
            continue;
        }

        if (is_start_with(p, "<=")) {
            cur = new_token(TOKEN_SYMBOL, cur, p, 2);
            p += 2;
            continue;
        }

        if (is_start_with(p, ">=")) {
            cur = new_token(TOKEN_SYMBOL, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>", *p)) {       // arg_2 に arg_1 の文字列があるかチェックする
            cur = new_token(TOKEN_SYMBOL, cur, p, 1);
            p++;
            continue;
        }

        // strtol(str, pointer, num) : str を読み込み、pointer を一つずらし、 int に変換する
        if (isdigit(*p)) {
            cur = new_token(TOKEN_NUM, cur, p, 0);
            char *ptr = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - ptr; 
            continue;
        }

        // variables (one character)
        if ('a' <= *p && *p <= 'z') {
            cur = new_token(TOKEN_IDENT, cur, p, 1);
            p += 1;
            continue;
        }

        error_at(p, "tokenize error");
    }

    // EOF token 生成
    new_token(TOKEN_EOF, cur, p, 0);
    return head.next;
}