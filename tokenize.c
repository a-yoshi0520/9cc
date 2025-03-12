#include "9cc.h"

// input string
static char *current_input;

static Token *new_token(TokenKind kind, Token *cur, char *loc, int len);
static bool startswith(char *p, char *q);

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// Reports an error location and exit.
static void verror_at(char *loc, char *fmt, va_list ap) {
    int pos = loc - current_input;
    fprintf(stderr, "%s\n", current_input);
    fprintf(stderr, "%*s", pos, ""); // print pos spaces.
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// Teports an error location and exit.
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(loc, fmt, ap);
}

void error_tok(Token *tok, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(tok->loc, fmt, ap);
}


// Consumes the current token if it matches `op`
bool equal(Token *tok, char *op) {
    return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

// Ensure that the current token is `op`
Token *skip(Token *tok, char *op) {
    if (!equal(tok, op)) {
        error_tok(tok, "expected '%s'", op);
    }
    return tok->next;
}

// 新しいトークンを作成してcurに繋げる
static Token *new_token(TokenKind kind, Token *cur, char *loc, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = loc;
    tok->len = len;
    cur->next = tok;
    return tok;
}

static bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *current_input) {
    char *p = current_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        //空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        // Multi-letter punctuator
        if (startswith(p, "==") || startswith(p, "!=") ||
           startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // Single-letter punctuator
        if (strchr("+-*/()<>", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        //Numeric literal
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "invalid token");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
