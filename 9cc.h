#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// error functions
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// Tokenization ////////////////////////////
// Tokenkind 型, Token 型 の定義
typedef enum {
    TOKEN_SYMBOL,
    TOKEN_NUM,
    TOKEN_EOF,
} Tokenkind;

// 連結リスト
typedef struct Token Token;
struct Token {
    Tokenkind kind;      // token の種類
    Token *next;    // 次の token
    int val;        // kind == TOKEN_NUM の場合の数字
    char *str;      // 文字列
    int len;        // 文字数
};

// global variables (declearation)
extern char *user_input;   // input された code
extern Token *token;

// tokenize functions
Token *tokenize(char *code);

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

// parser and code_generator functions
Node *expr();
void gen(Node *node);