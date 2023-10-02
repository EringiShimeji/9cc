#include "9cc.h"

void gen(Node *node) {
	if (node->kind == ND_NUM) {
		PUSH("%d", node->val);
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
