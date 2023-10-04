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

	printf(".intel_syntax noprefix\n");
	printf(".globl %s\n", _9CC_MAIN);
	printf("%s:\n", _9CC_MAIN);

	PUSH("rbp");
	MOV("rbp", "rsp");
	ASM("sub", "rsp", "%d", locals ? locals->offset : 0);

	for (int i = 0; code[i]; i++) {
		gen(code[i]);

		POP("rax");
	}
	MOV("rsp", "rbp");
	POP("rbp");
	RET();
	return 0;
}
