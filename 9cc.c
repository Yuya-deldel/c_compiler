#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// Tokenization ////////////////////////////
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
    int len;        // 文字数
};

// input された code (グローバル変数)
char *user_input;

// token を表すグローバル変数
Token *token;

// エラー処理
void error(char *fmt, ...) {
    va_list ap;     // 可変長引数を一つにまとめたもの
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

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
bool check(char *op) {
    if ((token->kind == TOKEN_SYMBOL) && (token->len == strlen(op))) {
        return memcmp(token->str, op, token->len) == 0;
    } else {
        return false;
    }
}

// true なら読み進めて true を返す
// false なら false を返す
bool consume(char *op) {
    bool checked = check(op);
    if (checked) {
        token = token->next;
    } 
    return checked;
}

// true なら読み進める
// false なら error
void expect(char *op) {
    if (check(op)) {
        token = token->next;
    } else {
        error_at(token->str, "'%s'ではありません", op);
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

// Parser ///////////////////////////////////////
typedef enum {
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_DIV,
    NODE_NUM,
    NODE_EQ,
    NODE_NE,
    NODE_LT,
    NODE_LE,
} Nodekind;

// Abstract Syntax Tree node type
typedef struct Node Node;

struct Node {
    Nodekind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(Nodekind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(Nodekind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val) {
    Node *node = new_node(NODE_NUM);
    node->val = val;
    return node;
}

// 構文解析 
Node *expr();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// expr = relational ( '==' relational | '!=' relational )*
Node *expr() {
    Node *node = relational();
    for (;;) {
        if (consume("==")) {
            node = new_binary(NODE_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_binary(NODE_NE, node, relational());
        } else {
            return node;
        }
    }
}

// relational = add ( '<' add | '<=' add | '>' add | '>=' add )*
Node *relational() {
    Node *node = add();
    for (;;) {
        if (consume("<")) {
            node = new_binary(NODE_LT, node, add());
        } else if (consume("<=")) {
            node = new_binary(NODE_LE, node, add());
        } else if (consume(">")) {
            node = new_binary(NODE_LT, add(), node);
        } else if (consume(">=")) {
            node = new_binary(NODE_LE, add(), node);
        } else {
            return node;
        }
    }
}

// add = mul ('+' mul | '-' mul)*
Node *add() {
    Node *node = mul();
    for (;;) {
        if (consume("+")) {
            node = new_binary(NODE_ADD, node, mul());
        } else if (consume("-")) { 
            node = new_binary(NODE_SUB, node, mul());
        } else {
            return node;
        }
    }
}

// mul = unary ('*' unary | '/' unary)*
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*")) {
            node = new_binary(NODE_MUL, node, unary());
        } else if (consume("/")) {
            node = new_binary(NODE_DIV, node, unary());
        } else {
            return node;
        }
    }
}

// unary = ('+' | '-')? primary
Node *unary() {
    if (consume("+")) {
        return primary();
    } else if (consume("-")) {
        return new_binary(NODE_SUB, new_num(0), primary());
    } else {
        return primary();
    }
}

// primary = '(' expr ')' | num
Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    } else {
        return new_num(expect_number());
    }
}

// code generator /////////////////////////////////
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