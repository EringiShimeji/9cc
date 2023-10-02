#include "9cc.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	user_input = argv[1];
	token = tokenize();

	Node *node = parse();

	printf(".intel_syntax noprefix\n");
	printf(".globl %s\n", _9CC_MAIN);
	printf("%s:\n", _9CC_MAIN);

	gen(node);
	POP("rax");
	RET();
	return 0;
}
