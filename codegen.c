#include "9cc.h"

void println(char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf("\n");
	va_end(ap);
}

void gen_lval(Node *node) {
	if (node->kind != ND_LVAR)
		error_at(user_input, "代入の左辺値が変数ではありません");
	println("  mov rax, rbp");
	println("  sub rax, %d", node->offset);
	println("  push rax");
}

static int label_id;

void gen(Node *node) {
	if (node == NULL)
		return;

	int cur_label;

	switch (node->kind) {
		case ND_NUM:
			println("  push %d", node->val);
			return;
		case ND_LVAR:
			gen_lval(node);
			println("  pop rax");
			println("  mov rax, [rax]");
			println("  push rax");
			return;
		case ND_ASSIGN:
			gen_lval(node->lhs);
			gen(node->rhs);
			println("  pop rdi");
			println("  pop rax");
			println("  mov [rax], rdi");
			println("  push rdi");
			return;
		case ND_RETURN:
			gen(node->lhs);
			println("  pop rax");
			println("  mov rsp, rbp");
			println("  pop rbp");
			println("  ret");
			return;
		case ND_IF:
			cur_label = label_id++;

			gen(node->condition);
			println("  pop rax");
			println("  cmp rax, 0");
			if (node->rhs) {
				println("  je .Lelse%.3d", cur_label);
				gen(node->lhs);
				println("  jmp .Lend%.3d", cur_label);
				println(".Lelse%.3d:", cur_label);
				gen(node->rhs);
				println(".Lend%.3d:", cur_label);
			} else {
				println("  je .Lend%.3d", cur_label);
				gen(node->lhs);
				println(".Lend%.3d:", cur_label);
			}
			return;
		case ND_WHILE:
			cur_label = label_id++;

			println(".Lbegin%.3d:", cur_label);
			gen(node->condition);
			println("  pop rax");
			println("  cmp rax, 0");
			println("  je .Lend%.3d", cur_label);
			gen(node->lhs);
			println("  jmp .Lbegin%.3d", cur_label);
			println(".Lend%.3d:", cur_label);
			return;
		case ND_FOR:
			cur_label = label_id++;

			gen(node->init);
			println(".Lbegin%.3d:", cur_label);
			if (node->condition) {
				gen(node->condition);
				println("  pop rax");
				println("  cmp rax, 0");
				println("  je .Lend%.3d", cur_label);
			}
			gen(node->lhs);
			gen(node->update);
			println("  jmp .Lbegin%.3d", cur_label);
			println(".Lend%.3d:", cur_label);
			return;
		case ND_BLOCK: {
			Node *cur = node->next;

			while (cur) {
				gen(cur);
				cur = cur->next;
			}

			return;
		}
	}

	gen(node->lhs);
	gen(node->rhs);

	println("  pop rdi");
	println("  pop rax");

	switch (node->kind) {
		case ND_ADD:
			println("  add rax, rdi");
			break;
		case ND_SUB:
			println("  sub rax, rdi");
			break;
		case ND_MUL:
			println("  imul rax, rdi");
			break;
		case ND_DIV:
			println("  cqo");
			println("  idiv rdi");
			break;
		case ND_EQ:
			println("  cmp rax, rdi");
			println("  sete al");
			println("  " MOVZB " rax, al");
			break;
		case ND_NE:
			println("  cmp rax, rdi");
			println("  setne al");
			println("  " MOVZB " rax, al");
			break;
		case ND_LE:
			println("  cmp rax, rdi");
			println("  setle al");
			println("  " MOVZB " rax, al");
			break;
		case ND_LT:
			println("  cmp rax, rdi");
			println("  setl al");
			println("  " MOVZB " rax, al");
			break;
	}

	println("  push rax");
}
