#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#define _9CC_MAIN "_main"
#define _9CC_MOVZB "movzx"
#else
#define _9CC_MAIN "main"
#define _9CC_MOVZB "movzb"
#endif

#define _9CC_ASM_UNARY_OP_FORMAT(op, format, value) \
	printf(                                         \
		"  "                                        \
		"" op " " format "\n",                      \
		value)
#define _9CC_ASM_NULLARY_OP(op) \
	printf(                     \
		"  "                    \
		"" op "\n")
#define _9CC_ASM_UNARY_OP(op, operand) \
	printf(                            \
		"  "                           \
		"" op " " operand "\n")
#define _9CC_ASM_BINARY_OP(op, lhs, rhs) \
	printf(                              \
		"  "                             \
		"" op " " lhs ", " rhs "\n")
#define _9CC_VA_ASM(_1, _2, _3, NAME, ...) NAME
#define ASM(...)                                                    \
	_9CC_VA_ASM(__VA_ARGS__, _9CC_ASM_BINARY_OP, _9CC_ASM_UNARY_OP, \
				_9CC_ASM_NULLARY_OP)                                \
	(__VA_ARGS__)
#define MOVZB(dst, src) ASM(_9CC_MOVZB, dst, src)
#define CMP(lhs, rhs) ASM("cmp", lhs, rhs)
#define _9CC_PUSH_FORMAT(format, value) \
	_9CC_ASM_UNARY_OP_FORMAT("push", format, value)
#define _9CC_PUSH_REG(reg) ASM("push", reg)
#define _9CC_VA_PUSH(_1, _2, NAME, ...) NAME
#define PUSH(...) \
	_9CC_VA_PUSH(__VA_ARGS__, _9CC_PUSH_FORMAT, _9CC_PUSH_REG)(__VA_ARGS__)
#define POP(reg) ASM("pop", reg)
#define RET() ASM("ret")

// トークンの種類
typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
	TokenKind kind;
	Token	 *next;
	int		  val;
	char	 *str;
	int		  len;
};

typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_NUM,
	ND_EQ,
	ND_NE,
	ND_LE,
	ND_LT,
} NodeKind;

typedef struct Node Node;

struct Node {
	NodeKind kind;
	Node	*lhs;
	Node	*rhs;
	int		 val;
};

Token *token;
char  *user_input;

Token *tokenize(void);
Node  *parse();
void   gen(Node *node);
