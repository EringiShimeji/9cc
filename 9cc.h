#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"

// トークンの種類
typedef enum {
	TK_RESERVED,
	TK_IDENT,
	TK_NUM,
	TK_EOF,
	TK_RETURN,
} TokenKind;

typedef struct Token Token;

struct Token {
	TokenKind kind;
	Token	 *next;
	int		  val;
	char	 *str;
	int		  len;
};

typedef struct LVar LVar;

struct LVar {
	LVar *next;
	char *name;
	int	  len;
	int	  offset;
};

LVar *locals;

typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_EQ,
	ND_NE,
	ND_LE,
	ND_LT,
	ND_ASSIGN,
	ND_LVAR,
	ND_NUM,
	ND_RETURN,
} NodeKind;

typedef struct Node Node;

struct Node {
	NodeKind kind;
	Node	*lhs;
	Node	*rhs;
	int		 val;
	int		 offset;
};

Token *token;
char  *user_input;
Node  *code[100];

Token *tokenize(void);
void   program();
void   error_at(char *loc, char *fmt, ...);
void   gen(Node *node);
