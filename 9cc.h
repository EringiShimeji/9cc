#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#define MAIN "_main"
#define MOVZB "movzx"
#else
#define MAIN "main"
#define MOVZB "movzb"
#endif

// トークンの種類
typedef enum {
	TK_RESERVED,
	TK_IDENT,
	TK_NUM,
	TK_EOF,
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
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
	ND_IF,
	ND_WHILE,
	ND_FOR,
	ND_BLOCK,
	ND_CALL
} NodeKind;

typedef struct Node Node;

struct Node {
	NodeKind kind;
	Node	*lhs;
	Node	*rhs;
	int		 val;
	int		 offset;
	Node	*init;
	Node	*condition;
	Node	*update;
	Node	*next;
	char	*func_name;
	int		 len;
	Node	*args;
};

Token *token;
char  *user_input;
Node  *code[100];

void   debug_token(const char *label);
Token *tokenize(void);
void   program();
void   error_at(char *loc, char *fmt, ...);
void   gen(Node *node);
void   println(char *fmt, ...);
