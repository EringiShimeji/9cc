/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   9cc.c                                              :+:      :+:    :+:   */ /*                                                    +:+ +:+         +:+     */
/*   By: smatsuo <smatsuo@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/02 03:27:15 by smatsuo           #+#    #+#             */
/*   Updated: 2023/10/02 03:44:09 by smatsuo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
# define _9CC_MAIN "_main"
# define _9CC_MOVZB "movzx"
#else
# define _9CC_MAIN "main"
# define _9CC_MOVZB "movzb"
#endif

#define _9CC_ASM_UNARY_OP_FORMAT(op, format, value) printf("  """ op " " format "\n", value)
#define _9CC_ASM_NULLARY_OP(op) printf("  """ op  "\n")
#define _9CC_ASM_UNARY_OP(op, operand) printf("  """ op " " operand "\n")
#define _9CC_ASM_BINARY_OP(op, lhs, rhs) printf("  """ op " " lhs ", " rhs "\n")
#define _9CC_VA_ASM(_1, _2, _3, NAME, ...) NAME
#define ASM(...) _9CC_VA_ASM(__VA_ARGS__, _9CC_ASM_BINARY_OP, _9CC_ASM_UNARY_OP, _9CC_ASM_NULLARY_OP)(__VA_ARGS__)
#define MOVZB(dst, src) ASM(_9CC_MOVZB, dst, src)
#define CMP(lhs, rhs) ASM("cmp", lhs, rhs)
#define _9CC_PUSH_FORMAT(format, value) _9CC_ASM_UNARY_OP_FORMAT("push", format, value)
#define _9CC_PUSH_REG(reg) ASM("push", reg)
#define _9CC_VA_PUSH(_1, _2, NAME, ...) NAME
#define PUSH(...) _9CC_VA_PUSH(__VA_ARGS__, _9CC_PUSH_FORMAT, _9CC_PUSH_REG)(__VA_ARGS__)
#define POP(reg) ASM("pop", reg)
#define RET() ASM("ret")

// トークンの種類
typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
}	TokenKind;

typedef struct Token	Token;

struct	Token {
	TokenKind	kind;
	Token	 	*next;
	int		 	val;
	char	 	*str;
	int		 	len;
};

Token	*token;
char	*user_input;

void	error_at(char *loc, char *fmt, ...) {
	va_list	ap;
	

	va_start(ap, fmt);

	int	pos = loc - user_input;

	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

bool	consume(char *op) {
	if (token->kind != TK_RESERVED || strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}

void	expect(char *op) {
	if (token->kind != TK_RESERVED || strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
		error_at(token->str, "'%s'ではありません", op);
	token = token->next;
}

int	expect_number(void) {
	if (token->kind != TK_NUM)
		error_at(token->str, "数ではありません");

	int	val = token->val;

	token = token->next;
	return val;
}

bool	at_eof(void) {
	return token->kind == TK_EOF;
}

Token	*new_token(TokenKind kind, Token *cur, char *str, int len) {
	Token	*tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

bool	startswith(char *p, char *q) {
	return memcmp(p, q, strlen(q)) == 0;
}

Token	*tokenize(void) {
	Token	head;
	Token	*cur;
	char	*p = user_input;

	head.next = NULL;
	cur = &head;

	while (*p) {
		if (isspace(*p)) {
			p++;
			continue ;
		}
		
		if (startswith(p, "==") || startswith(p, "!=") || startswith(p, ">=") ||
			startswith(p, "<=")) {
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
		}

		if (startswith(p, "+") || startswith(p, "-") || startswith(p, "*") ||
			startswith(p, "/") || startswith(p, "(") || startswith(p, ")") ||
			startswith(p, "<") || startswith(p, ">")) {
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue ;
		}

		if (isdigit(*p)) {
			char	*q = p;
			int		val = strtol(p, &p, 10);

			cur = new_token(TK_NUM, cur, q, p - q);
			cur->val = val;
			continue ;
		}

		error_at(p, "トークナイズできません");
	}
	new_token(TK_EOF, cur, p, 0);
	return head.next;
}

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
}	NodeKind;

typedef struct Node		Node;

struct	Node {
	NodeKind	kind;
	Node		*lhs;
	Node		*rhs;
	int			val;
};

Node	*new_node(NodeKind kind, Node *lhs, Node *rhs) {
	Node	*node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node	*new_node_num(int val) {
	Node	*node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

Node	*expr(void);
Node	*equality(void);
Node	*relational(void);
Node	*add(void);
Node	*mul(void);
Node	*unary(void);
Node	*primary(void);

Node	*expr(void) {
	return equality();
}

Node	*equality(void) {
	Node	*node = relational();

	for (;;) {
		if (consume("=="))
			node = new_node(ND_EQ, node, relational());
		else if (consume("!="))
			node = new_node(ND_NE, node, relational());
		else
			return node;
	}
}

Node	*relational(void) {
	Node	*node = add();

	for (;;) {
		if (consume("<="))
			node = new_node(ND_LE, node, add());
		else if (consume(">="))
			node = new_node(ND_LE, add(), node);
		else if (consume("<"))
			node = new_node(ND_LT, node, add());
		else if (consume(">"))
			node = new_node(ND_LT, add(), node);
		else
			return node;
	}
}

Node	*add(void) {
	Node	*node;

	node = mul();
	for (;;) {
		if (consume("+"))
			node = new_node(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return node;
	}
}

Node	*mul(void) {
	Node	*node;

	node = unary();
	for (;;) {
		if (consume("*"))
			node = new_node(ND_MUL, node, unary());
		else if (consume("/"))
			node = new_node(ND_DIV, node, unary());
		else
			return node;
	}
}

Node	*unary(void) {
	if (consume("+"))
		return primary();
	if (consume("-"))
		return new_node(ND_SUB, new_node_num(0), primary());
	return primary();
}

Node	*primary(void) {
	Node	*node;

	if (consume("(")) {
		node = expr();
		expect(")");
		return node;
	}

	return new_node_num(expect_number());
}

void	gen(Node *node) {
	if (node->kind == ND_NUM) {
		PUSH("%d", node->val);
		return ;
	}

	gen(node->lhs);
	gen(node->rhs);

	POP("rdi");
	POP("rax");

	switch (node->kind) {
	case ND_ADD:
		ASM("add", "rax", "rdi");
		break ;
	case ND_SUB:
		ASM("sub", "rax", "rdi");
		break ;
	case ND_MUL:
		ASM("imul", "rax", "rdi");
		break ;
	case ND_DIV:
		ASM("cqo");
		ASM("idiv", "rdi");
		break ;
	case ND_EQ:
		CMP("rax", "rdi");
		ASM("sete", "al");
		MOVZB("rax", "al");
		break ;
	case ND_NE:
		CMP("rax", "rdi");
		ASM("setne", "al");
		MOVZB("rax", "al");
		break ;
	case ND_LE:
		CMP("rax", "rdi");
		ASM("setle", "al");
		MOVZB("rax", "al");
		break ;
	case ND_LT:
		CMP("rax", "rdi");
		ASM("setl", "al");
		MOVZB("rax", "al");
		break ;
	}

	PUSH("rax");
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	user_input = argv[1];
	token = tokenize();

	Node	*node = expr();

	printf(".intel_syntax noprefix\n");
	printf(".globl %s\n", _9CC_MAIN);
	printf("%s:\n", _9CC_MAIN);

	gen(node);
	POP("rax");
	RET();
	return 0;
}
