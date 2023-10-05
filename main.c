#include "9cc.h"

const char *__asan_default_options() {
	return "detect_leaks=0";
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	user_input = argv[1];
	token = tokenize();
	program();

	println(".intel_syntax noprefix");
	println(".globl %s", MAIN);
	println("%s:", MAIN);

	println("  push rbp");
	println("  mov rbp, rsp");
	println("  sub rsp, %d", locals ? locals->offset : 0);

	for (int i = 0; code[i]; i++) {
		gen(code[i]);
	}
	println("  mov rsp, rbp");
	println("  pop rbp");
	println("  ret");
	return 0;
}
