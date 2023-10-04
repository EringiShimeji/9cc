#include "9cc.h"

void gen_lval(Node *node) {
	if (node->kind != ND_LVAR)
		error_at(user_input, "代入の左辺値が変数ではありません");
	MOV("rax", "rbp");
	ASM("sub", "rax", "%d", node->offset);
	PUSH("rax");
}

void gen(Node *node) {
	switch (node->kind) {
		case ND_NUM:
			PUSH("%d", node->val);
			return;
		case ND_LVAR:
			gen_lval(node);
			POP("rax");
			MOV("rax", "[rax]");
			PUSH("rax");
			return;
		case ND_ASSIGN:
			gen_lval(node->lhs);
			gen(node->rhs);
			POP("rdi");
			POP("rax");
			MOV("[rax]", "rdi");
			PUSH("rdi");
			return;
		case ND_RETURN:
			gen(node->lhs);
			POP("rax");
			MOV("rsp", "rbp");
			POP("rbp");
			RET();
			return;
	}

	gen(node->lhs);
	gen(node->rhs);

	POP("rdi");
	POP("rax");

	switch (node->kind) {
		case ND_ADD:
			ASM("add", "rax", "rdi");
			break;
		case ND_SUB:
			ASM("sub", "rax", "rdi");
			break;
		case ND_MUL:
			ASM("imul", "rax", "rdi");
			break;
		case ND_DIV:
			ASM("cqo");
			ASM("idiv", "rdi");
			break;
		case ND_EQ:
			CMP("rax", "rdi");
			ASM("sete", "al");
			MOVZB("rax", "al");
			break;
		case ND_NE:
			CMP("rax", "rdi");
			ASM("setne", "al");
			MOVZB("rax", "al");
			break;
		case ND_LE:
			CMP("rax", "rdi");
			ASM("setle", "al");
			MOVZB("rax", "al");
			break;
		case ND_LT:
			CMP("rax", "rdi");
			ASM("setl", "al");
			MOVZB("rax", "al");
			break;
	}

	PUSH("rax");
}
