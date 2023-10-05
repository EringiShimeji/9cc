#include "9cc.h"

void error_at(char *loc, char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);

	int pos = loc - user_input;

	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

bool startswith(char *p, char *q) {
	return memcmp(p, q, strlen(q)) == 0;
}

bool is_alnum(char c) {
	return (isalnum(c) || c == '_');
}

bool is_keyword(char *p, char *q) {
	return startswith(p, q) && !is_alnum(p[strlen(q)]);
}

Token *tokenize() {
	Token  head;
	Token *cur;
	char  *p = user_input;

	head.next = NULL;
	cur = &head;

	while (*p) {
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (is_keyword(p, "return")) {
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}

		if (is_keyword(p, "if")) {
			cur = new_token(TK_IF, cur, p, 2);
			p += 2;
			continue;
		}

		if (is_keyword(p, "else")) {
			cur = new_token(TK_ELSE, cur, p, 4);
			p += 4;
			continue;
		}

		if (is_keyword(p, "while")) {
			cur = new_token(TK_WHILE, cur, p, 5);
			p += 5;
			continue;
		}

		if (is_keyword(p, "for")) {
			cur = new_token(TK_FOR, cur, p, 3);
			p += 3;
			continue;
		}

		if (startswith(p, "==") || startswith(p, "!=") || startswith(p, ">=") ||
			startswith(p, "<=")) {
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (startswith(p, "+") || startswith(p, "-") || startswith(p, "*") ||
			startswith(p, "/") || startswith(p, "(") || startswith(p, ")") ||
			startswith(p, "<") || startswith(p, ">") || startswith(p, ";") ||
			startswith(p, "=") || startswith(p, "{") || startswith(p, "}") ||
			startswith(p, ",")) {
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if (isdigit(*p)) {
			char *q = p;
			int	  val = strtol(p, &p, 10);

			cur = new_token(TK_NUM, cur, q, p - q);
			cur->val = val;
			continue;
		}

		if (isalpha(*p) || *p == '_') {
			char *q = p;
			while (is_alnum(*p))
				p++;
			cur = new_token(TK_IDENT, cur, q, p - q);
			continue;
		}

		error_at(p, "トークナイズできません");
	}
	new_token(TK_EOF, cur, p, 0);
	return head.next;
}

bool consume(TokenKind kind) {
	if (token->kind != kind)
		return false;
	token = token->next;
	return true;
}

bool consume_op(char *op) {
	if (token->kind != TK_RESERVED || strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}

Token *consume_ident() {
	if (token->kind != TK_IDENT)
		return NULL;
	Token *ident = token;
	token = token->next;
	return ident;
}

void expect(char *op) {
	if (token->kind != TK_RESERVED || strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
		error_at(token->str, "'%s'ではありません", op);
	token = token->next;
}

int expect_number() {
	if (token->kind != TK_NUM)
		error_at(token->str, "数ではありません");

	int val = token->val;

	token = token->next;
	return val;
}

bool at_eof() {
	return token->kind == TK_EOF;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void program() {
	int i = 0;
	while (!at_eof())
		code[i++] = stmt();
	code[i] = NULL;
}

Node *stmt() {
	Node *node = NULL;

	if (consume(TK_RETURN)) {
		node = new_node(ND_RETURN, expr(), NULL);
		expect(";");
	} else if (consume(TK_IF)) {
		node = new_node(ND_IF, NULL, NULL);

		expect("(");

		node->condition = expr();

		expect(")");

		node->lhs = stmt();

		if (consume(TK_ELSE))
			node->rhs = stmt();
	} else if (consume(TK_WHILE)) {
		node = new_node(ND_WHILE, NULL, NULL);

		expect("(");

		node->condition = expr();

		expect(")");

		node->lhs = stmt();
	} else if (consume(TK_FOR)) {
		node = new_node(ND_FOR, NULL, NULL);

		expect("(");

		if (!consume_op(";")) {
			node->init = expr();
			expect(";");
		}
		if (!consume_op(";")) {
			node->condition = expr();
			expect(";");
		}
		if (!consume_op(")")) {
			node->update = expr();
			expect(")");
		}

		node->lhs = stmt();
	} else if (consume_op("{")) {
		node = new_node(ND_BLOCK, NULL, NULL);
		if (consume_op("}"))
			return (node);

		Node *last_stmt = stmt();
		node->next = last_stmt;

		while (!consume_op("}")) {
			last_stmt->next = stmt();
			last_stmt = last_stmt->next;
		}
	} else if (!consume_op(";")) {
		node = expr();
		expect(";");
	}

	return node;
}

Node *expr() {
	return assign();
}

Node *assign() {
	Node *node = equality();

	while (consume_op("="))
		node = new_node(ND_ASSIGN, node, equality());
	return node;
}

Node *equality() {
	Node *node = relational();

	for (;;) {
		if (consume_op("=="))
			node = new_node(ND_EQ, node, relational());
		else if (consume_op("!="))
			node = new_node(ND_NE, node, relational());
		else
			return node;
	}
}

Node *relational() {
	Node *node = add();

	for (;;) {
		if (consume_op("<="))
			node = new_node(ND_LE, node, add());
		else if (consume_op(">="))
			node = new_node(ND_LE, add(), node);
		else if (consume_op("<"))
			node = new_node(ND_LT, node, add());
		else if (consume_op(">"))
			node = new_node(ND_LT, add(), node);
		else
			return node;
	}
}

Node *add() {
	Node *node;

	node = mul();
	for (;;) {
		if (consume_op("+"))
			node = new_node(ND_ADD, node, mul());
		else if (consume_op("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return node;
	}
}

Node *mul() {
	Node *node;

	node = unary();
	for (;;) {
		if (consume_op("*"))
			node = new_node(ND_MUL, node, unary());
		else if (consume_op("/"))
			node = new_node(ND_DIV, node, unary());
		else
			return node;
	}
}

Node *unary() {
	if (consume_op("+"))
		return primary();
	if (consume_op("-"))
		return new_node(ND_SUB, new_node_num(0), primary());
	return primary();
}

LVar *find_lvar(Token *tok) {
	for (LVar *var = locals; var; var = var->next) {
		if (tok->len == var->len && !memcmp(tok->str, var->name, var->len))
			return var;
	}
	return NULL;
}

Node *primary() {
	Node *node;

	if (consume_op("(")) {
		node = expr();
		expect(")");
		return node;
	}

	Token *tok = consume_ident();
	if (tok) {
		if (consume_op("(")) {
			node = new_node(ND_CALL, NULL, NULL);
			node->func_name = tok->str;
			node->len = tok->len;

			if (consume_op(")"))
				return NULL;

			Node *last_arg = expr();
			node->args = last_arg;
			for (; !consume_op(")"); last_arg = last_arg->next) {
				expect(",");
				last_arg->next = expr();
			}
			return node;
		}
		LVar *lvar = find_lvar(tok);
		Node *node = new_node(ND_LVAR, NULL, NULL);

		if (lvar) {
			node->offset = lvar->offset;
		} else {
			lvar = calloc(1, sizeof(LVar));
			lvar->next = locals;
			lvar->name = tok->str;
			lvar->len = tok->len;
			if (locals)
				lvar->offset = locals->offset + 8;
			else
				lvar->offset = 8;
			node->offset = lvar->offset;
			locals = lvar;
		}

		return node;
	}

	return new_node_num(expect_number());
}
