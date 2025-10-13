#include "9cc.h"

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

Node *new_ident() {
    Node *node = new_node(NODE_LOCAL_VAR);
    node->offset = (token->str[0] - 'a' + 1) * 8;
    token = token->next;
    return node;
}

// 構文解析 
Node *statement();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// statement = expr ';'
Node *statement() {
    Node *node = expr();
    expect(";");
    return node;
}

// expr = assign
Node *expr() {
    Node *node = assign();
    return node;
}

// assign = equality ('=' assign)?
Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node = new_binary(NODE_ASSIGN, node, assign());
    } else {
        return node;
    }
}

// equality = relational ( '==' relational | '!=' relational )*
Node *equality() {
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

// primary = '(' expr ')' | num | ident
Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    } else if (token->kind == TOKEN_IDENT) {
        return new_ident();
    } else if (token->kind == TOKEN_NUM) {
        return new_num(expect_number());
    } else {
        error_at(token->str, "parsing error");
    }
}

void program() {
    int i = 0;
    while (!at_eof()) {
        code[i] = statement();
        i += 1;
    }
    code[i] = NULL;
}